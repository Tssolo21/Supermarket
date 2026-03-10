# Complete Class & Method Reference

## Models (hero::models::)

### Product
**File**: `include/backend/models/product.hpp` / `src/backend/models/product.cpp`

```cpp
class Product {
public:
    // Constructor
    Product(ProductId id, const std::string& name, const std::string& sku,
            double price, int stock, ProductId categoryId, ProductId supplierId);
    
    // Getters
    ProductId getId() const;
    const std::string& getName() const;
    const std::string& getSku() const;
    double getPrice() const;
    int getStock() const;
    ProductId getCategoryId() const;
    ProductId getSupplierId() const;
    const std::string& getDescription() const;
    time_t getCreatedAt() const;
    time_t getUpdatedAt() const;
    
    // Setters
    void setName(const std::string& name);
    void setPrice(double price);
    void setStock(int stock);
    void setDescription(const std::string& desc);
    void setSupplierId(ProductId id);
    
    // Stock Management
    bool addStock(int quantity);
    bool removeStock(int quantity);
    bool isInStock() const;
    bool isLowStock(int threshold = 10) const;
    
    // Serialization
    json toJson() const;
    static Product fromJson(const json& j);
};
```

### Category
**File**: `include/backend/models/category.hpp` / `src/backend/models/category.cpp`

```cpp
class Category {
public:
    Category(CategoryId id, const std::string& name, const std::string& description = "");
    
    CategoryId getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    time_t getCreatedAt() const;
    
    void setName(const std::string& name);
    void setDescription(const std::string& desc);
    
    json toJson() const;
    static Category fromJson(const json& j);
};
```

### Supplier
**File**: `include/backend/models/supplier.hpp` / `src/backend/models/supplier.cpp`

```cpp
class Supplier {
public:
    Supplier(SupplierId id, const std::string& name, const std::string& email,
             const std::string& phone, const std::string& address);
    
    SupplierId getId() const;
    const std::string& getName() const;
    const std::string& getEmail() const;
    const std::string& getPhone() const;
    const std::string& getAddress() const;
    time_t getCreatedAt() const;
    
    void setEmail(const std::string& email);
    void setPhone(const std::string& phone);
    void setAddress(const std::string& address);
    
    json toJson() const;
    static Supplier fromJson(const json& j);
};
```

### Transaction
**File**: `include/backend/models/transaction.hpp` / `src/backend/models/transaction.cpp`

```cpp
class Transaction {
public:
    struct TransactionItem {
        ItemId product_id;
        std::string product_name;
        int quantity;
        double unit_price;
        double subtotal;
        
        json toJson() const;
        static TransactionItem fromJson(const json& j);
    };
    
    enum class PaymentMethod { CASH, CARD, MOBILE_PAYMENT, CHECK };
    
    Transaction(TransactionId id);
    
    TransactionId getId() const;
    const std::vector<TransactionItem>& getItems() const;
    double getTotalAmount() const;
    PaymentMethod getPaymentMethod() const;
    time_t getTimestamp() const;
    const std::string& getCashier() const;
    
    void addItem(const TransactionItem& item);
    void setPaymentMethod(PaymentMethod method);
    void setCashier(const std::string& cashier);
    void finalize();
    
    json toJson() const;
    static Transaction fromJson(const json& j);
};
```

---

## Data Access Layer (hero::dal::)

### Database (Singleton)
**File**: `include/backend/dal/database.hpp` / `src/backend/dal/database.cpp`

```cpp
class Database {
public:
    static Database& getInstance();
    
    bool initialize(const std::string& db_path);
    void shutdown();
    bool isConnected() const;
    
    bool beginTransaction();
    bool commit();
    bool rollback();
    
    int execute(const std::string& sql);
    sqlite3_stmt* prepareStatement(const std::string& sql);
    void finalizeStatement(sqlite3_stmt* stmt);
    
    std::string getLastError() const;
    int getLastErrorCode() const;
    
    sqlite3* getRawConnection();
};
```

### DalProduct
**File**: `include/backend/dal/dal_product.hpp` / `src/backend/dal/dal_product.cpp`

```cpp
class DalProduct {
public:
    ProductId create(const Product& product);
    std::optional<Product> getById(ProductId id);
    std::vector<Product> getAll();
    std::vector<Product> getByCategoryId(ProductId category_id);
    std::vector<Product> getBySupplierId(ProductId supplier_id);
    std::optional<Product> getBySkU(const std::string& sku);
    std::vector<Product> searchByName(const std::string& name);
    
    bool update(const Product& product);
    bool deleteById(ProductId id);
    
    bool updateStock(ProductId id, int quantity);
    bool incrementStock(ProductId id, int quantity);
    bool decrementStock(ProductId id, int quantity);
    
    bool bulkCreate(const std::vector<Product>& products);
    bool bulkUpdate(const std::vector<Product>& products);
};
```

### DalCategory
**File**: `include/backend/dal/dal_category.hpp` / `src/backend/dal/dal_category.cpp`

