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

ListItem {
    divider {
        visible: false
    }
    height: modelLayout.height + (divider.visible ? divider.height : 0)

    property bool isActive: false

    signal cancel()
    signal hold()
    signal release()

    ListItemLayout {
        id: modelLayout
        subtitle {
            maximumLineCount: 3
            objectName: "subtitleLabel"
            text: {
                if (isActive) {
                    if (model.impressionsCompleted) {
                        i18n.tr("Printing %1 page", "Printing %1 pages", model.impressionsCompleted).arg(model.impressionsCompleted)
                                + "\n" + model.printerName
                    } else {
                        i18n.tr("Printing")
                                + "\n" + model.printerName
                    }
                } else {
                    if (model.impressionsCompleted) {
                        i18n.tr("Ready to print %1 page", "Ready to print %1 pages", model.impressionsCompleted).arg(model.impressionsCompleted)
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
            objectName: "titleLabel"
            text: model.title
        }

        ActionBar {
            actions: [
                Action {
                    iconName: model.held ? "media-playback-start" : "media-playback-pause"
                    objectName: "toggle"
                    text: model.held ? i18n.tr("Release") : i18n.tr("Hold")

                    onTriggered: {
                        if (model.held) {
                            release();
                        } else {
                            hold();
                        }
                    }
                },
                Action {
                    iconName: "clear"
                    objectName: "cancel"
                    text: i18n.tr("Cancel")

                    onTriggered: cancel();
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
