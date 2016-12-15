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
import QtQuick.Layouts 1.1

import Ubuntu.Components 1.3

Item {
    height: units.gu(6)

    property bool pdfMode: false
    property int sheets: 0

    signal cancel()
    signal confirm()

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            top: rowLayout.top
        }
        color: theme.palette.normal.base
        height: units.dp(1)
    }

    RowLayout {
        id: rowLayout
        anchors {
            fill: parent
        }
        spacing: units.gu(1)

        Button {
            objectName: "cancel"
            color: pressed ? Qt.darker(theme.palette.normal.background) : theme.palette.normal.background
            Layout.preferredWidth: units.gu(15)
            // Text color is determined from background color so set from html
            text: "<font color='" + theme.palette.normal.backgroundText + "'>" + i18n.tr("Cancel") + "</font>"

            onClicked: cancel()

            StyleHints {
                defaultColor: "black"
            }
        }

        Button {
            objectName: "button"
            color: pressed ? Qt.lighter("#111") : "#111"
            Layout.fillWidth: true
            text: pdfMode ? i18n.tr("Create PDF") : i18n.tr("Print") + " <u>" + sheets + "</u> " + i18n.tr("Sheets")

            onClicked: confirm()
        }
    }
}
