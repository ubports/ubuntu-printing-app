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

import "components"

MainView {
    // Set the application name to "ubuntu-printing-app" as we want to share
    // the i18n domain to share the same .pot file
    applicationName: "ubuntu-printing-app"
    height: units.gu(60)
    width: units.gu(60)

    property Page queuePage: null

    PageStack {
        id: pageStack
        anchors {
            fill: parent
        }
    }

    QueueHelper {
        id: queueHelper
    }

    Connections {
        target: queuePage

        onSettings: Qt.openUrlExternally("settings:///system/printing")
    }

    Component.onCompleted: {
        queuePage = pageStack.push(
            Qt.resolvedUrl("pages/QueuePage.qml"),
            {
                "queueHelper": queueHelper,
            }
        );
    }
}
