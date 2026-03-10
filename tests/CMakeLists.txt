cmake_minimum_required(VERSION 3.21)

# Unit tests using built-in testing framework
add_executable(supermarket_tests
    test_models.cpp
    test_dal.cpp
    test_services.cpp
)

target_link_libraries(supermarket_tests
    nlohmann_json::nlohmann_json
    SQLite::SQLite3
)

# Register tests
add_test(NAME ModelTests COMMAND supermarket_tests)
