#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <unordered_set>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace hero::middleware {

/**
 * @class JwtAuthenticator
 * @brief JWT token generation and validation
 * 
 * Provides authentication and authorization middleware.
 */
class JwtAuthenticator {
public:
    struct TokenPayload {
        std::string user_id;
        std::string username;
        std::string role;
        std::string email;
        long long issued_at;
        long long expires_at;
    };

    // Token management
    std::string generateToken(const TokenPayload& payload, int expiry_seconds = 3600);
    std::optional<TokenPayload> validateToken(const std::string& token);
    bool isTokenExpired(const std::string& token);
    bool revokeToken(const std::string& token);

    // Utility
    static std::string getTokenFromHeader(const std::string& auth_header);
    static JwtAuthenticator& getInstance();

private:
    JwtAuthenticator() = default;
    
    static constexpr const char* SECRET_KEY = "hero_system_secret_key_2026";
    std::unordered_set<std::string> blacklisted_tokens_;
    
    std::string encodeBase64(const std::string& input);
    std::string decodeBase64(const std::string& input);
    std::string createSignature(const std::string& header, const std::string& payload);
};

} // namespace hero::middleware
