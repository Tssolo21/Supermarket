#include <backend/models/supplier.hpp>
#include <ctime>

namespace hero::models {

Supplier::Supplier(SupplierId id, const std::string& name, const std::string& email,
                   const std::string& phone, const std::string& address)
    : id_(id), name_(name), email_(email), phone_(phone), address_(address) {
    created_at_ = std::time(nullptr);
}

json Supplier::toJson() const {
    json j;
    j["id"] = id_;
    j["name"] = name_;
    j["email"] = email_;
    j["phone"] = phone_;
    j["address"] = address_;
    j["created_at"] = created_at_;
    return j;
}

Supplier Supplier::fromJson(const json& j) {
    Supplier s;
    if (j.contains("id")) s.id_ = j["id"];
    if (j.contains("name")) s.name_ = j["name"];
    if (j.contains("email")) s.email_ = j["email"];
    if (j.contains("phone")) s.phone_ = j["phone"];
    if (j.contains("address")) s.address_ = j["address"];
    if (j.contains("created_at")) s.created_at_ = j["created_at"];
    return s;
}

} // namespace hero::models
