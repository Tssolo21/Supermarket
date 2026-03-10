#pragma once

#include <vector>
#include <optional>
#include <backend/models/category.hpp>

namespace hero::dal {

/**
 * @class DalCategory
 * @brief Data Access Layer for Category entity
 */
class DalCategory {
public:
    using Category = hero::models::Category;
    using CategoryId = Category::CategoryId;

    CategoryId create(const Category& category);
    std::optional<Category> getById(CategoryId id);
    std::vector<Category> getAll();
    bool update(const Category& category);
    bool deleteById(CategoryId id);
    std::vector<Category> searchByName(const std::string& name);

private:
    static constexpr const char* TABLE_NAME = "categories";
    bool createTableIfNotExists();
};

} // namespace hero::dal
