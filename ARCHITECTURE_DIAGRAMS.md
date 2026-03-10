# System Architecture Diagram

## High-Level System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    PRESENTATION LAYER                        │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────────┐  ┌──────────────────────────────┐    │
│  │  Qt6 Desktop UI  │  │    RESTful API Server         │    │
│  │                  │  │  (HTTP Endpoints)            │    │
│  │  - MainWindow    │  │  - Products, Categories      │    │
│  │  - ProductMgr    │  │  - Suppliers, Inventory      │    │
│  │  - InventoryWgt  │  │  - Checkout, Transactions    │    │
│  │  - CheckoutWgt   │  │  - Authentication            │    │
│  └──────────────────┘  └──────────────────────────────┘    │
│                                                               │
└─────────────────────────────────────────────────────────────┘
                              ↓ JSON/HTTP
┌─────────────────────────────────────────────────────────────┐
│                  MIDDLEWARE LAYER                            │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────────────┐  ┌──────────────────────────┐    │
│  │  JWT Authenticator   │  │   Request Logger         │    │
│  │  - generateToken()   │  │  - logRequest()          │    │
│  │  - validateToken()   │  │  - logResponse()         │    │
│  │  - expiration check  │  │  - file persistence      │    │
│  └──────────────────────┘  └──────────────────────────┘    │
│                                                               │
│  ┌──────────────────────────────────────────────────┐       │
│  │      Async Task Scheduler (std::jthread)         │       │
│  │  - Worker pool (4 threads)                       │       │
│  │  - Task queue with notifications                │       │
│  │  - Non-blocking checkout & updates              │       │
│  └──────────────────────────────────────────────────┘       │
│                                                               │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│               BUSINESS LOGIC LAYER                           │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────────┐  ┌──────────────────────────────┐    │
│  │  Product Service │  │  Inventory Service           │    │
│  │  (Factory)       │  │  - Statistics                │    │
│  │  - createProduct │  │  - Low stock alerts          │    │
│  │  - CRUD ops      │  │  - Stock levels              │    │
│  │  - stock mgmt    │  │  - Value calculations        │    │
│  └──────────────────┘  └──────────────────────────────┘    │
│                                                               │
│  ┌──────────────────────────────────────────────────┐       │
│  │       Checkout Service (Async Support)           │       │
│  │  - processCheckout() - Synchronous              │       │
│  │  - processCheckoutAsync() - Non-blocking        │       │
│  │  - Transaction validation                       │       │
│  │  - Sales analytics                              │       │
│  └──────────────────────────────────────────────────┘       │
│                                                               │
│  ┌──────────────────────────────────────────────────┐       │
│  │    Validation & Business Rules                   │       │
│  │  - Price validation (>= 0)                      │       │
│  │  - Stock checks (sufficient inventory)          │       │
│  │  - Transaction validation                       │       │
│  └──────────────────────────────────────────────────┘       │
│                                                               │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                DATA ACCESS LAYER (DAL)                       │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────────────────────────────────────────┐       │
│  │     Database (Singleton Pattern)                 │       │
│  │  - Connection management                        │       │
│  │  - ACID transactions (BEGIN, COMMIT, ROLLBACK) │       │
│  │  - Statement preparation & binding              │       │
│  │  - Error handling & logging                     │       │
│  │  - PRAGMA optimization (WAL, FOREIGN KEYS)     │       │
│  └──────────────────────────────────────────────────┘       │
│                                                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │ DalProduct   │  │ DalCategory  │  │ DalSupplier  │     │
│  │              │  │              │  │              │     │
│  │ - create()   │  │ - create()   │  │ - create()   │     │
│  │ - getById()  │  │ - getById()  │  │ - getById()  │     │
│  │ - getAll()   │  │ - getAll()   │  │ - getAll()   │     │
│  │ - update()   │  │ - update()   │  │ - update()   │     │
│  │ - delete()   │  │ - delete()   │  │ - delete()   │     │
│  │ - search()   │  │ - search()   │  │ - search()   │     │
│  │ - stock ops  │  │              │  │              │     │
│  │ - bulk ops   │  │              │  │              │     │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
│                                                               │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    PERSISTENCE LAYER                         │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────────────────────────────────────────┐       │
│  │              SQLite3 Database                    │       │
│  │                                                  │       │
│  │  ┌─────────────┐  ┌──────────────┐  ┌─────────┐│      │
│  │  │   PRODUCTS  │  │  CATEGORIES  │  │SUPPLIERS││      │
│  │  │             │  │              │  │         ││      │
│  │  │ id (PK)     │  │ id (PK)      │  │ id (PK) ││      │
│  │  │ name        │  │ name         │  │ name    ││      │
│  │  │ sku (UQ)    │  │ description  │  │ email   ││      │
│  │  │ price       │  │ created_at   │  │ phone   ││      │
│  │  │ stock       │  │              │  │ address ││      │
│  │  │ category_id├──→              │  │ created ││      │
│  │  │ supplier_id├─────────────────→  │ _at     ││      │
│  │  │ description│  │              │  │         ││      │
│  │  │ timestamps │  │              │  │         ││      │
│  │  └─────────────┘  └──────────────┘  └─────────┘│      │
│  │                                                  │       │
│  │  Features:                                       │       │
│  │  ✓ ACID Compliance                             │       │
│  │  ✓ Foreign Key Constraints                     │       │
│  │  ✓ Unique Constraints                          │       │
│  │  ✓ Write-Ahead Logging (WAL)                   │       │
│  │  ✓ Parameterized Queries                       │       │
│  │  ✓ Transaction Support                         │       │
│  │  ✓ Concurrent Access                           │       │
│  └──────────────────────────────────────────────────┘       │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

