cmake_minimum_required(VERSION 3.21)
project(SupermarketHeroSystem VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /std:c++latest /Zc:__cplusplus")
    add_compile_options(/FS)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()
set(CMAKE_PREFIX_PATH "D:/Qt/6.10.2/mingw_64")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Find required packages
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

# nlohmann_json - provided in the include/ directory
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)

# SQLite3 - provided in third_party/sqlite3
add_library(SQLite3 STATIC third_party/sqlite3/sqlite3.c)
set_target_properties(SQLite3 PROPERTIES LINKER_LANGUAGE C)
target_include_directories(SQLite3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite3)
if(NOT MSVC)
    target_compile_options(SQLite3 PRIVATE -w) # Suppress warnings from 3rd party code
endif()
add_library(SQLite::SQLite3 ALIAS SQLite3)

# ASIO (required by Crow) - header-only
FetchContent_Declare(
    asio
    URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.tar.gz
)
FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
    FetchContent_Populate(asio)
endif()
add_library(asio_lib INTERFACE)
target_include_directories(asio_lib INTERFACE ${asio_SOURCE_DIR}/include)
target_compile_definitions(asio_lib INTERFACE ASIO_STANDALONE)

# Crow - header-only
FetchContent_Declare(
    crow
    URL https://github.com/CrowCpp/Crow/archive/refs/tags/v1.2.0.tar.gz
)
FetchContent_GetProperties(crow)
if(NOT crow_POPULATED)
    FetchContent_Populate(crow)
endif()
add_library(crow_lib INTERFACE)
target_include_directories(crow_lib INTERFACE ${crow_SOURCE_DIR}/include)
target_link_libraries(crow_lib INTERFACE asio_lib)
add_library(Crow::Crow ALIAS crow_lib)
add_library(crow ALIAS crow_lib)
include_directories(${crow_SOURCE_DIR}/include)
include_directories(${asio_SOURCE_DIR}/include)


# Optional Qt6 for frontend
find_package(Qt6 COMPONENTS Core Gui Qml Quick QuickControls2 Network QUIET)

if(Qt6_FOUND)
# Qt policies
if(Qt6_FOUND)
    qt_policy(SET QTP0001 NEW)
    qt_policy(SET QTP0004 NEW)
endif()
endif()

# ============== COMMON LIBRARY SOURCES ==============

set(COMMON_SOURCES
    src/backend/dal/database.cpp
    src/backend/dal/dal_product.cpp
    src/backend/dal/dal_category.cpp
    src/backend/dal/dal_supplier.cpp
    src/backend/dal/dal_transaction.cpp
    src/backend/dal/dal_expense.cpp
    src/backend/models/product.cpp
    src/backend/models/category.cpp
    src/backend/models/supplier.cpp
    src/backend/models/transaction.cpp
    src/backend/business_logic/product_service.cpp
    src/backend/business_logic/inventory_service.cpp
    src/backend/business_logic/checkout_service.cpp
)

# ============== BACKEND EXECUTABLE ==============

set(BACKEND_SOURCES
    ${COMMON_SOURCES}
    include/backend/business_logic/financial_service.hpp
    src/backend/business_logic/financial_service.cpp
    src/backend/hal/dummy_printer.cpp
    src/backend/middleware/jwt_authenticator.cpp
    src/backend/middleware/request_logger.cpp
    src/backend/concurrency/async_task_scheduler.cpp
    src/backend/api/server.cpp
    src/backend/api/json_utils.cpp
    src/main.cpp
)

add_executable(supermarket_backend ${BACKEND_SOURCES})

target_link_libraries(supermarket_backend PRIVATE
    nlohmann_json::nlohmann_json
    SQLite::SQLite3
    Crow::Crow
)

# No Qt for backend
if(WIN32)
    target_link_libraries(supermarket_backend PRIVATE ws2_32 mswsock)
endif()

# ============== QT6 FRONTEND EXECUTABLE ==============

if(Qt6_FOUND)
    set(QT_FRONTEND_SOURCES
        ${COMMON_SOURCES}
        include/backend/business_logic/alert_system.hpp
        src/backend/business_logic/alert_system.cpp
        include/backend/business_logic/financial_service.hpp
        src/backend/business_logic/financial_service.cpp
        src/backend/hal/dummy_printer.cpp
        include/backend/hal/scanner_listener.hpp
        src/backend/hal/scanner_listener.cpp
        include/backend/business_logic/barcode_generator.hpp
        src/frontend/qml/main.cpp
        include/frontend/controllers/checkout_controller.hpp
        src/frontend/controllers/checkout_controller.cpp
        src/frontend/controllers/product_controller.cpp
        src/frontend/models/product_list_model.cpp
    )
    
    add_executable(supermarket_qt ${QT_FRONTEND_SOURCES})
    set_target_properties(supermarket_qt PROPERTIES
        AUTOMOC ON
        AUTORCC ON
    )
    
    qt_add_qml_module(supermarket_qt
        URI Supermarket
        VERSION 1.0
        QML_FILES
            src/frontend/qml/Main.qml
            src/frontend/qml/components/TransactionView.qml
            src/frontend/qml/components/InventoryView.qml
            src/frontend/qml/components/DashboardView.qml
    )
    
    target_link_libraries(supermarket_qt PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Network
        nlohmann_json::nlohmann_json
        SQLite::SQLite3
    )
else()
    message(STATUS "Qt6 not found - Qt frontend will not be built")
endif()

# ============== UNIT TESTS ==============

enable_testing()
add_executable(supermarket_tests
    ${COMMON_SOURCES}
    tests/test_main.cpp
    tests/test_models.cpp
    tests/test_dal.cpp
    tests/test_services.cpp
    tests/test_observer.cpp
)

set_target_properties(supermarket_tests PROPERTIES
    AUTOMOC ON
)

target_link_libraries(supermarket_tests PRIVATE
    nlohmann_json::nlohmann_json
    SQLite::SQLite3
)

if(Qt6_FOUND)
    target_link_libraries(supermarket_tests PRIVATE
        Qt6::Core
        Qt6::Gui
    )
endif()

# No Qt for tests


add_test(NAME ModelTests COMMAND supermarket_tests)

# ============== BUILD SUMMARY ==============

message(STATUS "")
message(STATUS "====== Supermarket Hero System Build Configuration ======")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
message(STATUS "Targets:")
message(STATUS "  - supermarket_backend (RESTful API Server)")
if(Qt6_FOUND)
    message(STATUS "  - supermarket_qt (Desktop Frontend)")
else()
    message(STATUS "  - supermarket_qt (Qt6 not found - skipped)")
endif()
message(STATUS "  - supermarket_tests (Unit Tests)")
message(STATUS "")
message(STATUS "Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "=========================================================")
message(STATUS "")
