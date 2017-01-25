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
import QtTest 1.1
import Ubuntu.Test 1.0
import "../../ubuntu-printing-app/components"

Item {
    width: units.gu(100)
    height: units.gu(75)

    CheckBoxRow {
        id: checkBoxRow
    }

    UbuntuTestCase {
        name: "CheckBoxRowTestCase"
        when: windowShown

        readonly property bool dataChecked: false
        readonly property string dataCheckboxText: "checkboxText"
        readonly property bool dataEnabled: true
        readonly property string dataText: "text"

        function init() {
            checkBoxRow.checked = dataChecked;
            checkBoxRow.checkboxText = dataCheckboxText;
            checkBoxRow.enabled = dataEnabled;
            checkBoxRow.text = dataText;

            waitForRendering(checkBoxRow);
        }

        function test_checked() {
            // Check checkbox checked state is starting value
            var checkbox = findChild(checkBoxRow, "checkbox");
            compare(checkBoxRow.checked, dataChecked);

            mouseClick(checkbox);

            tryCompare(checkBoxRow, "checked", !dataChecked);
        }

        function test_checkboxText() {
            // Check that the checkbox label is correct
            var label = findChild(checkBoxRow, "checkboxLabel");
            compare(label.text, dataCheckboxText);
        }

        function test_enabled() {
            // Check checkbox enabled state is starting value
            var checkbox = findChild(checkBoxRow, "checkbox");
            compare(checkBoxRow.enabled, dataEnabled);

            // Flip the enabled state
            checkBoxRow.enabled = !dataEnabled;

            // Check that the enabled state changes
            tryCompare(checkbox, "enabled", !dataEnabled);
        }

        function test_text() {
            // Check that the text is correct
            var label = findChild(checkBoxRow, "label");
            compare(label.text, dataText);
        }
    }
}
