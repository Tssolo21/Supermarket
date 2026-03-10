#include "backend/business_logic/financial_service.hpp"
#include "backend/dal/database.hpp"
#include "backend/dal/dal_product.hpp"
#include "backend/dal/dal_transaction.hpp"
#include "backend/dal/dal_expense.hpp"
#include <QDateTime>
#include <ctime>

namespace hero::business_logic {

QVariantMap FinancialService::calculateMetrics(int64_t startTime, int64_t endTime) {
    auto txs = getTransactionsBetween(startTime, endTime);
    auto exps = getExpensesBetween(startTime, endTime);

    double totalRevenue = 0.0;
    double totalCostOfGoodsSold = 0.0;
    double totalExpenses = 0.0;

    dal::DalProduct productDal;

    for (const auto& tx : txs) {
        totalRevenue += tx.getTotalAmount();
        
        for (const auto& item : tx.getItems()) {
            auto productOpt = productDal.getById(item.product_id);
            if (productOpt) {
                totalCostOfGoodsSold += (productOpt->getBuyPrice() * item.quantity);
            }
        }
    }

    for (const auto& exp : exps) {
        totalExpenses += exp.getAmount();
    }

    double grossProfit = totalRevenue - totalCostOfGoodsSold;
    double netProfit = grossProfit - totalExpenses;
    double profitMargin = totalRevenue > 0 ? (netProfit / totalRevenue) * 100.0 : 0.0;

    QVariantMap metrics;
    metrics["totalRevenue"] = totalRevenue;
    metrics["totalExpenses"] = totalExpenses;
    metrics["grossProfit"] = grossProfit;
    metrics["netProfit"] = netProfit;
    metrics["profitMargin"] = profitMargin;
    metrics["transactionCount"] = static_cast<int>(txs.size());

    return metrics;
}

QVariantMap FinancialService::getTodayMetrics() {
    QDateTime now = QDateTime::currentDateTime();
    QDateTime startOfDay = QDateTime(now.date(), QTime(0, 0, 0));
    return calculateMetrics(startOfDay.toSecsSinceEpoch(), now.toSecsSinceEpoch());
}

bool FinancialService::addExpense(const QString& category, const QString& description, double amount) {
    dal::DalExpense dal;
    models::Expense exp(-1, category.toStdString(), description.toStdString(), amount, std::time(nullptr));
    if (dal.create(exp) != -1) {
        emit metricsUpdated();
        return true;
    }
    return false;
}

std::vector<models::Transaction> FinancialService::getTransactionsBetween(int64_t start, int64_t end) {
    dal::DalTransaction dal;
    return dal.getBetweenDates(start, end);
}

std::vector<models::Expense> FinancialService::getExpensesBetween(int64_t start, int64_t end) {
    dal::DalExpense dal;
    return dal.getBetweenDates(start, end);
}

} // namespace hero::business_logic
