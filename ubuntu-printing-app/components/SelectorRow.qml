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
import Ubuntu.Components.ListItems 1.3 as ListItems

RowLayout {
    id: rowLayout
    anchors {
        left: parent.left
        leftMargin: units.gu(2)
        right: parent.right
        rightMargin: units.gu(2)
    }

    implicitHeight: selector.implicitHeight
    Layout.maximumWidth: width

    property alias delegate: selector.delegate
    property alias model: selector.model
    property var modelValue: model
    property alias selectedIndex: selector.selectedIndex
    property alias text: label.text

    readonly property var selectedValue: modelValue[selectedIndex]

    signal expansionCompleted()  // for qmltests

    Label {
        id: label
        elide: Text.ElideRight
        Layout.preferredWidth: units.gu(10)
        objectName: "label"
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }

    ListItems.ItemSelector {
        id: selector
        containerHeight: itemHeight * 4
        delegate: OptionSelectorDelegate {
            objectName: "option" + index
            text: modelData
        }
        Layout.fillWidth: true
        Layout.preferredWidth: units.gu(5)
//        Layout.minimumWidth: units.gu(20)
//        Layout.maximumWidth: units.gu(40)
        selectedIndex: 0

        onExpansionCompleted: rowLayout.expansionCompleted()
    }
}
