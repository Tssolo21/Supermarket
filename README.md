# Supermarket Hero Management System

A high-performance, production-ready supermarket management system built with modern C++20.

## Architecture

### Multi-Tier Architecture
- **Presentation Layer**: Qt6 Desktop UI + REST API
- **Business Logic Layer**: Services with design patterns (Singleton, Factory)
- **Data Access Layer**: ORM-like SQLite database with ACID compliance

### Key Features

#### Backend
- **RESTful API** using Crow framework
- **Database**: SQLite with ACID transactions
- **Authentication**: JWT-based security
- **Logging**: Comprehensive request/response logging
- **Concurrency**: `std::jthread` for async operations

#### Frontend
- **Qt6**: Desktop application with responsive UI
- **Real-time Updates**: Live inventory tracking
- **Checkout System**: Fast transaction processing

#### Design Patterns
- **Singleton**: Database coordinator for connection management
- **Factory**: Product creation with validation
- **Observer**: Event-driven architecture for stock updates
- **Repository**: Data Access Layer abstraction

## Project Structure

```
Supermarket/
├── CMakeLists.txt              # Build configuration
├── include/                     # Header files
│   └── backend/
│       ├── api/                # REST API
│       ├── dal/                # Data Access Layer
│       ├── models/             # Domain models
│       ├── business_logic/      # Services
│       ├── middleware/          # Auth, logging
│       └── concurrency/         # Async tasks
├── src/                         # Implementation
├── tests/                       # Unit tests
├── third_party/                 # External dependencies
└── docs/                        # Documentation
```

## Dependencies

- **C++20 Standard Library**
- **nlohmann/json**: JSON serialization
- **SQLite3**: Database
- **Crow**: HTTP framework (optional)
- **Qt6**: GUI framework (optional)

## Building

### Prerequisites
- CMake 3.21+
- C++20 compiler (MSVC, GCC, Clang)
- SQLite3 development files
- nlohmann/json

### Build Steps

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## API Endpoints

### Products
- `POST /api/products` - Create product
- `GET /api/products` - List all products
- `GET /api/products/{id}` - Get product details
- `PUT /api/products/{id}` - Update product
- `DELETE /api/products/{id}` - Delete product
- `POST /api/products/{id}/stock` - Update stock

### Inventory
- `GET /api/inventory/stats` - Inventory statistics
- `GET /api/inventory/low-stock` - Low stock items
- `GET /api/inventory/out-of-stock` - Out of stock items

### Checkout
- `POST /api/checkout` - Process transaction
- `GET /api/transactions` - List transactions
- `GET /api/transactions/{id}` - Transaction details

### Authentication
- `POST /api/auth/login` - User login
- `POST /api/auth/logout` - User logout
- `POST /api/auth/refresh` - Refresh JWT token

## Running

```bash
# Start backend server
./bin/supermarket_backend

# Start Qt6 frontend (if compiled)
./bin/supermarket_qt
```

## Testing

```bash
cmake --build build --target test
```

## Performance Features

1. **Async Checkout**: Non-blocking transaction processing
2. **Connection Pooling**: Efficient database resource management
3. **Real-time Updates**: Live inventory synchronization
4. **Batch Operations**: Bulk product/stock updates
5. **Indexed Queries**: Fast product search and filtering

## Security

- JWT Authentication for API endpoints
- SQL Injection prevention via parameterized queries
- ACID transactions for data integrity
- Role-based access control (RBAC) ready

## Future Enhancements

- [ ] WebAssembly frontend
- [ ] PostgreSQL support
- [ ] Advanced reporting and analytics
- [ ] Multi-location support
- [ ] Barcode scanning integration
- [ ] Mobile app (React Native)
- [ ] Cloud deployment (Docker, Kubernetes)

## License

MIT License

## Author

Hero Development Team - 2026
