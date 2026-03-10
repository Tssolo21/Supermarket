# Architecture Documentation

## System Overview

The Supermarket Hero Management System is a production-ready, multi-tier application built with C++20, featuring a RESTful backend API, SQLite persistence layer, and Qt6 desktop frontend.

## Architecture Layers

### 1. Presentation Layer
**Location**: `include/frontend/qt/` and `src/frontend/qt/`

- **MainWindow**: Central application window with tabbed interface
- **ProductManager**: Product CRUD interface
- **InventoryWidget**: Real-time inventory statistics and alerts
- **CheckoutWidget**: Fast checkout and transaction processing

**Technology**: Qt6 (Core, GUI, Widgets, Network)

### 2. Business Logic Layer
**Location**: `include/backend/business_logic/` and `src/backend/business_logic/`

#### Services:
1. **ProductService**
   - Factory Pattern: Creates products with validation
   - CRUD operations for products
   - Stock management (replenish, sell)
   - Product search and filtering

2. **InventoryService**
   - Inventory statistics (total items, low stock, out-of-stock)
   - Stock level queries
   - Inventory value calculation
   - Low stock alerts

3. **CheckoutService**
   - Synchronous checkout processing
   - Asynchronous checkout (`std::async`) for non-blocking operations
   - Transaction creation and validation
   - Daily sales analytics

### 3. Data Access Layer (DAL)
**Location**: `include/backend/dal/` and `src/backend/dal/`

#### Components:
1. **Database** (Singleton Pattern)
   ```cpp
   Database& db = Database::getInstance();
   db.initialize("supermarket.db");
   db.beginTransaction();
   // Operations
   db.commit();
   ```
   - Connection management
   - ACID transaction support
   - Error handling
   - SQLite pragmas for performance

2. **DalProduct** - Product persistence
3. **DalCategory** - Category persistence
4. **DalSupplier** - Supplier persistence

#### ORM Features:
- Parameterized queries (SQL injection prevention)
- Statement preparation and finalization
- Bulk operations with transactions
- Type-safe bindings

### 4. Domain Models
**Location**: `include/backend/models/` and `src/backend/models/`

#### Core Models:
1. **Product**
   - Attributes: ID, name, SKU, price, stock, category, supplier
   - Methods: stock management, JSON serialization
   - Validation: price >= 0, stock >= 0

2. **Category**
   - Attributes: ID, name, description

3. **Supplier**
   - Attributes: ID, name, email, phone, address

4. **Transaction**
   - Nested struct: TransactionItem
   - Payment method enum
   - Total calculation with tax

#### Serialization:
All models implement JSON serialization using `nlohmann/json`

### 5. Middleware Layer
**Location**: `include/backend/middleware/` and `src/backend/middleware/`

#### JWT Authentication
```cpp
auto token = JwtAuthenticator::getInstance()
    .generateToken(payload, 3600);

auto payload = JwtAuthenticator::getInstance()
    .validateToken(token);
```
- Token generation and validation
- Expiration checks
- Base64 encoding/decoding
- Signature verification

#### Request Logger
- Structured logging (DEBUG, INFO, WARNING, ERROR)
- File-based persistence
- Console output
- Request/response tracking
- User-based filtering

### 6. Concurrency Layer
**Location**: `include/backend/concurrency/` and `src/backend/concurrency/`

#### AsyncTaskScheduler
```cpp
auto& scheduler = AsyncTaskScheduler::getInstance();
scheduler.initialize(4); // 4 worker threads

scheduler.submitTask([]() {
    // Non-blocking task
});
```
- Worker pool with `std::jthread`
- Task queue with mutex synchronization
- Task statistics
- Dynamic worker management

### 7. API Layer
**Location**: `include/backend/api/` and `src/backend/api/`

#### RESTful Endpoints:
- Products: CRUD operations
- Categories: Category management
- Suppliers: Supplier management
- Inventory: Real-time statistics
- Checkout: Transaction processing
- Authentication: Login, logout, token refresh

## Design Patterns

