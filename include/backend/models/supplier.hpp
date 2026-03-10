#pragma once

#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace hero::models {

/**
 * @class Supplier
 * @brief Represents a product supplier
 */
class Supplier {
public:
    using SupplierId = int64_t;

    Supplier() = default;
    Supplier(SupplierId id, const std::string& name, const std::string& email,
             const std::string& phone, const std::string& address);

    SupplierId getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getEmail() const { return email_; }
    const std::string& getPhone() const { return phone_; }
    const std::string& getAddress() const { return address_; }
    time_t getCreatedAt() const { return created_at_; }

    void setEmail(const std::string& email) { email_ = email; }
    void setPhone(const std::string& phone) { phone_ = phone; }
    void setAddress(const std::string& address) { address_ = address; }

    json toJson() const;
    static Supplier fromJson(const json& j);

private:
    SupplierId id_ = -1;
    std::string name_;
    std::string email_;
    std::string phone_;
    std::string address_;
    time_t created_at_ = 0;
};

} // namespace hero::models
