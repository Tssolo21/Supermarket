#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QDir>
#include <QDebug>
#include "backend/dal/database.hpp"
#include "frontend/controllers/checkout_controller.hpp"
#include "backend/business_logic/financial_service.hpp"
#include "backend/business_logic/alert_system.hpp"
#include "backend/dal/dal_product.hpp"
#include "backend/business_logic/product_service.hpp"
#include "frontend/controllers/product_controller.hpp"
#include "frontend/models/product_list_model.hpp"

using namespace hero::dal;

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Material");
    app.setApplicationName("Supermarket Hero System");
    app.setOrganizationName("Hermes Inc.");

    // Initialize Database
    try {
        Database& db = Database::getInstance();
        QDir dir(app.applicationDirPath());
        QString dbPath = dir.absoluteFilePath("supermarket.db");
        qDebug() << "Initializing database at:" << dbPath;
        db.initialize(dbPath.toStdString());
    } catch (const std::exception& e) {
        qCritical() << "Failed to initialize database:" << e.what();
        return -1;
    }

    QQmlApplicationEngine engine;
    
    // Register Models
    qmlRegisterType<hero::frontend::models::ProductListModel>("Supermarket", 1, 0, "ProductListModel");

    // Instantiate Dependencies (Dependency Injection)
    auto dalProduct = std::make_shared<hero::dal::DalProduct>();
    auto productService = new hero::business_logic::ProductService(dalProduct); // Managed by smarter pointer or app life

    // Instantiate controllers & services
    auto checkoutController = new hero::frontend::controllers::CheckoutController(&app);
    auto productController = new hero::frontend::controllers::ProductController(productService, &app);
    auto& financialService = hero::business_logic::FinancialService::getInstance();
    auto& alertSystem = hero::business_logic::AlertSystem::getInstance();
    
    // Scanner integration
    auto& scanner = hero::hal::ScannerListener::getInstance();
    scanner.attach();
    QObject::connect(&scanner, &hero::hal::ScannerListener::barcodeScanned,
                     checkoutController, &hero::frontend::controllers::CheckoutController::onBarcodeScanned);

    // Expose to QML
    engine.rootContext()->setContextProperty("checkoutController", checkoutController);
    engine.rootContext()->setContextProperty("productController", productController);
    engine.rootContext()->setContextProperty("financialService", &financialService);
    engine.rootContext()->setContextProperty("alertSystem", &alertSystem);
    engine.rootContext()->setContextProperty("barcodeGenerator", &hero::business_logic::BarcodeGenerator::getInstance());
    
    const QUrl url("qrc:/Supermarket/src/frontend/qml/Main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        qWarning() << "No root objects found. Did Main.qml load correctly?";
        return -1;
    }

    return app.exec();
}
