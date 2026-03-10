#include <backend/models/category.hpp>
#include <ctime>

namespace hero::models {

Category::Category(CategoryId id, const std::string& name, const std::string& description)
    : id_(id), name_(name), description_(description) {
    created_at_ = std::time(nullptr);
}

json Category::toJson() const {
    json j;
    j["id"] = id_;
    j["name"] = name_;
    j["description"] = description_;
    j["created_at"] = created_at_;
    return j;
}

Category Category::fromJson(const json& j) {
    Category c;
    if (j.contains("id")) c.id_ = j["id"];
    if (j.contains("name")) c.name_ = j["name"];
    if (j.contains("description")) c.description_ = j["description"];
    if (j.contains("created_at")) c.created_at_ = j["created_at"];
    return c;
}

} // namespace hero::models
