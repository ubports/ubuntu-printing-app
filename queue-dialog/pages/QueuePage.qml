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

import Ubuntu.Components.Extras.Printers 0.1
import "../components"

Page {
    id: page
    header: PageHeader {
        subtitle: i18n.tr("%1 job", "%1 jobs", queueHelper.count).arg(queueHelper.count)
        title: i18n.tr("Printing")
        trailingActionBar {
            actions: [
                Action {
                    iconName: "settings"
                    objectName: "headerSettings"

                    onTriggered: page.settings()
                }
            ]
        }
    }
    objectName: "queuePage"

    property QtObject queueHelper: null

    signal settings()

    ScrollView {
        id: scrollView
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            top: page.header.bottom
        }

        QueueView {
            anchors {
                fill: parent
            }
            filtersModel: ListModel {
                id: filtersModel
                objectName: "filtersModel"
            }
            objectName: "queueView"
            queueHelper: page.queueHelper
        }
    }

    Label {
        anchors {
            centerIn: parent
        }
        objectName: "emptyLabel"
        text: i18n.tr("No printer jobs")
        visible: queueHelper.count === 0
    }

    Component.onCompleted: {
        filtersModel.append(
            {
                filter: queueHelper.createActiveFilter(),
                name: i18n.tr("Active"),
                state: "active",
            }
        );

        filtersModel.append(
            {
                filter: queueHelper.createQueuedFilter(),
                name: i18n.tr("Queued"),
                state: "queued",
            }
        );

        filtersModel.append(
            {
                filter: queueHelper.createPausedFilter(),
                name: i18n.tr("Paused"),
                state: "paused",
            }
        );
    }
}
