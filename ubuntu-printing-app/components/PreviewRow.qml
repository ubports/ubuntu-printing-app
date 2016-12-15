
import QtQuick 2.4
import Ubuntu.Components 1.3

import Ubuntu_Printing_App 1.0

Rectangle {
    anchors {
        left: parent.left
        right: parent.right
    }
    color: "#EEE"
    // Height is smallest of
    // - calc'd height using aspect and width of image
    // - 2/3 height of the view
    implicitHeight: Math.min((view.width - units.gu(10)) / pageHelper.aspect, view.height / 1.5)

    property alias document: pageHelper.document
    property var printer
    property var view

    PageHelper {
        id: pageHelper
        document: document
        page: 0  // change to human style ?
    }

    Button {
        anchors {
            right: previewImage.left
            rightMargin: units.gu(1)
            verticalCenter: previewImage.verticalCenter
        }
        enabled: pageHelper.page > 0
        color: "#000"
        text: "<"
        width: units.gu(4)

        onClicked: pageHelper.page--
    }

    Image {
        id: previewImage
        anchors {
            bottom: pageOverlay.top
            horizontalCenter: parent.horizontalCenter
            top: parent.top
        }
        asynchronous: true
        source: pageHelper.document.url.toString() !== "" ? "image://poppler/" + pageHelper.page + "/" + printer.colorMode + "/" + pageHelper.document.url : ""
        sourceSize {
            height: previewImage.height
            width: previewImage.width
        }
        // Width is smallest of
        // - calc'd width using aspect and height of image
        // - container width subtracting width of other items
        width: Math.min(height * pageHelper.aspect, parent.width - (units.gu(4) + units.gu(1) + units.gu(4) + units.gu(1)))

        ActivityIndicator {
            anchors {
                centerIn: parent
            }
            running: previewImage.status == Image.Loading
        }
    }

    Button {
        anchors {
            left: previewImage.right
            leftMargin: units.gu(1)
            verticalCenter: previewImage.verticalCenter
        }
        color: "#000"
        enabled: pageHelper.page < document.count - 1
        text: ">"
        width: units.gu(4)

        onClicked: pageHelper.page++
    }

    Rectangle {
        id: pageOverlay
        anchors {
            bottom: parent.bottom
            left: previewImage.left
            right: previewImage.right
        }
        color: "#000"
        height: units.gu(4)
        opacity: 0.8

        Label {
            anchors {
                centerIn: parent
            }
            color: "#EEE"
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
            text: (pageHelper.page + 1) + "/" + document.count
            width: parent.width
        }
    }
}
