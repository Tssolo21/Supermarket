#include "backend/hal/printer_interface.hpp"
#include <iostream>
#include <fstream>

namespace hero::hal {

class DummyEscPosPrinter : public PrinterInterface {
public:
    bool connect(const std::string& portOrIp) override {
        connection_string_ = portOrIp;
        connected_ = true;
        std::cout << "[HAL_DUMMY] Connected to ESC/POS Printer at " << portOrIp << "\n";
        return true;
    }
    void disconnect() override {
        connected_ = false;
        std::cout << "[HAL_DUMMY] Disconnected from ESC/POS Printer\n";
    }
    bool isConnected() const override { return connected_; }
    
    bool printText(const std::string& text) override {
        if (!connected_) return false;
        std::ofstream log("dummy_receipt_printer.log", std::ios::app);
        log << text << "\n";
        return true;
    }
    bool printBarcode(const std::string& code) override {
        if (!connected_) return false;
        std::ofstream log("dummy_receipt_printer.log", std::ios::app);
        log << "[BARCODE: " << code << "]\n";
        return true;
    }
    bool cutPaper() override {
        if (!connected_) return false;
        std::ofstream log("dummy_receipt_printer.log", std::ios::app);
        log << "--- PAPER CUT ---\n";
        return true;
    }
    bool kickDrawer() override {
        if (!connected_) return false;
        std::ofstream log("dummy_receipt_printer.log", std::ios::app);
        log << "--- DRAWER KICKED ---\n";
        return true;
    }
private:
    bool connected_ = false;
    std::string connection_string_;
};

class DummyLabelPrinter : public LabelPrinterInterface {
public:
    bool connect(const std::string& portOrIp) override {
        connection_string_ = portOrIp;
        connected_ = true;
        std::cout << "[HAL_DUMMY] Connected to Label Printer at " << portOrIp << "\n";
        return true;
    }
    void disconnect() override {
        connected_ = false;
        std::cout << "[HAL_DUMMY] Disconnected from Label Printer\n";
    }
    bool isConnected() const override { return connected_; }
    
    bool printText(const std::string& text) override { return false; } // Not used
    bool printBarcode(const std::string& code) override { return false; } // Not used
    bool cutPaper() override { return false; }
    bool kickDrawer() override { return false; }

    bool printLabel(const std::string& name, const std::string& barcode, double price) override {
        if (!connected_) return false;
        std::ofstream log("dummy_label_printer.log", std::ios::app);
        log << "+------------------------+\n"
            << "| " << name << "\n"
            << "| Price: $" << price << "\n"
            << "| [||" << barcode << "||]  \n"
            << "+------------------------+\n";
        return true;
    }
private:
    bool connected_ = false;
    std::string connection_string_;
};

} // namespace hero::hal
