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
import QtQuick.Layouts 1.1

import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Components.Extras.Printers 0.1

MainView {
    id: mainView
    objectName: "mainView"
    height: units.gu(60)
    width: units.gu(60)

    PageStack {
        anchors {
            fill: parent
        }

        Page {  // Split into Page
            id: page
            header: PageHeader {
                flickable: flickable  // queueView
                leadingActionBar {
                    actions: [
                        Action {
                            iconName: "close"
                            objectName: "headerClose"

                            onTriggered: Qt.quit()
                        }
                    ]
                }
                subtitle: i18n.tr("%n job", "%n jobs", Printers.printJobs.count)
                title: i18n.tr("Printing")
                trailingActionBar {
                    actions: [
                        Action {
                            iconName: "settings"
                            objectName: "settings"

                            onTriggered: Qt.openUrlExternally("settings:///system/printing")
                        }
                    ]
                }
            }

            Flickable {
                id: flickable
                anchors {
                    fill: parent
                }
                contentHeight: column.implicitHeight

                Column {
                    id: column
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    Repeater {
                        model: ListModel {
                            id: filtersModel
                        }

                        delegate: Column {
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            visible: jobFilterRepeater.count > 0

                            readonly property var filter: model.filter
                            readonly property int outerIndex: index

                            ListItem {  // Split into component
                                divider {
                                    visible: false
                                }
                                height: headerLayout.height

                                ListItemLayout {
                                    id: headerLayout
                                    title {
                                        text: model.name
                                    }
                                }
                            }

                            Repeater {
                                id: jobFilterRepeater
                                model: filter

                                delegate: ListItem {  // TODO: split into component
                                    divider {
                                        visible: false
                                    }
                                    height: modelLayout.height + (divider.visible ? divider.height : 0)

                                    ListItemLayout {
                                        id: modelLayout
                                        subtitle {
                                            maximumLineCount: 3
                                            text: {
                                                if (model.state == PrinterEnum.Processing) {
                                                    if (model.impressionsCompleted) {
                                                        i18n.tr("Printing %n pages", "Printing %n page", model.impressionsCompleted)
                                                                + "\n" + model.printerName
                                                    } else {
                                                        i18n.tr("Printing")
                                                                + "\n" + model.printerName
                                                    }
                                                } else {
                                                    if (model.impressionsCompleted) {
                                                        i18n.tr("Ready to print %n pages", "Ready to print %n page", model.impressionsCompleted)
                                                                + "\n" + model.printerName
                                                    } else {
                                                        i18n.tr("Ready to print")
                                                                + "\n" + model.printerName
                                                    }
                                                }
                                            }
                                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                        }
                                        title {
                                            text: displayName
                                        }

                                        ActionBar {
                                            actions: [
                                                Action {
                                                    iconName: model.held ? "media-playback-start" : "media-playback-pause"
                                                    text: i18n.tr("Pause")

                                                    onTriggered: {
                                                        if (model.held) {
                                                            Printers.releaseJob(model.printerName, model.id);
                                                        } else {
                                                            Printers.holdJob(model.printerName, model.id);
                                                        }
                                                    }
                                                },
                                                Action {
                                                    iconName: "clear"
                                                    text: i18n.tr("Cancel")

                                                    onTriggered: Printers.cancelJob(model.printerName, model.id)
                                                }
                                            ]
                                            anchors {
                                                verticalCenter: parent.verticalCenter
                                            }
                                            height: modelLayout.height
                                            SlotsLayout.position: SlotsLayout.Trailing
                                            SlotsLayout.overrideVerticalPositioning: true
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Component.onCompleted: {
                var activeFilter = Printers.createJobFilter();
                activeFilter.filterOnActive();

                filtersModel.append(
                    {
                        filter: activeFilter,
                        name: i18n.tr("Active"),
                    }
                );


                var queuedFilter = Printers.createJobFilter();
                queuedFilter.filterOnQueued();

                filtersModel.append(
                    {
                        filter: queuedFilter,
                        name: i18n.tr("Queued"),
                    }
                );


                var pausedFilter = Printers.createJobFilter();
                pausedFilter.filterOnPaused();

                filtersModel.append(
                    {
                        filter: pausedFilter,
                        name: i18n.tr("Paused"),
                    }
                );

                console.debug("Model:", filtersModel.count)
            }
        }

        Component.onCompleted: push(page)
    }
}
