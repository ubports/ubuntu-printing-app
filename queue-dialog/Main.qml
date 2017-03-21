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
import QtQuick.Window 2.2

import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems

import "components"

// Use a Window as UriHandler requests need to call Window.requestActivate()
Window {
    id: window
    contentOrientation: Screen.orientation
    minimumHeight: units.gu(20)
    minimumWidth: units.gu(20)
    height: units.gu(60)
    width: units.gu(60)

    // Nest a MainView inside the Window so that we can set the i18n domain
    // from QML to ubuntu-printing-app as we share the .pot
    MainView {
        id: mainView
        anchors {
            fill: parent
        }
        applicationName: "ubuntu-printing-app"

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
            target: mainView.queuePage

            onSettings: Qt.openUrlExternally("settings:///system/printing")
        }

        Connections {
            target: UriHandler
            onOpened: window.requestActivate()
        }

        Component.onCompleted: {
            mainView.queuePage = pageStack.push(
                Qt.resolvedUrl("pages/QueuePage.qml"),
                {
                    "queueHelper": queueHelper,
                }
            );
        }
    }
}
