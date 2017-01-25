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
import "../../ubuntu-printing-app/pages"

import Ubuntu_Printing_App 1.0
import Ubuntu.Settings.Printers 0.1

Item {
    width: units.gu(100)
    height: units.gu(75)

    // Mock the PrintingHelper
    QtObject {
        id: mockPrinting

        property ListModel model: ListModel {

        }
        property bool pdfMode: false
        property var printer: QtObject {
            property string name: "PrinterA"
            property var supportedColorModels: []
            property var supportedDuplexModes: []
            property var supportedPrintQualities: []
        }
        property var printerJob: QtObject {
            property int colorModel: 0
            property int copies: 1
            property int duplexMode: 0
            property string printRange: ""
            property var printRangeMode: 0  // var as it needs to be enum
            property int quality: 0
        }
        property int printerSelectedIndex: 0
    }

    Document {
        id: document
        url: Qt.resolvedUrl("../resources/pdf/a4_portrait.pdf")
    }

    PrintPage {
        id: printPage
        anchors {
            fill: parent
        }
        currentDocument: document
        printing: mockPrinting
    }

    SignalSpy {
        id: cancelSpy
        signalName: "cancel"
        target: printPage
    }

    SignalSpy {
        id: confirmSpy
        signalName: "confirm"
        target: printPage
    }

    UbuntuTestCase {
        name: "PrintPageTestCase"

        when: windowShown

        property var dataColorModels: ["A", "B", "C"]
        property var dataDuplexModes: ["A", "B", "C"]
        property var dataPrinters: [
            {name: "PrinterA"},
            {name: "PrinterB"},
        ]
        property var dataPrintQualities: ["A", "B", "C"]
        readonly property int timeout: 1000

        function init() {
            document.url = Qt.resolvedUrl("../resources/pdf/a4_portrait.pdf");

            mockPrinting.model.clear();
            for (var i=0; i < dataPrinters.length; i++) {
                mockPrinting.model.append(dataPrinters[i]);
            }

            mockPrinting.pdfMode = false;

            mockPrinting.printer.supportedColorModels = dataColorModels;
            mockPrinting.printer.supportedDuplexModes = dataDuplexModes;
            mockPrinting.printer.supportedPrintQualities = dataPrintQualities;

            mockPrinting.printerJob.copies = 1;
            mockPrinting.printerJob.printRangeMode = PrinterEnum.AllPages;

            mockPrinting.printerSelectedIndex = 0;

            cancelSpy.clear();
            confirmSpy.clear();

            waitForRendering(printPage, timeout);
        }

        function test_cancelButton() {
            var cancel = findChild(printPage, "cancel");

            // Click the cancel button
            mouseClick(cancel);

            // Check that cancel signal was emitted
            cancelSpy.wait();
            compare(cancelSpy.count, 1);
        }

        function test_cancelHeader() {
            // Note SDK adds _button to objectNames in ActionBar
            var cancel = findChild(printPage, "headerBack_button");

            // Click cancel in the header
            mouseClick(cancel);

            // Check that cancel signal was emitted
            cancelSpy.wait();
            compare(cancelSpy.count, 1);
        }

        function test_colorModel() {
            var colorModel = findChild(printPage, "colorModelSelector");

            // Check color model length is correct
            compare(colorModel.model.length, dataColorModels.length);

            // Check each of the options has the correct text
            for (var i=0; i < dataColorModels.length; i++) {
                var option = findChild(colorModel, "option" + i);
                compare(option.text, dataColorModels[i]);
            }
        }

        function test_colorModelEmptyModels() {
            // Set the colorModel to empty
            var colorModel = findChild(printPage, "colorModelSelector");
            mockPrinting.printer.supportedColorModels = [];

            waitForRendering(colorModel, timeout)

            // Check that the selector becomes disabled
            compare(colorModel.enabled, false);
        }

        function test_colorModelSingleModel() {
            // Set the colorModel to a single entry
            var colorModel = findChild(printPage, "colorModelSelector");
            mockPrinting.printer.supportedColorModels = ["A"];

            waitForRendering(colorModel, timeout)

            // Check that the selector becomes disabled
            compare(colorModel.enabled, false);
        }

        function test_confirm() {
            var confirm = findChild(printPage, "confirm");

            // Click the confirm button
            mouseClick(confirm);

            // Check that the confirm signal was emitted
            confirmSpy.wait();
            compare(confirmSpy.count, 1);
            compare(confirmSpy.signalArguments.length, 1);

            // Check the arg in the signal was the document url
            var arg = confirmSpy.signalArguments.pop();
            compare(arg.length, 1);
            compare(arg[0], document.url);
        }

        function test_copies() {
            // Check copies starting value is correct
            var copies = findChild(printPage, "copiesTextField");
            compare(copies.value, "1");

            // Change the copies value and check the textField changes
            mockPrinting.printerJob.copies = 5;
            tryCompare(copies, "value", "5", timeout, "Copies value did not change");
        }

        function test_copiesKeyClick() {
            // Check copies starting value is correct
            var copies = findChild(printPage, "copiesTextField");
            compare(copies.value, "1");

            // Click on the textField
            mouseClick(copies);

            // Clear the current text and enter "2"
            keyClick(Qt.Key_Backspace);
            keyClick(Qt.Key_2);

            // Check that "2" is set to the backend
            tryCompare(copies, "value", "2", timeout, "Copies value did not change");
            compare(mockPrinting.printerJob.copies, 2);
        }

        function test_duplex() {
            var duplex = findChild(printPage, "duplexSelector");

            // Check the duplex model length is correct
            compare(duplex.model.length, dataDuplexModes.length);

            // Check each of the options has the correct text
            for (var i=0; i < dataDuplexModes.length; i++) {
                var option = findChild(duplex, "option" + i);
                compare(option.text, dataDuplexModes[i]);
            }
        }

        function test_duplexEmptyModes() {
            // Set the duplexModes to empty and with a multipage doc
            var duplex = findChild(printPage, "duplexSelector");
            document.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");
            mockPrinting.printer.supportedDuplexModes = [];

            // Check the selector becomes disabled
            waitForRendering(duplex, timeout)
            compare(duplex.enabled, false);
        }
        function test_duplexEnabledMultiPage() {
            var duplex = findChild(printPage, "duplexSelector");

            // Check that with single page doc duplex is disabled
            document.url = Qt.resolvedUrl("../resources/pdf/a4_portrait.pdf");
            compare(duplex.enabled, false);

            // Check with multi page doc, duplex becomes enabled
            document.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");
            tryCompare(duplex, "enabled", true, timeout,
                       "Duplex selected didn't become enabled when document changed");
        }
        function test_duplexSingleMode() {
            // Set the duplexModes to a single entry and with a multipage doc
            var duplex = findChild(printPage, "duplexSelector");
            document.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");
            mockPrinting.printer.supportedDuplexModes = ["A"];

            // Check the selector becomes disabled
            waitForRendering(duplex, timeout)
            compare(duplex.enabled, false);
        }

        function testPdfMode() {
            var objects = ["copiesTextField", "duplexSelector",
                           "pageRangeSelector", "pageRangeTextField",
                           "pageRangeLabel", "colorModelSelector",
                           "qualitySelector"];
            var pageTitle = printPage.title;

            // Enable pdf mode
            mockPrinting.pdfMode = true;
            waitForRendering(printPage, timeout);

            // Check that all the selectors become disabled
            for (var obj in objects) {
                compare(findChild(printPage, obj).enabled, false);
            }

            // Check that the page title changes
            verify(pageTitle !== printPage.title, "Page title did not change");
        }

        function test_printers() {
            var printers = findChild(printPage, "printerSelector");

            // Check that the printers model length is correct
            compare(printers.model.count, dataPrinters.length);

            // Check each of the options has the correct text
            for (var i=0; i < dataPrinters.length; i++) {
                var option = findChild(printers, "option" + i);
                compare(option.text, dataPrinters[i]["name"]);
            }
        }

        function test_printRangeMode() {
            var printRangeMode = findChild(printPage, "pageRangeSelector");
            var printRange = findChild(printPage, "pageRangeTextField");

            // Check that with AllPages, printRange is not visible
            compare(mockPrinting.printerJob.printRangeMode, PrinterEnum.AllPages);
            compare(printRange.visible, false);

            // Change to PageRange mode
            mockPrinting.printerJob.printRangeMode = PrinterEnum.PageRange;

            // Check that printRange becomes visible
            compare(mockPrinting.printerJob.printRangeMode, PrinterEnum.PageRange);
            tryCompare(printRange, "visible", true, timeout);
        }

        function test_quality() {
            var quality = findChild(printPage, "qualitySelector");

            // Check that the qualities model length is correct
            compare(quality.model.length, dataPrintQualities.length);

            // Check each of the options has the correct text
            for (var i=0; i < dataPrintQualities.length; i++) {
                var option = findChild(quality, "option" + i);
                compare(option.text, dataPrintQualities[i]);
            }
        }

        function test_qualityEmptyModels() {
            // Set the qualities model to empty
            var quality = findChild(printPage, "qualitySelector");
            mockPrinting.printer.supportedPrintQualities = [];

            // Check that the selector becomes disabled
            waitForRendering(quality, timeout)
            compare(quality.enabled, false);
        }

        function test_qualitySingleModel() {
            // Set the colorModel to a single entry
            var quality = findChild(printPage, "qualitySelector");
            mockPrinting.printer.supportedPrintQualities = ["A"];

            // Check that the selector becomes disabled
            waitForRendering(quality, timeout)
            compare(quality.enabled, false);
        }
    }
}
