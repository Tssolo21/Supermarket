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

        RowLayout {
            TextField {
                id: nameField
                placeholderText: "Product Name"
                Layout.fillWidth: true
            }
            TextField {
                id: priceField
                placeholderText: "Sell Price"
                validator: DoubleValidator { bottom: 0 }
            }
            TextField {
                id: stockField
                placeholderText: "Initial Stock"
                validator: IntValidator { bottom: 0 }
            }
        }

        RowLayout {
            TextField {
                id: barcodeField
                placeholderText: "Barcode"
                Layout.fillWidth: true
            }
            Button {
                text: "✨ Generate Barcode"
                onClicked: barcodeField.text = barcodeGenerator.generateEAN13()
            }
        }

        Button {
            text: "Add Product"
            Layout.fillWidth: true
            height: 50
            background: Rectangle { color: "#44bd32"; radius: 6 }
            contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            onClicked: {
                // To be linked to ProductManager controller in real implementation
                console.log("Adding product with barcode: " + barcodeField.text)
                barcodeField.clear()
                nameField.clear()
                priceField.clear()
                stockField.clear()
            }
        }
        
        Item { Layout.fillHeight: true }
    }
}
