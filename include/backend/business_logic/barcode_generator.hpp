#pragma once
#include <QObject>
#include <QString>
#include <random>

namespace hero::business_logic {

class BarcodeGenerator : public QObject {
    Q_OBJECT
public:
    static BarcodeGenerator& getInstance() {
        static BarcodeGenerator instance;
        return instance;
    }

    Q_INVOKABLE QString generateEAN13() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9);
        
        QString barcode;
        int sumStr = 0;
        for (int i = 0; i < 12; ++i) {
            int digit = dis(gen);
            barcode += QString::number(digit);
            sumStr += (i % 2 == 0) ? digit : digit * 3;
        }
        
        int checksum = (10 - (sumStr % 10)) % 10;
        barcode += QString::number(checksum);
        return barcode;
    }

private:
    BarcodeGenerator() = default;
};

} // namespace hero::business_logic