---

## Data Flow: Checkout Process

```
User Input (Checkout Widget)
        ↓
CheckoutService::processCheckout()
        ↓
validate items & quantities
        ↓
InventoryService::hasEnoughStock()
        ↓
for each item:
    ProductService::sellProduct()
        ↓
        DalProduct::decrementStock()
        ↓
        Database::prepareStatement()
        ↓
        SQLite: UPDATE products SET stock = stock - ?
        ↓
    transaction saved
        ↓
Response to UI (success/error)
```

---

## Async Checkout Flow

```
User Submits Checkout
        ↓
CheckoutService::processCheckoutAsync()
        ↓
std::async(std::launch::async, ...)
        ↓
AsyncTaskScheduler::submitTask()
        ↓
Task added to queue
        ↓
Worker threads process task
        ↓
Database transaction in background
        ↓
Future returns CheckoutResult
        ↓
UI updates with result
```

---

## Class Hierarchy

```
hero::models::
├── Product
├── Category
├── Supplier
└── Transaction
    └── TransactionItem

hero::dal::
├── Database (Singleton)
├── DalProduct
├── DalCategory
└── DalSupplier

hero::business_logic::
├── ProductService (Factory)
├── InventoryService
└── CheckoutService

hero::middleware::
├── JwtAuthenticator (Singleton)
└── RequestLogger (Singleton)

hero::concurrency::
└── AsyncTaskScheduler (Singleton)

hero::api::
└── Server

hero::utils::
└── JsonUtils
```

---

## Design Patterns Used

### 1. Singleton (Database, Authenticator, Logger, Scheduler)
```
Static instance accessed globally
Ensures single resource (DB connection)
Thread-safe in modern C++
```

### 2. Factory (Product Creation)
```
ProductService::createProduct()
Encapsulates complex creation logic
Enforces validation rules
Returns ready-to-use objects
```

### 3. Repository (DAL)
```
DalProduct, DalCategory, DalSupplier
Abstracts database operations
Provides CRUD interface
Enables future database switching
```