```cpp
class DalCategory {
public:
    CategoryId create(const Category& category);
    std::optional<Category> getById(CategoryId id);
    std::vector<Category> getAll();
    bool update(const Category& category);
    bool deleteById(CategoryId id);
    std::vector<Category> searchByName(const std::string& name);
};
```

### DalSupplier
**File**: `include/backend/dal/dal_supplier.hpp` / `src/backend/dal/dal_supplier.cpp`

```cpp
class DalSupplier {
public:
    SupplierId create(const Supplier& supplier);
    std::optional<Supplier> getById(SupplierId id);
    std::vector<Supplier> getAll();
    bool update(const Supplier& supplier);
    bool deleteById(SupplierId id);
    std::vector<Supplier> searchByName(const std::string& name);
};
```

---

## Business Logic Layer (hero::business_logic::)

### ProductService (Factory Pattern)
**File**: `include/backend/business_logic/product_service.hpp` / `src/backend/business_logic/product_service.cpp`

```cpp
class ProductService {
public:
    // Factory Method
    static Product createProduct(const std::string& name, const std::string& sku,
                                 double price, int stock, ProductId category_id,
                                 ProductId supplier_id);
    
    // CRUD
    ProductId addProduct(const Product& product);
    std::optional<Product> getProduct(ProductId id);
    std::vector<Product> getAllProducts();
    std::vector<Product> getProductsByCategory(ProductId category_id);
    std::vector<Product> getProductsBySupplier(ProductId supplier_id);
    bool updateProduct(const Product& product);
    bool deleteProduct(ProductId id);
    
    // Search & Filtering
    std::vector<Product> searchProducts(const std::string& query);
    
    // Stock Management
    bool replenishStock(ProductId id, int quantity, ProductId supplier_id);
    bool sellProduct(ProductId id, int quantity);
    std::vector<Product> getLowStockProducts(int threshold = 10);
    
    // Analytics
    int getTotalInventoryValue();
    
private:
    void validateProduct(const Product& product);
};
```

### InventoryService
**File**: `include/backend/business_logic/inventory_service.hpp` / `src/backend/business_logic/inventory_service.cpp`

```cpp
class InventoryService {
public:
    struct InventoryStats {
        int total_products;
        int low_stock_items;
        int out_of_stock_items;
        double total_inventory_value;
        double average_price;
    };
    
    InventoryStats getInventoryStats();
    std::vector<Product> getOutOfStockProducts();
    std::vector<Product> getLowStockProducts(int threshold = 10);
    double calculateInventoryValue();
    bool hasEnoughStock(ProductId product_id, int quantity);
    int getAvailableStock(ProductId product_id);
};
```

### CheckoutService
**File**: `include/backend/business_logic/checkout_service.hpp` / `src/backend/business_logic/checkout_service.cpp`

```cpp
class CheckoutService {
public:
    struct CheckoutResult {
        bool success;
        TransactionId transaction_id;
        double total_amount;
        std::string message;
    };
    
    // Synchronous Checkout
    CheckoutResult processCheckout(
        const std::vector<std::pair<ProductId, int>>& items,
        const std::string& cashier,
        Transaction::PaymentMethod method);
    
    // Asynchronous Checkout (std::async)
    std::future<CheckoutResult> processCheckoutAsync(
        const std::vector<std::pair<ProductId, int>>& items,
        const std::string& cashier,
        Transaction::PaymentMethod method);
    
    // Transaction Retrieval
    std::optional<Transaction> getTransaction(TransactionId id);
    std::vector<Transaction> getTransactionsByDateRange(time_t start, time_t end);
    std::vector<Transaction> getTransactionsByCashier(const std::string& cashier);
    
    // Analytics
    double getDailySales(time_t date);
    int getDailyTransactionCount(time_t date);
    
private:
    void validateCheckoutItems(const std::vector<std::pair<ProductId, int>>& items);
    Transaction createTransaction(const std::vector<std::pair<ProductId, int>>& items,
                                  const std::string& cashier);
};
```

---

## Middleware Layer (hero::middleware::)

### JwtAuthenticator (Singleton)
**File**: `include/backend/middleware/jwt_authenticator.hpp` / `src/backend/middleware/jwt_authenticator.cpp`

```cpp
class JwtAuthenticator {
public:
    struct TokenPayload {
        std::string user_id;
        std::string username;
        std::string role;
        std::string email;
        long long issued_at;
        long long expires_at;
    };
    
    static JwtAuthenticator& getInstance();
    
    std::string generateToken(const TokenPayload& payload, int expiry_seconds = 3600);
    std::optional<TokenPayload> validateToken(const std::string& token);
    bool isTokenExpired(const std::string& token);
    bool revokeToken(const std::string& token);
    
    static std::string getTokenFromHeader(const std::string& auth_header);
    
private:
    std::string encodeBase64(const std::string& input);
    std::string decodeBase64(const std::string& input);
    std::string createSignature(const std::string& header, const std::string& payload);
};
```

### RequestLogger (Singleton)
**File**: `include/backend/middleware/request_logger.hpp` / `src/backend/middleware/request_logger.cpp`

