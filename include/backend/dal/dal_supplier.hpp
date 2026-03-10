#pragma once

#include <vector>
#include <optional>
#include <backend/models/supplier.hpp>

namespace hero::dal {

/**
 * @class DalSupplier
 * @brief Data Access Layer for Supplier entity
 */
class DalSupplier {
public:
    using Supplier = hero::models::Supplier;
    using SupplierId = Supplier::SupplierId;

    SupplierId create(const Supplier& supplier);
    std::optional<Supplier> getById(SupplierId id);
    std::vector<Supplier> getAll();
    bool update(const Supplier& supplier);
    bool deleteById(SupplierId id);
    std::vector<Supplier> searchByName(const std::string& name);

private:
    static constexpr const char* TABLE_NAME = "suppliers";
    bool createTableIfNotExists();
};

} // namespace hero::dal
