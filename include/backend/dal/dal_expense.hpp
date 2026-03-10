#pragma once
#include "backend/models/expense.hpp"
#include <vector>

namespace hero::dal {

class DalExpense {
public:
    static bool createTableIfNotExists();
    models::Expense::ExpenseId create(const models::Expense& expense);
    std::vector<models::Expense> getBetweenDates(int64_t start, int64_t end);
};

} // namespace hero::dal
