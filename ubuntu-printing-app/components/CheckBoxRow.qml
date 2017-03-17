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
import QtQuick.Layouts 1.1

import Ubuntu.Components 1.3

BaseRowLayout {
    property alias checked: checkbox.checked
    property alias checkboxText: checkboxLabel.text
    property alias enabled: checkbox.enabled
    property alias text: label.text

    Label {
        id: label
        Layout.preferredWidth: units.gu(10)
        objectName: "label"
    }

    MouseArea {
        enabled: checkbox.enabled
        Layout.fillWidth: true
        Layout.preferredHeight: units.gu(3)
        Layout.preferredWidth: units.gu(10)

        onClicked: checkbox.checked = !checkbox.checked

        Row {
            anchors {
                fill: parent
            }
            spacing: units.gu(1)

            CheckBox {
                id: checkbox
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                objectName: "checkbox"
            }

            Label {
                id: checkboxLabel
                enabled: checkbox.enabled
                height: parent.height
                objectName: "checkboxLabel"
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
