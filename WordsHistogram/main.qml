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
    width: 640
    height: 480
    visible: true
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
            var yMax = -1
            console.log("You chose: " + urlToPath(fileUrl.toString()))
            var stringCounter = fileParser.parseFile(urlToPath(fileUrl.toString()))
            wordsSeries.clear()
            for (var entry in stringCounter) {
//                console.log("(", entry, ", ", stringCounter[entry], ")")
                var value = stringCounter[entry]
                wordsSeries.append(String(entry), [stringCounter[entry]])
                if (value > yMax) {
                    yMax = value
                }
            }
            barSeriesYAxis.max = yMax
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    FileParser {
        id: fileParser
        onSignalEmitted: console.log("QML received the signal, message is:", msg)
    }

    GridLayout {
        id: gridLayout
        anchors.fill: parent

        ChartView {
            animationOptions: ChartView.SeriesAnimations
            theme: ChartView.ChartThemeBlueIcy
            title: "Words hist"
            anchors.fill: parent
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
                BarSet { label: "aa"; values: [2] }
            }

        }

        Button {
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            id: fileNameDialogBtn
            text: "Choose file"
            Layout.fillHeight: false
            Layout.fillWidth: false
            onClicked: fileDialog.visible = true
            antialiasing: true
            onHoveredChanged: hovered ? fileNameDialogBtnShadow.color = "#50000000" : fileNameDialogBtnShadow.color = "#70000000"
            onReleased: fileNameDialogBtnShadow.color = "#70000000"
            onPressed: fileNameDialogBtnShadow.color = "#30000000"
            DropShadow {
                id: fileNameDialogBtnShadow
                anchors.fill: fileNameDialogBtnRect
                cached: true
                horizontalOffset: 3
                verticalOffset: 3
                radius: 8.0
                samples: 16
                color: "#70000000"
                source: fileNameDialogBtnRect
            }
            background: Rectangle {
                id: fileNameDialogBtnRect
                implicitWidth: 120
                implicitHeight: 50
                border.color: "#aaa"
                border.width: 0
                color: "#ffffff"
                radius: 5
            }
        }
    }

}
