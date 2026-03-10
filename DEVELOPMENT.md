# Development Guide

## Quick Start

### 1. Clone and Setup
```bash
cd Supermarket
mkdir build
cd build
cmake ..
```

### 2. Build
```bash
# Windows (Visual Studio)
cmake --build . --config Release

# Linux/Mac (Unix Makefiles)
cmake --build . --config Release
```

### 3. Run Backend
```bash
./bin/supermarket_backend
```

The server will start on `http://localhost:8080`

### 4. Run Frontend (Optional)
```bash
./bin/supermarket_qt
```

## Project Structure Guide

### Adding a New Feature

#### 1. Define the Domain Model
```cpp
// include/backend/models/your_entity.hpp
class YourEntity {
    // Properties
    // Getters/Setters
    // Serialization (toJson/fromJson)
};
```

#### 2. Create DAL
```cpp
// include/backend/dal/dal_your_entity.hpp
class DalYourEntity {
    // CRUD methods
};

// src/backend/dal/dal_your_entity.cpp
// Implementation
```

#### 3. Create Service
```cpp
// include/backend/business_logic/your_service.hpp
class YourService {
    // Business logic methods
};

// src/backend/business_logic/your_service.cpp
// Implementation
```

#### 4. Add API Route
```cpp
// include/backend/api/server.hpp
void Server::setupYourRoutes();

// src/backend/api/server.cpp
void Server::setupYourRoutes() {
    // Define routes
}
```

#### 5. Update Frontend
```cpp
// Add new tab or widget in MainWindow
```

## Code Organization

### Namespaces
```
hero
├── models          // Domain entities
├── dal             // Data Access Layer
├── business_logic  // Services
├── api             // REST API
├── middleware      // Auth, logging
├── concurrency     // Async tasks
└── utils           // Utilities
```

### Naming Conventions

- **Classes**: PascalCase (e.g., `ProductService`)
- **Methods**: camelCase (e.g., `getProduct()`)
- **Variables**: snake_case (e.g., `product_id`)
- **Constants**: UPPER_CASE (e.g., `DEFAULT_TIMEOUT`)
- **Files**: snake_case (e.g., `product_service.cpp`)

## Build Configuration

### CMake Variables
- `CMAKE_CXX_STANDARD=20` - C++20 standard
- `CMAKE_BUILD_TYPE=Release` - Release build
- `Qt6_FOUND` - Qt6 availability check

### Preprocessor Defines
```cpp
#define HERO_DEBUG 1      // Debug mode
#define HERO_VERSION "1.0.0"
```

## Testing

### Run Tests
```bash
cmake --build build --target test
```

### Add New Tests
1. Create test file in `tests/`
2. Update `tests/CMakeLists.txt`
3. Include test headers from implemented code

### Test Structure
```cpp
#include <cassert>
#include <your_header.hpp>

void test_feature() {
    // Arrange
    // Act
    // Assert
    assert(result == expected);
    std::cout << "✓ Test passed" << std::endl;
}
```

## Debugging

### GDB
```bash
gdb ./bin/supermarket_backend
(gdb) run
(gdb) break src/backend/api/server.cpp:50
(gdb) continue
```

### VS Code Debug Configuration
Create `.vscode/launch.json`:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Backend",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/bin/supermarket_backend",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "preLaunchTask": "cmake: build"
        }
    ]
}
```

## Database Management

### Initialize Database
```cpp
auto& db = hero::dal::Database::getInstance();
db.initialize("supermarket.db");
```

### Run Migrations
```cpp
// In Database::initialize()
execute("CREATE TABLE IF NOT EXISTS products (...)");
```

### Reset Database
```cpp
// Delete supermarket.db file and reinitialize
std::remove("supermarket.db");
Database::getInstance().initialize("supermarket.db");
```

## Performance Profiling

### Task Scheduler Stats
```cpp
auto& scheduler = hero::concurrency::AsyncTaskScheduler::getInstance();
std::cout << "Queue size: " << scheduler.getQueueSize() << std::endl;
std::cout << "Completed tasks: " << scheduler.getCompletedTaskCount() << std::endl;
```

### Database Queries
Enable query logging in Database class:
```cpp
// Log execution time
auto start = std::chrono::high_resolution_clock::now();
// Execute query
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
std::cout << "Query time: " << duration.count() << "ms" << std::endl;
```

## Common Tasks

### Add New Product Property
1. Update Product model: `include/backend/models/product.hpp`
2. Update database schema in `DalProduct::createTableIfNotExists()`
3. Update JSON serialization methods
4. Update service methods if needed

### Change Database to PostgreSQL
1. Create new DAL implementation using libpq
2. Keep existing DAL interface for compatibility
3. Update CMakeLists.txt dependencies
4. Update schema SQL statements

### Add WebAssembly Frontend
1. Install Emscripten
2. Create WASM-specific build target in CMakeLists.txt
3. Use nlohmann/json for client-side serialization
4. Build WASM binary and embed in HTML

## Troubleshooting

### SQLite "database is locked"
- Ensure only one process accesses the database
- Use WAL mode (already enabled)
- Check file permissions

### JSON Parsing Errors
- Validate JSON with `JsonUtils::isValidJson()`
- Use try-catch for `json::parse()`
- Check encoding (UTF-8)

### Async Tasks Not Executing
- Verify `AsyncTaskScheduler::initialize()` called
- Check worker count: `getWorkerCount()`
- Monitor queue size: `getQueueSize()`

### Qt Application Won't Start
- Verify Qt6 is installed
- Check Qt6 CMake modules available
- Ensure DISPLAY variable set on Linux

## Contributing

### Code Style
- 4-space indentation
- 100-character line limit
- Comments for complex logic
- Doxygen-style documentation

### Commit Messages
```
[Feature] Add product search functionality
[Fix] Prevent stock over-deduction
[Docs] Update API documentation
[Test] Add unit tests for DAL
```

### Pull Request Process
1. Fork repository
2. Create feature branch
3. Implement changes with tests
4. Update documentation
5. Submit pull request

## Resources

- [C++20 Standard](https://en.cppreference.com/)
- [nlohmann/json Docs](https://github.com/nlohmann/json)
- [SQLite3 API](https://sqlite.org/appfileformat.html)
- [Qt6 Documentation](https://doc.qt.io/qt-6/)
- [CMake Reference](https://cmake.org/cmake/help/latest/)

## Support

For issues and feature requests, create a GitHub issue with:
- System information
- Steps to reproduce
- Expected vs actual behavior
- Logs and error messages
