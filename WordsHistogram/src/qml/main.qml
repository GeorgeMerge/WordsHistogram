import QtQuick 2.12
import QtQuick.Window 2.12
import QtCharts 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import FileParser 1.0

Window {
    width: 805
    height: 600
    visible: true
    color: "#f4f5f5"
    title: qsTr("Words Histogram")

    function urlToPath(str)
    {
        return (String(str).slice(9))
    }

    FileDialog {
        id: fileDialog
        title: "Choose a file"
        folder: shortcuts.home
        nameFilters: ["Text files (*.txt)"]
        onAccepted: {
            fileParser.parseFileInSeparateThread(urlToPath(fileUrl.toString()))
            progressBar.visible = true
            progressBar.value = 0
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    FileParser {
        id: fileParser
        onProgressChanged: {
            progressBar.value = value
        }
        onFileParsed: {
            var yMax = -1
            wordsSeries.clear()
            for (var entry in resultMap) {
                var value = resultMap[entry]
                wordsSeries.append(String(entry), [resultMap[entry]])
                if (value > yMax) {
                    yMax = value
                }
            }
            barSeriesYAxis.max = yMax
        }
    }

    Row {
        id: row
        anchors.fill: parent
        spacing: 5

        Column {
            id: column
            width: 125
            height: 600
            spacing: 0

            Item {
                id: item1
                width: 125
                height: 9
            }

            Button {
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                id: fileNameDialogBtn
                x: 5
                width: 123
                anchors.topMargin: 100
                text: "Choose file"
                font.letterSpacing: 0.6
                font.preferShaping: false
                font.bold: false
                font.pointSize: 10
                rightPadding: 0
                leftPadding: 0
                topPadding: 0
                bottomPadding: 0
                wheelEnabled: true
                spacing: 6
                Layout.fillHeight: false
                Layout.fillWidth: false
                onClicked: fileDialog.visible = true
                antialiasing: true
                onHoveredChanged: hovered ? fileNameDialogBtnShadow.color = "#50000000" : fileNameDialogBtnShadow.color = "#70000000"
                onReleased: fileNameDialogBtnShadow.color = "#70000000"
                onPressed: fileNameDialogBtnShadow.color = "#30000000"
                DropShadow {
                    id: fileNameDialogBtnShadow
                    x: 0
                    width: 120
                    anchors.fill: fileNameDialogBtnRect
                    cached: true
                    horizontalOffset: 3
                    verticalOffset: 3
                    radius: 8.0
                    samples: 16
                    color: "#70000000"
                    source: fileNameDialogBtnRect
                    anchors.rightMargin: 0
                    anchors.bottomMargin: 0
                    anchors.leftMargin: 0
                    anchors.topMargin: 0
                }
                background: Rectangle {
                    id: fileNameDialogBtnRect
                    implicitWidth: 120
                    implicitHeight: 50
                    border.color: "#aaa"
                    border.width: 0
                    color: "#fdfcff"
                    radius: 5
                }
            }

            ProgressBar {
                id: progressBar
                x: 5
                width: 123
                height: 36
                value: 0.0
                visible: false
                from: 0.0
                to: 100.0
            }

        }

        ChartView {
            animationOptions: ChartView.SeriesAnimations
            theme: ChartView.ChartThemeBlueIcy
            title: "Words histogram"
            titleFont.family: "Segoe UI Symbol"
            legend.font.family: "Segoe UI Symbol"
            legend.font.pointSize: 8
            titleFont.pointSize: 14
            anchors.fill: parent
            anchors.rightMargin: 0
            anchors.bottomMargin: 0
            anchors.topMargin: 0
            anchors.leftMargin: 125
            legend.alignment: Qt.AlignBottom
            antialiasing: true
            BarSeries {
                id: wordsSeries
                Layout.fillWidth: true
                axisX: BarCategoryAxis { categories: [" "] }
                axisY: ValueAxis {
                    id: barSeriesYAxis
                }
            }

        }
    }
}
