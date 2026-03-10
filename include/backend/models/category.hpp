#pragma once

#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace hero::models {

/**
 * @class Category
 * @brief Represents a product category
 */
class Category {
public:
    using CategoryId = int64_t;

    Category() = default;
    Category(CategoryId id, const std::string& name, const std::string& description = "");

    CategoryId getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    time_t getCreatedAt() const { return created_at_; }

    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& desc) { description_ = desc; }

    json toJson() const;
    static Category fromJson(const json& j);

private:
    CategoryId id_ = -1;
    std::string name_;
    std::string description_;
    time_t created_at_ = 0;
};

} // namespace hero::models
