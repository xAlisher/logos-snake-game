import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            text: "SnakeGame"
            font.pixelSize: 24
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: backend.items
            delegate: Text {
                text: modelData.name
            }
        }

        Button {
            text: "Add Item"
            onClicked: backend.addItem("New Item")
        }
    }
}
