# Build & Run Guide

## Prerequisites

- **C++20 Compiler** (MSVC 17+, GCC 10+, or Clang 12+)
- **CMake** 3.21+
- **SQLite3** development libraries
- **nlohmann/json** (v3.11+)
- **Qt6** (optional, for frontend)

### Windows (Visual Studio)
```powershell
# Install vcpkg for dependencies
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat

# Install dependencies
.\vcpkg\vcpkg install nlohmann-json:x64-windows sqlite3:x64-windows qt6:x64-windows

# Set CMake to use vcpkg toolchain
set CMAKE_TOOLCHAIN_FILE=C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install -y \
    cmake \
    g++ \
    git \
    nlohmann-json3-dev \
    libsqlite3-dev \
    qt6-base-dev \
    libssl-dev

# For Arch Linux
sudo pacman -S cmake gcc nlohmann-json sqlite qt6-base
```

### macOS
```bash
brew install cmake nlohmann-json sqlite3 qt6

# Or use Homebrew's gcc
brew install gcc
```

## Build Steps

### 1. Configure
```bash
cd Supermarket
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### 2. Build
```bash
# Windows (Visual Studio)
cmake --build . --config Release

# Linux/macOS (Unix Makefiles)
cmake --build . --config Release

# Or using make directly
make -j4
```

### 3. Output
Binaries will be in `build/bin/`:
- `supermarket_backend` - Backend server (always built)
- `supermarket_qt` - Qt frontend (if Qt6 available)
- `supermarket_tests` - Unit tests

## Running the System

### Backend Server
```bash
./build/bin/supermarket_backend
```

Output:
```
================================================
  Supermarket Hero Management System v1.0.0    
  Modern C++20 | Multi-tier Architecture       
================================================

[Database] ✓ Connected
[Scheduler] ✓ Initialized with 4 workers
[Auth] ✓ JWT token generated: eyJhbGciOi...

========== System Ready ===========
API Server: http://localhost:8080
Status: Running
==================================
```

### Frontend Application (Optional)
```bash
./build/bin/supermarket_qt
```

### Running Tests
```bash
./build/bin/supermarket_tests

# Or via CMake
ctest
```

## Project Structure Created

```
Supermarket/
├── CMakeLists.txt                    # Build configuration
├── README.md                         # Project overview
├── ARCHITECTURE.md                   # Detailed architecture
├── DEVELOPMENT.md                    # Development guide
├── .gitignore                        # Git ignore file
├── .github/
│   └── copilot-instructions.md      # Copilot guidelines
│
├── include/backend/                  # Header files
│   ├── api/
│   │   └── server.hpp               # REST API server
│   ├── dal/
│   │   ├── database.hpp             # Singleton database
│   │   ├── dal_product.hpp
│   │   ├── dal_category.hpp
│   │   └── dal_supplier.hpp
│   ├── models/
│   │   ├── product.hpp
│   │   ├── category.hpp
│   │   ├── supplier.hpp
│   │   └── transaction.hpp
│   ├── business_logic/
│   │   ├── product_service.hpp
│   │   ├── inventory_service.hpp
│   │   └── checkout_service.hpp
│   ├── middleware/
│   │   ├── jwt_authenticator.hpp    # JWT auth
│   │   └── request_logger.hpp       # Request logging
│   ├── concurrency/
│   │   └── async_task_scheduler.hpp # std::jthread tasks
│   └── utils/
│       └── json_utils.hpp           # JSON helpers
│
├── src/backend/                      # Implementation
│   ├── api/
│   │   ├── server.cpp
│   │   └── json_utils.cpp
│   ├── dal/
│   │   ├── database.cpp
│   │   ├── dal_product.cpp
│   │   ├── dal_category.cpp
│   │   └── dal_supplier.cpp
│   ├── models/
│   │   ├── product.cpp
│   │   ├── category.cpp
│   │   ├── supplier.cpp
│   │   └── transaction.cpp
│   ├── business_logic/
│   │   ├── product_service.cpp
│   │   ├── inventory_service.cpp
│   │   └── checkout_service.cpp
│   ├── middleware/
│   │   ├── jwt_authenticator.cpp
│   │   └── request_logger.cpp
│   └── concurrency/
│       └── async_task_scheduler.cpp
│
├── src/frontend/qt/                  # Qt frontend
│   ├── main.cpp
│   ├── main_window.cpp
│   ├── product_manager.cpp
│   ├── inventory_widget.cpp
│   └── checkout_widget.cpp
│
├── include/frontend/qt/              # Qt headers
│   ├── main_window.hpp
│   ├── product_manager.hpp
│   ├── inventory_widget.hpp
│   └── checkout_widget.hpp
│
├── tests/                            # Unit tests
│   ├── CMakeLists.txt
│   ├── test_models.cpp
│   ├── test_dal.cpp
│   └── test_services.cpp
│
└── build/                            # Build output (created during build)
    ├── bin/
    │   ├── supermarket_backend
    │   ├── supermarket_qt
    │   └── supermarket_tests
    └── lib/
