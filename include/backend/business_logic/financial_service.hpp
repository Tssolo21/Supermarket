#ifdef QT_CORE_LIB
#include <QObject>
#include <QVariantMap>
#endif
#include "backend/models/transaction.hpp"
#include "backend/models/expense.hpp"
#include <vector>
#include <cstdint>

namespace hero::business_logic {

#ifdef QT_CORE_LIB
class FinancialService : public QObject {
    Q_OBJECT
#else
class FinancialService {
#endif
public:
    static FinancialService& getInstance() {
        static FinancialService instance;
        return instance;
    }

    // Calculates financial metrics for a specific date range
#ifdef QT_CORE_LIB
    Q_INVOKABLE QVariantMap calculateMetrics(int64_t startTime, int64_t endTime);
    Q_INVOKABLE QVariantMap getTodayMetrics();
    Q_INVOKABLE bool addExpense(const QString& category, const QString& description, double amount);

signals:
    void metricsUpdated();
#else
    void calculateMetrics(int64_t startTime, int64_t endTime);
#endif

private:
    FinancialService() = default;
    ~FinancialService() = default;
    
    // Disable copy
    FinancialService(const FinancialService&) = delete;
    FinancialService& operator=(const FinancialService&) = delete;

    std::vector<models::Transaction> getTransactionsBetween(int64_t start, int64_t end);
    std::vector<models::Expense> getExpensesBetween(int64_t start, int64_t end);
};

} // namespace hero::business_logic