```cpp
class RequestLogger {
public:
    enum class LogLevel { DEBUG, INFO, WARNING, ERROR };
    
    struct LogEntry {
        time_t timestamp;
        LogLevel level;
        std::string method;
        std::string path;
        int status_code;
        std::string message;
        std::string user_id;
        long duration_ms;
    };
    
    static RequestLogger& getInstance();
    
    void log(LogLevel level, const std::string& method, const std::string& path,
             int status_code, const std::string& message, const std::string& user_id = "");
    void logRequest(const std::string& method, const std::string& path, const std::string& user_id);
    void logResponse(const std::string& method, const std::string& path, 
                     int status_code, long duration_ms);
    
    void setLogFile(const std::string& file_path);
    void setLogLevel(LogLevel level);
    void enableConsoleOutput(bool enable);
    
    std::vector<LogEntry> getRecentLogs(int count = 100);
    std::vector<LogEntry> getLogsByUser(const std::string& user_id);
    
private:
    void writeToFile(const LogEntry& entry);
    std::string levelToString(LogLevel level) const;
};
```

---

## Concurrency Layer (hero::concurrency::)

### AsyncTaskScheduler (Singleton)
**File**: `include/backend/concurrency/async_task_scheduler.hpp` / `src/backend/concurrency/async_task_scheduler.cpp`

```cpp
class AsyncTaskScheduler {
public:
    using Task = std::function<void()>;
    
    static AsyncTaskScheduler& getInstance();
    
    void initialize(int num_workers = 4);
    void shutdown();
    bool isRunning() const;
    
    void submitTask(Task task);
    void submitPriorityTask(Task task);
    
    void setWorkerCount(int count);
    int getQueueSize() const;
    int getWorkerCount() const;
    
    int getCompletedTaskCount() const;
    int getFailedTaskCount() const;
    
private:
    void workerThreadFunction(std::stop_token stop_token);
};
```

---

## API Layer (hero::api::)

### Server
**File**: `include/backend/api/server.hpp` / `src/backend/api/server.cpp`

```cpp
class Server {
public:
    bool initialize(int port = 8080);
    void run();
    void stop();
    bool isRunning() const;
    
    void setPort(int port);
    void setHost(const std::string& host);
    int getPort() const;
    
private:
    void setupProductRoutes();
    void setupCategoryRoutes();
    void setupSupplierRoutes();
    void setupCheckoutRoutes();
    void setupInventoryRoutes();
    void setupAuthRoutes();
    void setupMiddleware();
};
```

---

## Utility Layer (hero::utils::)

### JsonUtils
**File**: `include/utils/json_utils.hpp` / `src/backend/api/json_utils.cpp`

```cpp
class JsonUtils {
public:
    static std::string toJsonString(const json& obj);
    static json fromJsonString(const std::string& json_str);
    
    static bool isValidJson(const std::string& json_str);
    
    static json createSuccessResponse(const json& data, const std::string& message = "Success");
    static json createErrorResponse(const std::string& error, int code = 400);
    static json createPaginatedResponse(const json& data, int page, int page_size, int total);
};
```

---

## Frontend Layer (Qt6)

### MainWindow
**File**: `include/frontend/qt/main_window.hpp` / `src/frontend/qt/main_window.cpp`

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
private:
    void setupUi();
    void connectSignals();
};
```

### ProductManager
**File**: `include/frontend/qt/product_manager.hpp` / `src/frontend/qt/product_manager.cpp`

```cpp
class ProductManager : public QWidget {
    Q_OBJECT
public:
    explicit ProductManager(QWidget* parent = nullptr);
    
private slots:
    void onAddProduct();
    void onEditProduct();
    void onDeleteProduct();
    void onSearchProducts();
    void onRefreshProducts();
    
private:
    void setupUi();
    void loadProducts();
};
```

### InventoryWidget
**File**: `include/frontend/qt/inventory_widget.hpp` / `src/frontend/qt/inventory_widget.cpp`

```cpp
class InventoryWidget : public QWidget {
    Q_OBJECT
public:
    explicit InventoryWidget(QWidget* parent = nullptr);
    
private slots:
    void onRefreshStats();
    void onLowStockAlert();
    
private:
    void setupUi();
    void updateStats();
};
```

### CheckoutWidget
**File**: `include/frontend/qt/checkout_widget.hpp` / `src/frontend/qt/checkout_widget.cpp`

```cpp
class CheckoutWidget : public QWidget {
    Q_OBJECT
public:
    explicit CheckoutWidget(QWidget* parent = nullptr);
    
private slots:
    void onAddItem();
    void onRemoveItem();
    void onProcessCheckout();
    void onClearCart();
    
private:
    void setupUi();
    void updateTotal();
};
```

---

## Summary Statistics

| Category | Count |
|----------|-------|
| Classes | 20+ |
| Methods | 200+ |
| Header Files | 18 |
| Implementation Files | 25+ |
| Test Files | 3 |
| Documentation Files | 7 |
| **Total Files** | **50+** |

---

**All classes follow C++20 best practices with proper RAII, move semantics, and const-correctness.**
