import QtQuick 2.4
import QtTest 1.1
import Ubuntu.Test 1.0
import "../../ubuntu-printing-app/components"

import Ubuntu_Printing_App 1.0
import Ubuntu.Settings.Printers 0.1

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
        name: "MainTestCase"

        when: windowShown


        function init() {
            printRow.pdfMode = false;
            printRow.sheets = 0;

            cancelSignal.clear();
        }

        function test_clickCancel() {
            var button = findChild(printRow, "cancel");
            var buttonCenter = centerOf(button);

            mouseClick(button, buttonCenter.x, buttonCenter.y);
            cancelSignal.wait();
            compare(cancelSignal.count, 1)
        }

        function test_clickConfirm() {
            var button = findChild(printRow, "confirm");
            var buttonCenter = centerOf(button);

            mouseClick(button, buttonCenter.x, buttonCenter.y);
            confirmSignal.wait();
            compare(confirmSignal.count, 1)
        }

        function test_pdfMode() {
            var button = findChild(printRow, "confirm");
            var originalText = button.text;

            printRow.pdfMode = true;

            verify(button.text !== originalText, 1000, "Text did not change when entering PDF Mode");
        }

        function test_sheets() {
            var button = findChild(printRow, "confirm");
            var originalText = button.text;

            printRow.sheets = 10;

            verify(button.text !== originalText, 1000, "Text did not change when changing sheets");
        }
    }
}
