#include "backend/hal/scanner_listener.hpp"
#include <QGuiApplication>
#include <QKeyEvent>
#include <QEvent>

namespace hero::hal {

void ScannerListener::attach() {
    if (!isAttached_ && qGuiApp) {
        qGuiApp->installEventFilter(this);
        isAttached_ = true;
    }
}

void ScannerListener::detach() {
    if (isAttached_ && qGuiApp) {
        qGuiApp->removeEventFilter(this);
        isAttached_ = false;
    }
}

bool ScannerListener::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        
        // If Enter is pressed, evaluate if it's a barcode
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (currentBuffer_.length() >= 5) { // Minimum length for typical barcode
                emit barcodeScanned(currentBuffer_);
                currentBuffer_.clear();
                return true; // Consume event
            }
            currentBuffer_.clear();
            return false;
        }

        // Only consider printable characters
        if (!keyEvent->text().isEmpty() && keyEvent->text().at(0).isPrint()) {
            if (!currentBuffer_.isEmpty() && timer_.isValid()) {
                if (timer_.elapsed() > MAX_MS_BETWEEN_KEYPRESS) {
                    // Too slow, user is likely typing manually, not a scan
                    currentBuffer_.clear();
                }
            }
            currentBuffer_.append(keyEvent->text());
            timer_.start();
            
            // Passively listen so we don't block normal typing
        }
    }
    return QObject::eventFilter(obj, event);
}

} // namespace hero::hal
