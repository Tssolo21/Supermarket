#pragma once

#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace hero::models {

/**
 * @class Expense
 * @brief Represents an operational expense (e.g., rent, utilities, employee salary)
 */
class Expense {
public:
    using ExpenseId = int64_t;

    Expense() = default;
    Expense(ExpenseId id, const std::string& category, const std::string& description, double amount, time_t date)
        : id_(id), category_(category), description_(description), amount_(amount), date_(date) {}

    ExpenseId getId() const { return id_; }
    const std::string& getCategory() const { return category_; }
    const std::string& getDescription() const { return description_; }
    double getAmount() const { return amount_; }
    time_t getDate() const { return date_; }

    void setCategory(const std::string& category) { category_ = category; }
    void setDescription(const std::string& description) { description_ = description; }
    void setAmount(double amount) { amount_ = amount; }
    void setDate(time_t date) { date_ = date; }

    json toJson() const {
        json j;
        j["id"] = id_;
        j["category"] = category_;
        j["description"] = description_;
        j["amount"] = amount_;
        j["date"] = date_;
        return j;
    }

    static Expense fromJson(const json& j) {
        Expense e;
        if (j.contains("id")) e.id_ = j["id"];
        if (j.contains("category")) e.category_ = j["category"];
        if (j.contains("description")) e.description_ = j["description"];
        if (j.contains("amount")) e.amount_ = j["amount"];
        if (j.contains("date")) e.date_ = j["date"];
        return e;
    }

private:
    ExpenseId id_ = -1;
    std::string category_;
    std::string description_;
    double amount_ = 0.0;
    time_t date_ = 0;
};

} // namespace hero::models
