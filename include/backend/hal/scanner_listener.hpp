#pragma once
#include <QObject>
#include <QString>
#include <QElapsedTimer>

class QKeyEvent;
class QEvent;

namespace hero::hal {

/**
 * Filter for detecting rapid key presses indicative of a barcode scanner.
 * Installs as an event filter on the QApplication.
 */
class ScannerListener : public QObject {
    Q_OBJECT
public:
    static ScannerListener& getInstance() {
        static ScannerListener instance;
        return instance;
    }

    // Installs filter
    void attach();
    void detach();

    bool eventFilter(QObject* obj, QEvent* event) override;

signals:
    void barcodeScanned(const QString& barcode);

private:
    ScannerListener() = default;
    ~ScannerListener() override { detach(); }

    QElapsedTimer timer_;
    QString currentBuffer_;
    const qint64 MAX_MS_BETWEEN_KEYPRESS = 30; // 30ms max between chars for typical scanner
    bool isAttached_ = false;
};

} // namespace hero::hal
