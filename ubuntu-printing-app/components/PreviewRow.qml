/*
 * Copyright 2016, 2017 Canonical Ltd.
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
import Ubuntu.Components.Extras.Printers 0.1

Rectangle {
    id: previewRow
    anchors {
        left: parent.left
        right: parent.right
    }
    // Use foreground as this element is ontop of the background
    color: theme.palette.normal.foreground

    // Height is the calculated height + the difference given by the separator
    // but also has a minimumHeight, otherwise it can be resized too small
    implicitHeight: Math.max(separator.heightDiff + calcHeight, minimumHeight)

    // Calculated height is smallest of
    // - calc'd height using aspect and width of image
    // - 40% height of the view
    readonly property double calcHeight: Math.min((view.width - units.gu(10)) / pageHelper.aspect, view.height * 0.4)

    property Document document
    property double minimumHeight: units.gu(15)
    // This is the MouseArea that the separator uses to monitor when to change
    // the cursorShape
    property MouseArea monitorMouseArea: null
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
        // Use foregroundText as this is ontop of foreground
        // and we want the button to be flipped colours
        color: theme.palette.normal.foregroundText
        objectName: "previousButton"
        text: "<"
        width: units.gu(4)
        visible: enabled

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
        // Use foregroundText as this is ontop of foreground
        // and we want the button to be flipped colours
        color: theme.palette.normal.foregroundText
        enabled: pageHelper.page < document.count - 1
        objectName: "nextButton"
        text: ">"
        width: units.gu(4)
        visible: enabled

        onClicked: pageHelper.page++
    }

    Rectangle {
        id: pageOverlay
        anchors {
            bottom: separator.top
            left: previewImage.left
            right: previewImage.right
        }
        // Use foregroundText as this is ontop of foreground
        color: theme.palette.normal.foregroundText
        height: units.gu(4)

        Label {
            anchors {
                centerIn: parent
            }
            // Use foreground as this is ontop of foregroundText
            // and we want this to have flipped colours
            color: theme.palette.normal.foreground
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
            objectName: "overlayLabel"
            text: (pageHelper.page + 1) + "/" + document.count
            width: parent.width
        }
    }

    Item {
        id: separator
        anchors {
            bottom: parent.bottom
        }
        height: units.gu(1)
        objectName: "separator"
        width: parent.width

        readonly property alias heightDiff: resizer.y

        // So that qmltests can find the resizer (it has no parent)
        readonly property var resizer: resizer

        Rectangle {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            color: theme.palette.normal.base
            height: 1
        }

        Item {
            id: resizer
            parent: null
        }

        MouseArea {
            id: mouseArea
            anchors {
                fill: parent
            }
            cursorShape: Qt.SizeVerCursor
            drag {
                axis: Drag.YAxis
                // Prevent the view being resized smaller than minimumHeight
                // otherwise you cannot resize it back
                minimumY: -calcHeight + previewRow.minimumHeight
                target: resizer
            }

            // ScrollView has a MouseArea which doesn't propagate hover events
            // down to us. So instead we have a global MouseArea that is above
            // the ScrollView and passed to us as monitorMouseArea
            //
            // monitorContainsMouse then tells us if the mouse in the global
            // MouseArea is also inside this MouseArea
            readonly property bool monitorContainsMouse: {
                var relativePos = mapFromItem(monitorMouseArea, monitorMouseArea.mouseX, monitorMouseArea.mouseY);
                return contains(Qt.point(relativePos.x, relativePos.y));
            }

            // When the mouse is inside this MouseArea set the global MouseArea
            // cursorShape to be the cursor we want
            Binding {
                target: monitorMouseArea
                property: "cursorShape"
                value: mouseArea.monitorContainsMouse ? Qt.SizeVerCursor : Qt.ArrowCursor
            }
        }
    }
}
