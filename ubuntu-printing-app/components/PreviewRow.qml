/*
 * Copyright 2016 Canonical Ltd.
 *
 * This file is part of ubuntu-printing-app.
 *
 * ubuntu-printing-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * ubuntu-printing-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored-by: Andrew Hayzen <andrew.hayzen@canonical.com>
 */
import QtQuick 2.4
import Ubuntu.Components 1.3

import UbuntuPrintingApp 1.0
import Ubuntu.Settings.Printers 0.1

Rectangle {
    id: previewRow
    anchors {
        left: parent.left
        right: parent.right
    }
    color: "#EEE"
    // Height is smallest of
    // - calc'd height using aspect and width of image
    // - 2/3 height of the view
    implicitHeight: Math.min((view.width - units.gu(10)) / pageHelper.aspect, view.height / 1.5)

    property Document document
    property var printerJob
    property var view

    // FIXME: QMLtests are not able to find by objectName, so make a property for now
    readonly property var pageHelper: PageHelper {
        document: previewRow.document
        objectName: "pageHelper"
        page: 0  // change to human style ?
    }

    Connections {  // reset to first page when document changes
        target: document
        onUrlChanged: pageHelper.page = 0
    }

    Button {
        anchors {
            right: previewImage.left
            rightMargin: units.gu(1)
            verticalCenter: previewImage.verticalCenter
        }
        enabled: pageHelper.page > 0
        color: "#000"
        objectName: "previousButton"
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
        objectName: "previewImage"
        source: pageHelper.document.url.toString() !== "" ? "image://poppler/" + pageHelper.page + "/" + (printerJob.colorModelType !== PrinterEnum.GrayType ? "true" : "false") + "/" + pageHelper.document.url : ""
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
            objectName: "activityIndicator"
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
        objectName: "nextButton"
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
            objectName: "overlayLabel"
            text: (pageHelper.page + 1) + "/" + document.count
            width: parent.width
        }
    }
}
