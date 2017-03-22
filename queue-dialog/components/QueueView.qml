/*
 * Copyright 2017 Canonical Ltd.
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
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Components.Extras.Printers 0.1

Flickable {
    id: flickable
    anchors {
        fill: parent
    }
    contentHeight: column.implicitHeight

    property alias filtersModel: repeaterModel.model
    property QtObject queueHelper: null

    Column {
        id: column
        anchors {
            left: parent.left
            right: parent.right
        }

        Repeater {
            id: repeaterModel
            delegate: Column {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                objectName: "filter" + index
                visible: jobFilterRepeater.count > 0

                readonly property var filter: model.filter
                readonly property int outerIndex: index  // used for QML tests
                readonly property string outerState: model.state

                ListItem {
                    divider {
                        visible: false
                    }
                    height: headerLayout.height

                    ListItemLayout {
                        id: headerLayout
                        padding {
                            bottom: units.gu(1)
                            top: units.gu(1)
                        }
                        title {
                            objectName: "filterHeaderLabel" + index
                            text: model.name
                            textSize: Label.Large
                        }
                    }
                }

                Repeater {
                    id: jobFilterRepeater
                    model: filter
                    objectName: "jobFilter" + index

                    delegate: QueueDelegate {
                        isActive: outerState === "active"
                        objectName: "jobFilter" + outerIndex + "delegate" + index

                        onCancel: {
                            console.debug("Cancel Job:", model.id, "for", model.printerName);
                            queueHelper.cancelJob(model.printerName, model.id);
                        }
                        onHold: {
                            console.debug("Hold Job:", model.id, "for", model.printerName);
                            queueHelper.holdJob(model.printerName, model.id);
                        }
                        onRelease: {
                            console.debug("Release Job:", model.id, "for", model.printerName);
                            queueHelper.releaseJob(model.printerName, model.id);
                        }
                    }
                }
            }
        }
    }
}
