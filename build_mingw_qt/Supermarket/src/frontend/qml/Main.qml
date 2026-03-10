import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Supermarket

ApplicationWindow {
    id: root
    width: 1280
    height: 800
    visible: true
    title: qsTr("Hirro POS System")
    
    // Aesthetic Material/Premium colors
    color: "#f5f6fa" // Light aesthetic background
    
    // Header
    header: ToolBar {
        background: Rectangle {
            color: "#2f3640"
            implicitHeight: 60
        }
        RowLayout {
            anchors.fill: parent
            anchors.margins: 16
            
            Label {
                text: "🛒 Hirro System"
                font.pixelSize: 24
                font.bold: true
                color: "#ffffff"
                Layout.alignment: Qt.AlignVCenter
            }
            
            Item { Layout.fillWidth: true } // Spacer
            
            Label {
                text: new Date().toLocaleTimeString(Qt.locale(), "hh:mm ap")
                font.pixelSize: 16
                color: "#dcdde1"
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }

    // Sidebar & Main Content Stack
    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Sidebar Navigation
        Rectangle {
            width: 250
            Layout.fillHeight: true
            color: "#353b48"

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 20
                spacing: 1
                
                Button {
                    text: "📊 Dashboard"
                    Layout.fillWidth: true
                    height: 50
                    font.pixelSize: 16
                    font.bold: stackLayout.currentIndex === 0
                    onClicked: stackLayout.currentIndex = 0
                }
                Button {
                    text: "💳 Checkout (F2)"
                    Layout.fillWidth: true
                    height: 50
                    font.pixelSize: 16
                    font.bold: stackLayout.currentIndex === 1
                    onClicked: stackLayout.currentIndex = 1
                }
                Button {
                    text: "📦 Inventory"
                    Layout.fillWidth: true
                    height: 50
                    font.pixelSize: 16
                    font.bold: stackLayout.currentIndex === 2
                    onClicked: stackLayout.currentIndex = 2
                }
                Item { Layout.fillHeight: true } // Spacer
            }
        }

        // Main Content Area
        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: 1 // Default to Checkout
            
            DashboardView {}
            TransactionView {}
            InventoryView {}
        }
    }
}