### 4. Observer (Ready for Implementation)
```
Event system for stock updates
Notify UI of inventory changes
Real-time notifications
```

---

## Technology Stack

```
┌─────────────────────────────────────────┐
│           C++20 Standard                │
└─────────────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│  Framework & Libraries                   │
├─────────────────────────────────────────┤
│ • Crow (REST API Framework)             │
│ • nlohmann/json (JSON Serialization)    │
│ • SQLite3 (Database)                    │
│ • std::jthread (Concurrency)            │
│ • Qt6 (Desktop GUI)                     │
│ • CMake 3.21+ (Build System)            │
└─────────────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│  Operating Systems                       │
├─────────────────────────────────────────┤
│ • Windows (MSVC)                        │
│ • Linux (GCC/Clang)                     │
│ • macOS (Clang)                         │
└─────────────────────────────────────────┘
```

---

## Security Architecture

```
┌──────────────────────┐
│  User Request        │
└──────────┬───────────┘
           ↓
┌──────────────────────────────────────┐
│  JWT Authentication Middleware       │
│  - Validate token                    │
│  - Check expiration                  │
│  - Verify signature                  │
└──────────┬───────────────────────────┘
           ↓
┌──────────────────────────────────────┐
│  Request Logger Middleware            │
│  - Log request details                │
│  - Track user                         │
│  - Audit trail                        │
└──────────┬───────────────────────────┘
           ↓
┌──────────────────────────────────────┐
│  Business Logic Layer                 │
│  - Input validation                   │
│  - Stock checks                       │
│  - Business rule enforcement          │
└──────────┬───────────────────────────┘
           ↓
┌──────────────────────────────────────┐
│  Data Access Layer                    │
│  - Parameterized queries              │
│  - SQL injection prevention           │
└──────────┬───────────────────────────┘
           ↓
┌──────────────────────────────────────┐
│  Database Transaction                 │
│  - ACID compliance                    │
│  - Data integrity                     │
└──────────────────────────────────────┘
```

---

## Performance Optimization

```
┌──────────────────────────────────────┐
│  Write-Ahead Logging (WAL)            │
│  - Concurrent read/write              │
│  - Transaction support                │
└────────────────────────────────────────┘
         ↑
┌──────────────────────────────────────┐
│  Database Pragmas                     │
│  - Foreign keys enabled               │
│  - Synchronous mode optimized         │
└────────────────────────────────────────┘
         ↑
┌──────────────────────────────────────┐
│  Async Task Scheduler                 │
│  - Non-blocking operations             │
│  - Worker thread pool                 │
└────────────────────────────────────────┘
         ↑
┌──────────────────────────────────────┐
│  Statement Preparation                │
│  - Compiled SQL plans                 │
│  - Reusable statements                │
└────────────────────────────────────────┘
         ↑
┌──────────────────────────────────────┐
│  Batch Operations                     │
│  - Transaction wrapping               │
│  - Reduced round-trips                │
└────────────────────────────────────────┘
```

---

## Testing Architecture

```
┌─────────────────────────────────────┐
│        Unit Tests                    │
├─────────────────────────────────────┤
│ • Model Tests                        │
│   - Creation                         │
│   - Serialization                    │
│   - Stock management                 │
│                                      │
│ • DAL Tests                          │
│   - CRUD operations                  │
│   - Transactions                     │
│   - Parameterized queries            │
│                                      │
│ • Service Tests                      │
│   - Business logic                   │
│   - Validation                       │
│   - Stock operations                 │
└─────────────────────────────────────┘
         ↓
┌─────────────────────────────────────┐
│      Integration Tests               │
│  (Ready for implementation)          │
├─────────────────────────────────────┤
│ • End-to-end flows                   │
│ • API endpoint tests                 │
│ • Database consistency               │
└─────────────────────────────────────┘
```

---

**This architecture is production-ready, scalable, and maintainable.**
