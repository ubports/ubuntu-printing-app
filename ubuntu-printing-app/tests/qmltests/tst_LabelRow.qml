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
import "../../components"

Item {
    width: units.gu(100)
    height: units.gu(75)

    LabelRow {
        id: labelRow
    }

    UbuntuTestCase {
        name: "LabelRowTestCase"
        when: windowShown

        readonly property bool dataEnabled: true
        readonly property string dataPrimaryText: "Primary"
        readonly property string dataSecondaryText: "Secondary"

        function init() {
            labelRow.enabled = dataEnabled;
            labelRow.primaryText = dataPrimaryText;
            labelRow.secondaryText = dataSecondaryText;

            waitForRendering(labelRow);
        }

        function test_enabled() {
            // Check that label enabled state is at the starting state
            var secondary = findChild(labelRow, "secondary");
            compare(labelRow.enabled, dataEnabled);

            // Flip the enabled state
            labelRow.enabled = !dataEnabled;

            // Check that the label enabled state changes
            tryCompare(secondary, "enabled", !dataEnabled);
        }

        function test_primaryText() {
            // Check primary text is correct
            var primary = findChild(labelRow, "primary");
            compare(primary.text, dataPrimaryText);
        }

        function test_secondaryText() {
            // Check secondary text is correct
            var secondary = findChild(labelRow, "secondary");
            compare(secondary.text, dataSecondaryText);
        }
    }
}
