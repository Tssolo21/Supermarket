#pragma once
#include "backend/models/transaction.hpp"
#include <vector>
#include <optional>

namespace hero::dal {

class DalTransaction {
public:
    static bool createTableIfNotExists();
    
    models::Transaction::TransactionId create(const models::Transaction& transaction);
    std::optional<models::Transaction> getById(models::Transaction::TransactionId id);
    std::vector<models::Transaction> getBetweenDates(int64_t start, int64_t end);
};

} // namespace hero::dal
