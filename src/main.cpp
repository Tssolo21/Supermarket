#include <iostream>
#include <crow.h>
#include <backend/api/server.hpp>
#include <backend/dal/database.hpp>
#include <backend/business_logic/product_service.hpp>
#include <backend/business_logic/inventory_service.hpp>
#include <backend/middleware/jwt_authenticator.hpp>
#include <backend/concurrency/async_task_scheduler.hpp>
#include <backend/business_logic/alert_system.hpp>

int main() {
    try {
        std::cout << "================================================" << std::endl;
        std::cout << "  Supermarket Hero Management System v1.0.0    " << std::endl;
        std::cout << "  Modern C++20 | Multi-tier Architecture       " << std::endl;
        std::cout << "================================================" << std::endl;
        std::cout << std::endl;

        // Initialize Observer Pattern
        auto& notifier = hero::business_logic::InventoryNotifier::getInstance();
        auto alertSystem = std::shared_ptr<hero::business_logic::AlertSystem>(
            &hero::business_logic::AlertSystem::getInstance(), 
            [](hero::business_logic::AlertSystem*){} // No-op deleter for singleton
        );
        notifier.addObserver(alertSystem);

        // Initialize server
        crow::SimpleApp app;
        hero::api::Server server;
        if (!server.initialize(app, 8080)) {
            std::cerr << "Failed to initialize server" << std::endl;
            return 1;
        }

        // Display system information
        std::cout << "[System] Database: SQLite3" << std::endl;
        std::cout << "[System] API Framework: Crow (REST)" << std::endl;
        std::cout << "[System] Authentication: JWT" << std::endl;
        std::cout << "[System] Concurrency: std::jthread" << std::endl;
        std::cout << "[System] Frontend: Qt6" << std::endl;
        std::cout << std::endl;

        // Test database connectivity
        auto& db = hero::dal::Database::getInstance();
        if (db.isConnected()) {
            std::cout << "[Database] ✓ Connected" << std::endl;
        } else {
            std::cout << "[Database] ✗ Connection failed" << std::endl;
            return 1;
        }

        // Test async scheduler
        auto& scheduler = hero::concurrency::AsyncTaskScheduler::getInstance();
        std::cout << "[Scheduler] ✓ Initialized with " << scheduler.getWorkerCount() 
                  << " workers" << std::endl;

        std::cout << std::endl;
        std::cout << "========== System Ready ==========" << std::endl;
        std::cout << "API Server: http://localhost:8080" << std::endl;
        std::cout << "Status: Running" << std::endl;
        std::cout << "=================================" << std::endl;
        std::cout << std::endl;

        // Submit some async tasks
        scheduler.submitTask([]() {
            std::cout << "[Task] Async task executed successfully" << std::endl;
        });

        // Keep server running
        server.run();

        std::cout << "Press Ctrl+C to stop server..." << std::endl;

        // Graceful shutdown
        server.stop();

        std::cout << "[Server] Shutdown complete" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
