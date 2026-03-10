#include <backend/middleware/jwt_authenticator.hpp>
#include <nlohmann/json.hpp>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <cstring>

using json = nlohmann::json;

namespace hero::middleware {

JwtAuthenticator& JwtAuthenticator::getInstance() {
    static JwtAuthenticator instance;
    return instance;
}

std::string JwtAuthenticator::generateToken(const TokenPayload& payload, int expiry_seconds) {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto expiry_time = now_time + expiry_seconds;

    // Header
    json header;
    header["alg"] = "HS256";
    header["typ"] = "JWT";
    std::string header_str = header.dump();
    std::string header_b64 = encodeBase64(header_str);

    // Payload
    json payload_json;
    payload_json["user_id"] = payload.user_id;
    payload_json["username"] = payload.username;
    payload_json["role"] = payload.role;
    payload_json["email"] = payload.email;
    payload_json["iat"] = now_time;
    payload_json["exp"] = expiry_time;
    
    std::string payload_str = payload_json.dump();
    std::string payload_b64 = encodeBase64(payload_str);

    // Signature
    std::string signature = createSignature(header_b64, payload_b64);

    return header_b64 + "." + payload_b64 + "." + signature;
}

std::optional<JwtAuthenticator::TokenPayload> JwtAuthenticator::validateToken(const std::string& token) {
    size_t first_dot = token.find('.');
    size_t second_dot = token.find('.', first_dot + 1);

    if (first_dot == std::string::npos || second_dot == std::string::npos) {
        return std::nullopt;
    }

    std::string header_b64 = token.substr(0, first_dot);
    std::string payload_b64 = token.substr(first_dot + 1, second_dot - first_dot - 1);
    std::string signature = token.substr(second_dot + 1);

    // Check blacklist
    if (blacklisted_tokens_.find(token) != blacklisted_tokens_.end()) {
        return std::nullopt;
    }

    // Verify signature
    if (signature != createSignature(header_b64, payload_b64)) {
        return std::nullopt;
    }

    // Decode payload
    try {
        std::string payload_str = decodeBase64(payload_b64);
        auto payload_json = json::parse(payload_str);

        // Check expiration
        auto exp = payload_json["exp"].get<long long>();
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        
        if (exp < now) {
            return std::nullopt;
        }

        TokenPayload payload;
        payload.user_id = payload_json["user_id"];
        payload.username = payload_json["username"];
        payload.role = payload_json["role"];
        payload.email = payload_json["email"];
        payload.issued_at = payload_json["iat"];
        payload.expires_at = exp;

        return payload;
    } catch (...) {
        return std::nullopt;
    }
}

bool JwtAuthenticator::isTokenExpired(const std::string& token) {
    auto payload = validateToken(token);
    return !payload.has_value();
}

bool JwtAuthenticator::revokeToken(const std::string& token) {
    blacklisted_tokens_.insert(token);
    return true;
}

std::string JwtAuthenticator::getTokenFromHeader(const std::string& auth_header) {
    const std::string bearer = "Bearer ";
    if (auth_header.substr(0, bearer.length()) == bearer) {
        return auth_header.substr(bearer.length());
    }
    return "";
}

std::string JwtAuthenticator::encodeBase64(const std::string& input) {
    static const char* base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encoded;
    int val = 0;
    int valb = 0;

    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 6) {
            valb -= 6;
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
        }
    }

    if (valb > 0) {
        encoded.push_back(base64_chars[(val << (6 - valb)) & 0x3F]);
    }

    while (encoded.size() % 4) {
        encoded.push_back('=');
    }

    return encoded;
}

std::string JwtAuthenticator::decodeBase64(const std::string& input) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string decoded;
    std::vector<int> T(256, -1);

    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

    int val = 0, valb = 0;
    for (unsigned char c : input) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 8) {
            valb -= 8;
            decoded.push_back(char((val >> valb) & 0xFF));
        }
    }

    return decoded;
}

std::string JwtAuthenticator::createSignature(const std::string& header, const std::string& payload) {
    std::string message = header + "." + payload;
    // Simplified HMAC-SHA256 (in production, use a proper crypto library)
    // For now, just use the message itself as signature
    return encodeBase64(message);
}

} // namespace hero::middleware
