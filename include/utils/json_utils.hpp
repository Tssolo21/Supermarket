#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace hero::utils {

/**
 * @class JsonUtils
 * @brief JSON serialization/deserialization utilities
 */
class JsonUtils {
public:
    // Serialization
    static std::string toJsonString(const json& obj);
    static json fromJsonString(const std::string& json_str);

    // Validation
    static bool isValidJson(const std::string& json_str);

    // Helpers
    static json createSuccessResponse(const json& data, const std::string& message = "Success");
    static json createErrorResponse(const std::string& error, int code = 400);
    static json createPaginatedResponse(const json& data, int page, int page_size, int total);

private:
    JsonUtils() = default;
};

} // namespace hero::utils
