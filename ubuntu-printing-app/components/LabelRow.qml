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
    property alias enabled: secondaryLabel.enabled
    property alias primaryText: primaryLabel.text
    property alias secondaryText: secondaryLabel.text

    Label {
        id: primaryLabel
        Layout.preferredWidth: units.gu(10)
        objectName: "primary"
    }

    Label {
        id: secondaryLabel
        Layout.fillWidth: true
        Layout.preferredHeight: units.gu(3)
        Layout.preferredWidth: units.gu(10)
        objectName: "secondary"
        verticalAlignment: Text.AlignVCenter
    }
}
