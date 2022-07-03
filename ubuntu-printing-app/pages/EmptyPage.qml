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
import QtQuick 2.12

import Ubuntu.Components 1.3

Page {
    id: page
    anchors {
        fill: parent
    }
    header: PageHeader {
        id: pageHeader
        leadingActionBar {
            actions: [
                Action {
                    iconName: "back"
                    objectName: "headerBack"

                    onTriggered: Qt.quit()
                }
            ]
        }
    }

    Label {
        anchors {
            centerIn: parent
        }
        horizontalAlignment: Text.AlignHCenter
        text: i18n.tr("No document was chosen for printing. In order to print, you need to select print from an app. Alternatively you can export a PDF document from an app.")
        width: parent.width - units.gu(1)
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }
}
