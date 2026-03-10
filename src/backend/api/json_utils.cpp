#include <utils/json_utils.hpp>

namespace hero::utils {

std::string JsonUtils::toJsonString(const json& obj) {
    return obj.dump();
}

json JsonUtils::fromJsonString(const std::string& json_str) {
    try {
        return json::parse(json_str);
    } catch (const std::exception& e) {
        return json::object();
    }
}

bool JsonUtils::isValidJson(const std::string& json_str) {
    try {
        json::parse(json_str);
        return true;
    } catch (...) {
        return false;
    }
}

json JsonUtils::createSuccessResponse(const json& data, const std::string& message) {
    json response;
    response["status"] = "success";
    response["message"] = message;
    response["data"] = data;
    response["timestamp"] = std::time(nullptr);
    return response;
}

json JsonUtils::createErrorResponse(const std::string& error, int code) {
    json response;
    response["status"] = "error";
    response["message"] = error;
    response["code"] = code;
    response["timestamp"] = std::time(nullptr);
    return response;
}

json JsonUtils::createPaginatedResponse(const json& data, int page, int page_size, int total) {
    json response;
    response["status"] = "success";
    response["data"] = data;
    response["pagination"]["page"] = page;
    response["pagination"]["page_size"] = page_size;
    response["pagination"]["total"] = total;
    response["pagination"]["total_pages"] = (total + page_size - 1) / page_size;
    return response;
}

} // namespace hero::utils
