# Supermarket Hero Management System - Copilot Instructions

## Project Overview

This is a production-ready, multi-tier Supermarket Management System built with modern C++20, featuring:
- RESTful Backend API with Crow
- SQLite database with ACID compliance
- Qt6 Desktop Frontend
- JWT Authentication
- Asynchronous task processing with std::jthread
- Comprehensive logging and monitoring

## Architecture

### Three-Tier Architecture
1. **Presentation Layer**: Qt6 GUI + REST API Endpoints
2. **Business Logic Layer**: Services with design patterns (Singleton, Factory)
3. **Data Access Layer**: SQLite ORM-like abstraction

## Key Components

### Backend (C++20)
- **Models**: Product, Category, Supplier, Transaction
- **DAL**: Database (Singleton), DalProduct, DalCategory, DalSupplier
- **Services**: ProductService, InventoryService, CheckoutService
- **Middleware**: JwtAuthenticator, RequestLogger
- **Concurrency**: AsyncTaskScheduler with std::jthread
- **API**: RESTful server with Crow framework

### Frontend (Qt6)
- **MainWindow**: Tabbed interface
- **ProductManager**: Product CRUD
- **InventoryWidget**: Real-time statistics
- **CheckoutWidget**: Transaction processing

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Running

```bash
# Backend
./bin/supermarket_backend

# Frontend (if Qt6 available)
./bin/supermarket_qt

# Tests
./bin/supermarket_tests
```

## Code Organization

```
include/backend/
  ├── api/           # REST API endpoints
  ├── dal/           # Data Access Layer
  ├── models/        # Domain models
  ├── business_logic/# Services
  ├── middleware/    # Auth, logging
  └── concurrency/   # Async tasks

src/backend/        # Implementations
src/frontend/qt/    # Qt frontend
tests/              # Unit tests
```

## Development Guidelines

### Adding Features
1. Create domain model in `include/backend/models/`
2. Implement DAL in `include/backend/dal/`
3. Create service in `include/backend/business_logic/`
4. Add API routes in `include/backend/api/`
5. Update frontend if needed

### Naming Conventions
- **Classes**: PascalCase (ProductService)
- **Methods**: camelCase (getProduct)
- **Variables**: snake_case (product_id)
- **Namespaces**: hero::component

### Code Quality
- Use C++20 features (concepts, ranges, etc.)
- Implement RAII patterns
- Use smart pointers for memory safety
- Add Doxygen documentation
- Write unit tests for new features

## Database

### Initialization
Database automatically creates tables on first run.

### Schema
- **products**: ID, name, SKU, price, stock, category_id, supplier_id
- **categories**: ID, name, description
- **suppliers**: ID, name, email, phone, address

### ACID Transactions
All operations use WAL mode and PRAGMA settings for concurrent access.

## API Endpoints

### Products
- POST /api/products - Create
- GET /api/products - List
- GET /api/products/{id} - Get one
- PUT /api/products/{id} - Update
- DELETE /api/products/{id} - Delete

### Inventory
- GET /api/inventory/stats
- GET /api/inventory/low-stock
- GET /api/inventory/out-of-stock

### Checkout
- POST /api/checkout - Process transaction
- GET /api/transactions - List

### Auth
- POST /api/auth/login
- POST /api/auth/logout
- POST /api/auth/refresh

## Design Patterns Used

1. **Singleton**: Database coordinator
2. **Factory**: Product creation
3. **Repository**: DAL abstraction
4. **Observer**: Event-driven (ready for implementation)

## Performance Features

- Async checkout with std::async
- Worker thread pool for background tasks
- Database connection pooling
- Batch operations with transactions
- Indexed queries

## Security

- JWT token authentication
- SQL injection prevention (parameterized queries)
- ACID transactions for data integrity
- Request logging and user tracking
- Role-based access control ready

## Testing

Unit tests cover:
- Model creation and serialization
- Stock management
- Service operations
- Database transactions

## Future Enhancements

- [ ] WebAssembly frontend
- [ ] PostgreSQL support
- [ ] Advanced analytics
- [ ] Multi-location support
- [ ] Barcode scanning
- [ ] Mobile app
- [ ] Cloud deployment

## Common Tasks

### Rebuild
```bash
cd build
cmake --build . --clean-first
cmake --build . --config Release
```

### Reset Database
```bash
rm supermarket.db supermarket.db-wal supermarket.db-shm
# Restart backend to reinitialize
```

### Enable Debug Logging
Set `RequestLogger::setLogLevel(LogLevel::DEBUG)`

### View Logs
Check `requests.log` file

## Troubleshooting

**SQLite Locked Error**: Ensure single process, use WAL mode (default)
**JSON Parse Error**: Validate with `JsonUtils::isValidJson()`
**Qt Not Building**: Install Qt6, verify CMake finds it
**Async Tasks Not Running**: Check `AsyncTaskScheduler::initialize()` called

## Documentation Files

- **README.md**: Project overview and quick start
- **ARCHITECTURE.md**: Detailed architecture and design patterns
- **DEVELOPMENT.md**: Development guide and best practices
- **CMakeLists.txt**: Build configuration

## Key Classes to Know

- `hero::dal::Database` - Singleton for DB connection
- `hero::models::Product` - Core entity
- `hero::business_logic::ProductService` - Business logic
- `hero::api::Server` - REST API server
- `hero::middleware::JwtAuthenticator` - Authentication
- `hero::concurrency::AsyncTaskScheduler` - Task execution

## Support Resources

- Check ARCHITECTURE.md for design patterns
- See DEVELOPMENT.md for setup guide
- Review test files for usage examples
- Comment with Doxygen style /// and /**
