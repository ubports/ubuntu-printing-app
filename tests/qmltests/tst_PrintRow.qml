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

    PrintRow {
        id: printRow
        anchors {
            fill: parent
        }
    }

    SignalSpy {
        id: cancelSignal
        signalName: "cancel"
        target: printRow
    }

    SignalSpy {
        id: confirmSignal
        signalName: "confirm"
        target: printRow
    }

    UbuntuTestCase {
        name: "PrintRowTestCase"

        when: windowShown


        function init() {
            printRow.canPrint = true;
            printRow.pdfMode = false;
            printRow.sheets = 0;

            cancelSignal.clear();
            confirmSignal.clear();
        }

        function test_canPrint() {
            var button = findChild(printRow, "confirm");
            compare(button.enabled, true);

            // Set canPrint to false, as the printer is loading
            printRow.canPrint = false;

            // Check that the button becomes disabled
            compare(button.enabled, false);
        }

        function test_clickCancel() {
            var button = findChild(printRow, "cancel");
            var buttonCenter = centerOf(button);
            compare(cancelSignal.count, 0)

            // Click on the cancel button
            mouseClick(button);

            // Check that the cancel signal is emitted
            cancelSignal.wait();
            compare(cancelSignal.count, 1)
        }

        function test_clickConfirm() {
            var button = findChild(printRow, "confirm");
            var buttonCenter = centerOf(button);
            compare(confirmSignal.count, 0)

            // Click on the confirm button
            mouseClick(button);

            // Check that the confirm signal is emitted
            confirmSignal.wait();
            compare(confirmSignal.count, 1)
        }

        function test_pdfMode() {
            var button = findChild(printRow, "confirm");
            var originalText = button.text;

            // Change to Pdf mode
            printRow.pdfMode = true;

            // Check that the confirm button text changes
            verify(button.text !== originalText, 1000, "Text did not change when entering PDF Mode");
        }

        function test_sheets() {
            var button = findChild(printRow, "confirm");
            var originalText = button.text;

            // Change the number of sheets
            printRow.sheets = 10;

            // Check that the confirm button text changes
            verify(button.text !== originalText, 1000, "Text did not change when changing sheets");
        }
    }
}
