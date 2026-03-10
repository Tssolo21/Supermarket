import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: dashboardView

    property var metrics: financialService.getTodayMetrics()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        Label {
            text: "Financial Dashboard"
            font.pixelSize: 24
            font.bold: true
        }

        RowLayout {
            spacing: 20
            Layout.fillWidth: true

            Rectangle {
                Layout.fillWidth: true
                height: 120
                color: "#0097e6"
                radius: 8
                ColumnLayout {
                    anchors.centerIn: parent
                    Label { text: "Total Revenue"; color: "white"; font.pixelSize: 16 }
                    Label { text: "$" + Number(metrics.totalRevenue || 0).toFixed(2); color: "white"; font.pixelSize: 28; font.bold: true }
                }
            }
            Rectangle {
                Layout.fillWidth: true
                height: 120
                color: "#e1b12c"
                radius: 8
                ColumnLayout {
                    anchors.centerIn: parent
                    Label { text: "Net Profit"; color: "white"; font.pixelSize: 16 }
                    Label { text: "$" + Number(metrics.netProfit || 0).toFixed(2); color: "white"; font.pixelSize: 28; font.bold: true }
                }
            }
            Rectangle {
                Layout.fillWidth: true
                height: 120
                color: "#8c7ae6"
                radius: 8
                ColumnLayout {
                    anchors.centerIn: parent
                    Label { text: "Profit Margin"; color: "white"; font.pixelSize: 16 }
                    Label { text: Number(metrics.profitMargin || 0).toFixed(1) + "%"; color: "white"; font.pixelSize: 28; font.bold: true }
                }
            }
        }

        Label {
            text: "System Alerts"
            font.pixelSize: 20
            font.bold: true
            Layout.topMargin: 20
        }

        ListView {
            id: alertList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: ListModel { id: alertsModel }
            delegate: Rectangle {
                width: ListView.view.width
                height: 50
                color: "#f5f6fa"
                border.width: 1
                border.color: "#dcdde1"
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    Label { text: model.message; color: model.isWarning ? "#e84118" : "#fbc531"; font.pixelSize: 16; font.bold: true }
                }
            }
        }
    }

    Connections {
        target: alertSystem
        function onLowStockAlert(name, stock) {
            alertsModel.insert(0, { message: "⚠️ Low Stock: " + name + " (Only " + stock + " left)", isWarning: true })
        }
        function onExpirationAlert(name, date) {
            alertsModel.insert(0, { message: "⛔ Expired: " + name + " on " + date, isWarning: true })
        }
    }
    Connections {
        target: financialService
        function onMetricsUpdated() {
            metrics = financialService.getTodayMetrics()
        }
    }
    
    Component.onCompleted: {
        alertSystem.runDailyCheck()
    }
}
