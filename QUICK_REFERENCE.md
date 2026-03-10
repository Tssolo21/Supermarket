# Quick Reference Guide

## 🚀 Getting Started (5 minutes)

### 1. Build
```bash
cd Supermarket
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### 2. Run Backend
```bash
./bin/supermarket_backend
```

Expected output:
```
[Database] ✓ Connected
[Scheduler] ✓ Initialized with 4 workers
[Auth] ✓ JWT token generated
API Server: http://localhost:8080
```

### 3. Run Tests
```bash
./bin/supermarket_tests
```

---

## 📚 Key Classes & Usage

### Database (Singleton)
```cpp
#include <backend/dal/database.hpp>

auto& db = hero::dal::Database::getInstance();
db.initialize("supermarket.db");

// Transaction support
db.beginTransaction();
// ... operations
db.commit();  // or db.rollback()
```

### Product Service (Factory Pattern)
```cpp
#include <backend/business_logic/product_service.hpp>

hero::business_logic::ProductService service;

// Factory method
auto product = service.createProduct("Laptop", "SKU-001", 999.99, 10, 1, 1);
auto id = service.addProduct(product);

// CRUD
auto p = service.getProduct(id);
service.updateProduct(p);
service.deleteProduct(id);

// Stock management
service.replenishStock(id, 50, supplierId);
service.sellProduct(id, 5);

// Queries
auto products = service.getAllProducts();
auto lowStock = service.getLowStockProducts(10);
```

### Inventory Service
```cpp
#include <backend/business_logic/inventory_service.hpp>

hero::business_logic::InventoryService inventory;

auto stats = inventory.getInventoryStats();
// stats.total_products, low_stock_items, total_inventory_value

auto outOfStock = inventory.getOutOfStockProducts();
bool hasStock = inventory.hasEnoughStock(productId, 5);
```

### Checkout Service (Async Support)
```cpp
#include <backend/business_logic/checkout_service.hpp>

hero::business_logic::CheckoutService checkout;

// Synchronous checkout
auto result = checkout.processCheckout(
    {{productId1, qty1}, {productId2, qty2}},
    "John",
    Transaction::PaymentMethod::CARD
);

// Asynchronous checkout
auto future = checkout.processCheckoutAsync(
    items,
    "John",
    Transaction::PaymentMethod::CARD
);
auto result = future.get();  // Wait for result
```

### JWT Authentication
```cpp
#include <backend/middleware/jwt_authenticator.hpp>

auto& auth = hero::middleware::JwtAuthenticator::getInstance();

// Generate token
auto token = auth.generateToken({
    "user123",
    "john",
    "ADMIN",
    "john@company.com"
}, 3600);  // 1 hour expiry

// Validate token
auto payload = auth.validateToken(token);
if (payload) {
    std::cout << "User: " << payload->username << std::endl;
} else {
    std::cout << "Invalid or expired token" << std::endl;
}
```

### Request Logger
```cpp
#include <backend/middleware/request_logger.hpp>

auto& logger = hero::middleware::RequestLogger::getInstance();
logger.setLogFile("requests.log");
logger.setLogLevel(LogLevel::INFO);

logger.logRequest("GET", "/api/products", "user123");
logger.logResponse("GET", "/api/products", 200, 45);  // 45ms
```

### Async Task Scheduler
```cpp
#include <backend/concurrency/async_task_scheduler.hpp>

auto& scheduler = hero::concurrency::AsyncTaskScheduler::getInstance();
scheduler.initialize(4);  // 4 workers

scheduler.submitTask([]() {
    std::cout << "Task executed in worker thread" << std::endl;
});

std::cout << "Queue size: " << scheduler.getQueueSize() << std::endl;
std::cout << "Completed tasks: " << scheduler.getCompletedTaskCount() << std::endl;
```

### JSON Utilities
```cpp
#include <utils/json_utils.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Create success response
auto response = hero::utils::JsonUtils::createSuccessResponse(
    json{{"name", "Laptop"}},
    "Product created successfully"
);

// Validate JSON
if (hero::utils::JsonUtils::isValidJson(jsonString)) {
    auto obj = hero::utils::JsonUtils::fromJsonString(jsonString);
}

// Pagination
auto paged = hero::utils::JsonUtils::createPaginatedResponse(
    products,
    1,      // page
    10,     // page_size
    100     // total
);
```

---

## 🏗️ Adding New Features

### Add New Entity

#### 1. Create Model
```cpp
// include/backend/models/order.hpp
class Order {
    // Properties
    // Getters/Setters
    json toJson() const;
    static Order fromJson(const json& j);
};
```

#### 2. Create DAL
```cpp
// include/backend/dal/dal_order.hpp
class DalOrder {
    OrderId create(const Order& order);
    std::optional<Order> getById(OrderId id);
    std::vector<Order> getAll();
    bool update(const Order& order);
    bool deleteById(OrderId id);
};
```

#### 3. Create Service
```cpp
// include/backend/business_logic/order_service.hpp
class OrderService {
    OrderId createOrder(const Order& order);
    // ... CRUD and business logic
};
```

#### 4. Add Routes
```cpp
// include/backend/api/server.hpp
void Server::setupOrderRoutes();

