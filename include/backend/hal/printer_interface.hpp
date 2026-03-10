#pragma once
#include <string>

namespace hero::hal {

class PrinterInterface {
public:
    virtual ~PrinterInterface() = default;
    
    virtual bool connect(const std::string& portOrIp) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    virtual bool printText(const std::string& text) = 0;
    virtual bool printBarcode(const std::string& code) = 0;
    virtual bool cutPaper() = 0;
    virtual bool kickDrawer() = 0;
};

// Abstract factory or generic interface for label specific operations
class LabelPrinterInterface : public PrinterInterface {
public:
    virtual ~LabelPrinterInterface() = default;
    virtual bool printLabel(const std::string& name, const std::string& barcode, double price) = 0;
};

} // namespace hero::hal
