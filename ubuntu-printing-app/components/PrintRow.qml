import QtQuick 2.4
import QtQuick.Layouts 1.1

import Ubuntu.Components 1.3

Item {
    height: units.gu(6)

    property bool pdfMode: false
    property int sheets: 0

    signal cancel()
    signal confirm()

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            top: rowLayout.top
        }
        color: theme.palette.normal.base
        height: units.dp(1)
    }

    RowLayout {
        id: rowLayout
        anchors {
            fill: parent
        }
        spacing: units.gu(1)

        Button {
            objectName: "cancel"
            color: pressed ? Qt.darker(theme.palette.normal.background) : theme.palette.normal.background
            Layout.preferredWidth: units.gu(15)
            // Text color is determined from background color so set from html
            text: "<font color='" + theme.palette.normal.backgroundText + "'>" + i18n.tr("Cancel") + "</font>"

            onClicked: cancel()

            StyleHints {
                defaultColor: "black"
            }
        }

        Button {
            objectName: "button"
            color: pressed ? Qt.lighter("#111") : "#111"
            Layout.fillWidth: true
            text: pdfMode ? i18n.tr("Create PDF") : i18n.tr("Print") + " <u>" + sheets + "</u> " + i18n.tr("Sheets")

            onClicked: confirm()
        }
    }
}