// src/backend/api/server.cpp
void Server::setupOrderRoutes() {
    // POST /api/orders
    // GET /api/orders
    // GET /api/orders/{id}
    // PUT /api/orders/{id}
    // DELETE /api/orders/{id}
}
```

---

## 🔍 Database Queries

### Direct Database Operations
```cpp
auto& db = hero::dal::Database::getInstance();

// Execute simple query
db.execute("DELETE FROM products WHERE stock = 0");

// Prepared statement
auto stmt = db.prepareStatement(
    "SELECT * FROM products WHERE id = ?"
);
sqlite3_bind_int64(stmt, 1, 123);

while (sqlite3_step(stmt) == SQLITE_ROW) {
    // Process row
}

db.finalizeStatement(stmt);
```

### Transaction Example
```cpp
if (db.beginTransaction()) {
    DalProduct dalProduct;
    for (const auto& product : products) {
        if (dalProduct.create(product) < 0) {
            db.rollback();
            return false;
        }
    }
    db.commit();
}
```

---

## 📊 API Endpoints (Ready for Implementation)

### Products
```
POST   /api/products               Create product
GET    /api/products               List all products
GET    /api/products/{id}          Get product
PUT    /api/products/{id}          Update product
DELETE /api/products/{id}          Delete product
```

### Inventory
```
GET    /api/inventory/stats         Get statistics
GET    /api/inventory/low-stock     Low stock items
GET    /api/inventory/out-of-stock  Out of stock items
```

### Checkout
```
POST   /api/checkout               Process checkout
GET    /api/transactions           List transactions
GET    /api/transactions/{id}      Get transaction details
```

### Authentication
```
POST   /api/auth/login             User login
POST   /api/auth/logout            User logout
POST   /api/auth/refresh           Refresh token
```

---

## 🐛 Debugging Tips

### Enable Debug Logging
```cpp
hero::middleware::RequestLogger::getInstance()
    .setLogLevel(LogLevel::DEBUG);
```

### Check Task Scheduler Status
```cpp
auto& scheduler = AsyncTaskScheduler::getInstance();
if (!scheduler.isRunning()) {
    scheduler.initialize();
}
```

### Database Error Handling
```cpp
auto& db = Database::getInstance();
if (!db.isConnected()) {
    std::cerr << "DB Error: " << db.getLastError() << std::endl;
}
```

### JSON Validation
```cpp
if (hero::utils::JsonUtils::isValidJson(input)) {
    auto obj = hero::utils::JsonUtils::fromJsonString(input);
} else {
    std::cerr << "Invalid JSON" << std::endl;
}
```

---

## 📝 Common Patterns

### Stock Management
```cpp
ProductService service;
if (service.replenishStock(productId, 100, supplierId)) {
    auto product = service.getProduct(productId);
    std::cout << "New stock: " << product->getStock() << std::endl;
}
```

### Error Handling
```cpp
try {
    service.validateProduct(product);
    auto id = service.addProduct(product);
} catch (const std::invalid_argument& e) {
    std::cerr << "Validation error: " << e.what() << std::endl;
}
```

### Async Operations
```cpp
auto future = checkout.processCheckoutAsync(items, "John", method);
// Do other work...
auto result = future.get();  // Wait for result

if (result.success) {
    std::cout << "Transaction ID: " << result.transaction_id << std::endl;
} else {
    std::cout << "Error: " << result.message << std::endl;
}
```

---

## 📖 Documentation Files

- **README.md** - Project overview and features
- **ARCHITECTURE.md** - System design and patterns
- **DEVELOPMENT.md** - Development guidelines
- **BUILD_AND_RUN.md** - Build instructions
- **IMPLEMENTATION_SUMMARY.md** - What was built
- **CMakeLists.txt** - Build configuration

---

## 🔗 Dependencies

```
nlohmann/json (v3.11+)    - JSON serialization
SQLite3                   - Database
Qt6 (optional)            - Desktop GUI
C++20 compiler            - Language standard
```

---

## 💡 Pro Tips

1. **Always use getInstance()** for Singleton classes (Database, JwtAuthenticator, etc.)
2. **Parameterized queries** are automatically used - never concatenate SQL
3. **ACID transactions** are available - use for bulk operations
4. **Async scheduler** runs automatically - just submit tasks
5. **JSON is first-class** - all models serialize to/from JSON
6. **Logger is optional** - it won't break without configuration

---

## ⚡ Performance Tips

1. Use batch operations for multiple creates/updates
2. Let AsyncTaskScheduler handle I/O operations
3. Close unused database statements with finalizeStatement()
4. WAL mode is enabled by default for concurrent access
5. Connection pooling ready for future implementation

---

**For more details, see the documentation files in the project root.**
