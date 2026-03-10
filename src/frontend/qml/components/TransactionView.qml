import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: transactionView
    
    // Global hotkeys handling
    Shortcut {
        sequence: "F9"
        onActivated: validateCheckout()
    }
    Shortcut {
        sequence: "F12"
        onActivated: discountDialog.open()
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Cart List View
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#ffffff"
            radius: 8
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                
                Label {
                    text: "Current Transaction"
                    font.pixelSize: 20
                    font.bold: true
                }
                
                ListView {
                    id: cartList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: ListModel { id: cartModel }
                    delegate: ItemDelegate {
                        width: ListView.view.width
                        height: 60
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 5
                            Label { text: model.name; Layout.fillWidth: true; font.pixelSize: 18 }
                            Label { text: "x" + model.quantity; font.pixelSize: 16 }
                            Label { text: "$" + Number(model.subtotal).toFixed(2); font.bold: true; font.pixelSize: 18 }
                            Button {
                                text: "❌"
                                onClicked: checkoutController.removeItem(index)
                            }
                        }
                    }
                }
            }
        }
        
        // Processing Panel
        Rectangle {
            Layout.preferredWidth: 350
            Layout.fillHeight: true
            color: "#ffffff"
            radius: 8
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Label {
                    text: "Payment Summary"
                    font.pixelSize: 20
                    font.bold: true
                }
                
                Rectangle {
                    Layout.fillWidth: true
                    height: 2
                    color: "#f5f6fa"
                }
                
                Label {
                    id: totalLabel
                    text: "Total: $" + Number(checkoutController.cartTotal).toFixed(2)
                    font.pixelSize: 32
                    font.bold: true
                    color: "#44bd32"
                    Layout.alignment: Qt.AlignHCenter
                }
                
                Item { Layout.fillHeight: true }
                
                Button {
                    text: "Apply Discount (F12)"
                    Layout.fillWidth: true
                    height: 50
                    font.pixelSize: 16
                    onClicked: discountDialog.open()
                }
                
                Button {
                    text: "Validate & Pay (F9)"
                    Layout.fillWidth: true
                    height: 70
                    font.pixelSize: 20
                    font.bold: true
                    background: Rectangle { color: "#0097e6"; radius: 6 }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: validateCheckout()
                }
                
                Button {
                    text: "Cancel Transaction"
                    Layout.fillWidth: true
                    height: 50
                    font.pixelSize: 16
                    background: Rectangle { color: "#e84118"; radius: 6 }
                    contentItem: Text { text: parent.text; color: "white"; font: parent.font; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    onClicked: checkoutController.clearCart()
                }
            }
        }
    }
    
    Dialog {
        id: discountDialog
        title: "Apply Discount"
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent
        ColumnLayout {
            Label { text: "Discount Percentage (%):" }
            TextField {
                id: discountInput
                validator: DoubleValidator { bottom: 0; top: 100 }
            }
        }
        onAccepted: {
            checkoutController.applyDiscount(parseFloat(discountInput.text || "0"));
        }
    }
    
    Connections {
        target: checkoutController
        function onCartUpdated(items) {
            cartModel.clear();
            for (var i = 0; i < items.length; i++) {
                cartModel.append(items[i]);
            }
        }
        function onCheckoutCompleted(success, message) {
            msgDialog.title = "Success";
            msgDialog.text = message;
            msgDialog.open();
        }
        function onCheckoutError(error) {
            msgDialog.title = "Error";
            msgDialog.text = error;
            msgDialog.open();
        }
    }
    
    Dialog {
        id: msgDialog
        property alias text: msgText.text
        standardButtons: Dialog.Ok
        anchors.centerIn: parent
        Label { id: msgText; text: "" }
    }

    function validateCheckout() {
        checkoutController.validateCheckout("CASH");
    }
}
