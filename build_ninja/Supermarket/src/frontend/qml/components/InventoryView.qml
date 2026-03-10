import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: inventoryView

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        Label {
            text: "Inventory Management"
            font.pixelSize: 24
            font.bold: true
        }

        // Search and Actions Bar
        RowLayout {
            spacing: 15
            Layout.fillWidth: true

            TextField {
                id: searchBar
                placeholderText: "🔍 Search by name or barcode..."
                Layout.fillWidth: true
                onTextChanged: productController.searchProducts(text)
            }

            Button {
                text: "➕ Add New Product"
                onClicked: addProductDialog.open()
                background: Rectangle { color: "#0097e6"; radius: 6 }
                contentItem: Text { text: parent.text; color: "white"; font.bold: true; padding: 10 }
            }
            
            Button {
                text: "🔄 Refresh"
                onClicked: productController.refresh()
            }
        }

        // Product Grid/List
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "white"
            radius: 8
            border.color: "#dcdde1"

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // Header
                Rectangle {
                    Layout.fillWidth: true
                    height: 40
                    color: "#f5f6fa"
                    radius: 8
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 15
                        anchors.rightMargin: 15
                        Label { text: "Name"; Layout.fillWidth: true; font.bold: true }
                        Label { text: "Barcode"; Layout.preferredWidth: 150; font.bold: true }
                        Label { text: "Stock"; Layout.preferredWidth: 80; font.bold: true }
                        Label { text: "Price"; Layout.preferredWidth: 100; font.bold: true }
                        Label { text: "Actions"; Layout.preferredWidth: 100; font.bold: true }
                    }
                }

                ListView {
                    id: productList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: productController.productModel
                    delegate: ItemDelegate {
                        width: ListView.view.width
                        height: 50
                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 15
                            anchors.rightMargin: 15
                            Label { text: model.name; Layout.fillWidth: true }
                            Label { text: model.barcode; Layout.preferredWidth: 150; color: "#7f8c8d" }
                            Label { 
                                text: model.quantity; 
                                Layout.preferredWidth: 80; 
                                color: model.quantity <= 10 ? "#e84118" : "#2f3640";
                                font.bold: model.quantity <= 10
                            }
                            Label { text: "$" + Number(model.sellPrice).toFixed(2); Layout.preferredWidth: 100; font.bold: true }
                            RowLayout {
                                Layout.preferredWidth: 100
                                spacing: 10
                                Button {
                                    text: "🗑️"
                                    onClicked: productController.deleteProduct(model.id)
                                    flat: true
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Dialog {
        id: addProductDialog
        title: "Add New Product"
        modal: true
        standardButtons: Dialog.Save | Dialog.Cancel
        anchors.centerIn: parent
        width: 400

        ColumnLayout {
            width: parent.width - 40
            spacing: 15

            TextField { id: nameField; placeholderText: "Product Name"; Layout.fillWidth: true }
            
            RowLayout {
                TextField { id: buyPriceField; placeholderText: "Buy Price"; Layout.fillWidth: true; validator: DoubleValidator { bottom: 0 } }
                TextField { id: sellPriceField; placeholderText: "Sell Price"; Layout.fillWidth: true; validator: DoubleValidator { bottom: 0 } }
            }
            
            TextField { id: stockField; placeholderText: "Initial Stock"; Layout.fillWidth: true; validator: IntValidator { bottom: 0 } }
            
            RowLayout {
                TextField { id: barcodeField; placeholderText: "Barcode"; Layout.fillWidth: true }
                Button {
                    text: "✨"
                    onClicked: barcodeField.text = barcodeGenerator.generateEAN13()
                }
            }
        }

        onAccepted: {
            productController.addProduct(
                nameField.text,
                barcodeField.text,
                parseFloat(buyPriceField.text || "0"),
                parseFloat(sellPriceField.text || "0"),
                parseInt(stockField.text || "0"),
                "" // Expiration date placeholder
            )
            clearFields()
        }
        
        function clearFields() {
            nameField.clear()
            barcodeField.clear()
            buyPriceField.clear()
            sellPriceField.clear()
            stockField.clear()
        }
    }
}