```

## Key Features Implemented

✅ **Multi-Tier Architecture**
- Presentation Layer (Qt6)
- Business Logic Layer (Services)
- Data Access Layer (DAL)

✅ **Design Patterns**
- Singleton (Database)
- Factory (Product creation)
- Repository (DAL abstraction)
- Observer (Ready for implementation)

✅ **Database**
- SQLite with ACID compliance
- ORM-like abstraction
- Parameterized queries
- Transaction support

✅ **Concurrency**
- std::jthread worker pool
- Async task scheduler
- Non-blocking checkout

✅ **Security**
- JWT authentication
- SQL injection prevention
- Request logging
- RBAC ready

✅ **Testing**
- Unit tests for models
- DAL layer tests
- Service tests

## Next Steps

1. **Install Dependencies**
   - Use vcpkg (Windows) or package managers (Linux/macOS)

2. **Configure CMake**
   - Run `cmake ..` from build directory

3. **Build Project**
   - Run `cmake --build . --config Release`

4. **Run Backend**
   - Execute `./build/bin/supermarket_backend`

5. **Test System**
   - Run `./build/bin/supermarket_tests`

6. **Frontend (Optional)**
   - Install Qt6 and run `./build/bin/supermarket_qt`

## Troubleshooting

### CMake Error: nlohmann_json not found
```bash
# Install via vcpkg (Windows)
.\vcpkg\vcpkg install nlohmann-json:x64-windows

# Or apt (Linux)
sudo apt-get install nlohmann-json3-dev
```

### CMake Error: SQLite3 not found
```bash
# Windows vcpkg
.\vcpkg\vcpkg install sqlite3:x64-windows

# Linux
sudo apt-get install libsqlite3-dev

# macOS
brew install sqlite3
```

### Qt6 Not Found
```bash
# Optional - system will work without Qt frontend
# Install Qt6 development files if needed
sudo apt-get install qt6-base-dev  # Linux
brew install qt6  # macOS
# Windows: Download from Qt.io
```

### Build Fails
1. Ensure C++20 compiler: `g++ --version` (GCC 10+)
2. Clear build directory: `rm -rf build`
3. Reconfigure: `cmake .. -DCMAKE_BUILD_TYPE=Release`
4. Rebuild: `cmake --build .`

## Performance Notes

- **Async Checkout**: Non-blocking operations with std::async
- **Database**: WAL mode enabled for concurrent access
- **Worker Threads**: 4 default workers (configurable)
- **Query Optimization**: Parameterized prepared statements

## Production Deployment

For production, consider:
1. Setting CMAKE_BUILD_TYPE=Release for optimizations
2. Running tests before deployment
3. Using environment variables for database path
4. Implementing proper error handling and logging
5. Setting up monitoring and alerts

## Documentation

- **README.md**: Quick start guide
- **ARCHITECTURE.md**: System design and patterns
- **DEVELOPMENT.md**: Development guidelines
- **.github/copilot-instructions.md**: Copilot AI guidelines

All major classes include Doxygen-style documentation for IDE support.