### 1. Singleton Pattern
**Database Coordinator**
```cpp
class Database {
private:
    Database() = default;
    static Database& getInstance();
    // ...
};
```
Ensures single database connection instance.

### 2. Factory Pattern
**Product Creation**
```cpp
Product ProductService::createProduct(
    const std::string& name,
    const std::string& sku,
    double price,
    int stock,
    ProductId category_id,
    ProductId supplier_id);
```
Encapsulates product creation logic and validation.

### 3. Repository Pattern
**Data Access Layer**
- Abstract database operations
- Consistent CRUD interface across entities
- Query abstraction

### 4. Observer Pattern (Ready for Implementation)
- Stock update notifications
- Inventory alerts

## Data Flow

### Checkout Process
```
User Input (Checkout Widget)
    ↓
CheckoutService::processCheckout()
    ↓
InventoryService::hasEnoughStock()
    ↓
ProductService::sellProduct()
    ↓
DalProduct::decrementStock()
    ↓
Database::execute() [Transaction]
    ↓
Transaction Logged
    ↓
Response to UI
```

### Async Checkout
```
User Submits Checkout
    ↓
CheckoutService::processCheckoutAsync()
    ↓
std::async(processCheckout)
    ↓
AsyncTaskScheduler::submitTask()
    ↓
Worker Thread Executes
    ↓
Database Transaction
    ↓
Future Returns Result
```

## Database Schema

### Products Table
```sql
CREATE TABLE products (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    sku TEXT NOT NULL UNIQUE,
    price REAL NOT NULL,
    stock INTEGER NOT NULL DEFAULT 0,
    category_id INTEGER,
    supplier_id INTEGER,
    description TEXT,
    created_at INTEGER,
    updated_at INTEGER,
    FOREIGN KEY(category_id) REFERENCES categories(id),
    FOREIGN KEY(supplier_id) REFERENCES suppliers(id)
);
```

### Categories Table
```sql
CREATE TABLE categories (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    description TEXT,
    created_at INTEGER
);
```

### Suppliers Table
```sql
CREATE TABLE suppliers (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    email TEXT,
    phone TEXT,
    address TEXT,
    created_at INTEGER
);
```

## Security Considerations

1. **Input Validation**
   - ProductService validates all product data
   - Parameterized queries prevent SQL injection

2. **Authentication**
   - JWT tokens with expiration
   - Role-based access control ready

3. **Data Integrity**
   - ACID transactions
   - Foreign key constraints
   - Stock validation before deduction

4. **Logging**
   - All API requests logged
   - User tracking
   - Error tracking

## Performance Optimizations

1. **Database**
   - WAL (Write-Ahead Logging) for concurrent access
   - PRAGMA optimizations
   - Indexed queries

2. **Concurrency**
   - Non-blocking async checkout
   - Worker pool for background tasks
   - Real-time stock updates

3. **Batch Operations**
   - Bulk product create/update
   - Transaction batching

## Extension Points

1. **Authentication**: Extend JwtAuthenticator for OAuth2
2. **Database**: Switch to PostgreSQL via DAL abstraction
3. **Frontend**: Add WebAssembly version
4. **API Framework**: Replace placeholder with Crow
5. **Reporting**: Add advanced analytics module

## Testing Strategy

1. **Unit Tests**: Model and service tests
2. **Integration Tests**: DAL with database
3. **API Tests**: HTTP endpoint validation
4. **Performance Tests**: Load testing

## Deployment

### Backend
```bash
./bin/supermarket_backend --port 8080
```

### Frontend
```bash
./bin/supermarket_qt
```

### Docker Support (Future)
```dockerfile
FROM cpp:20
WORKDIR /app
COPY . .
RUN cmake . && make
CMD ["./bin/supermarket_backend"]
```

## Monitoring

- Request logs in `requests.log`
- Database error tracking
- Task scheduler statistics
- Real-time inventory alerts

## Future Enhancements

1. WebAssembly frontend for browser access
2. PostgreSQL migration
3. Advanced reporting and BI
4. Multi-location support
5. Barcode integration
6. Mobile application
7. Cloud deployment (Kubernetes)
8. Real-time WebSocket updates
