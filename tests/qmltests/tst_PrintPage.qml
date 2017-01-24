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

            mouseClick(cancel);

            cancelSpy.wait();
            compare(cancelSpy.count, 1);
        }

        function test_cancelHeader() {
            // Note SDK adds _button to objectNames in ActionBar
            var cancel = findChild(root, "headerBack_button");

            mouseClick(cancel);

            cancelSpy.wait();
            compare(cancelSpy.count, 1);
        }

        function test_colorModel() {
            var colorModel = findChild(printPage, "colorModelSelector");

            compare(colorModel.model.length, dataColorModels.length);

            for (var i=0; i < dataColorModels.length; i++) {
                var option = findChild(colorModel, "option" + i);
                compare(option.text, dataColorModels[i]);
            }
        }

        function test_colorModelEmptyModels() {
            var colorModel = findChild(printPage, "colorModelSelector");
            mockPrinting.printer.supportedColorModels = [];

            waitForRendering(colorModel, timeout)
            compare(colorModel.enabled, false);
        }

        function test_colorModelSingleModel() {
            var colorModel = findChild(printPage, "colorModelSelector");
            mockPrinting.printer.supportedColorModels = ["A"];

            waitForRendering(colorModel, timeout)
            compare(colorModel.enabled, false);
        }

        function test_confirm() {
            var confirm = findChild(printPage, "confirm");

            mouseClick(confirm);

            confirmSpy.wait();
            compare(confirmSpy.count, 1);
            compare(confirmSpy.signalArguments.length, 1);

            var arg = confirmSpy.signalArguments.pop();
            compare(arg.length, 1);
            compare(arg[0], document.url);
        }

        function test_copies() {
            var copies = findChild(printPage, "copiesTextField");
            compare(copies.value, "1");

            mockPrinting.printerJob.copies = 5;
            tryCompare(copies, "value", "5", timeout, "Copies value did not change");
        }

        function test_copiesKeyClick() {
            var copies = findChild(printPage, "copiesTextField");
            compare(copies.value, "1");

            mouseClick(copies);

            keyClick(Qt.Key_Backspace);
            keyClick(Qt.Key_2);

            tryCompare(copies, "value", "2", timeout, "Copies value did not change");
            compare(mockPrinting.printerJob.copies, 2);
        }

        function test_duplex() {
            var duplex = findChild(printPage, "duplexSelector");

            compare(duplex.model.length, dataDuplexModes.length);

            for (var i=0; i < dataDuplexModes.length; i++) {
                var option = findChild(duplex, "option" + i);
                compare(option.text, dataDuplexModes[i]);
            }
        }

        function test_duplexEmptyModes() {
            var duplex = findChild(printPage, "duplexSelector");

            mockPrinting.printer.supportedDuplexModes = [];

            waitForRendering(duplex, timeout)
            compare(duplex.enabled, false);
        }
        function test_duplexEnabledMultiPage() {
            var duplex = findChild(printPage, "duplexSelector");

            document.url = Qt.resolvedUrl("../resources/pdf/a4_portrait.pdf");
            compare(duplex.enabled, false);

            document.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");
            tryCompare(duplex, "enabled", true, timeout,
                       "Duplex selected didn't become enabled when document changed");
        }
        function test_duplexSingleMode() {
            var duplex = findChild(printPage, "duplexSelector");
            document.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");

            mockPrinting.printer.supportedDuplexModes = ["A"];

            waitForRendering(duplex, timeout)
            compare(duplex.enabled, false);
        }

        function testPdfMode() {
            var objects = ["copiesTextField", "duplexSelector",
                           "pageRangeSelector", "pageRangeTextField",
                           "pageRangeLabel", "colorModelSelector",
                           "qualitySelector"];
            var pageTitle = printPage.title;

            mockPrinting.pdfMode = true;
            waitForRendering(printPage, timeout);

            for (var obj in objects) {
                compare(findChild(printPage, obj).enabled, false);
            }

            verify(pageTitle !== printPage.title, "Page title did not change");
        }

        function test_printers() {
            var printers = findChild(printPage, "printerSelector");

            compare(printers.model.count, dataPrinters.length);

            for (var i=0; i < dataPrinters.length; i++) {
                var option = findChild(printers, "option" + i);
                compare(option.text, dataPrinters[i]["name"]);
            }
        }

        function test_printRangeMode() {
            var printRangeMode = findChild(printPage, "pageRangeSelector");
            var printRange = findChild(printPage, "pageRangeTextField");

            compare(mockPrinting.printerJob.printRangeMode, PrinterEnum.AllPages);
            compare(printRange.visible, false);

            mockPrinting.printerJob.printRangeMode = PrinterEnum.PageRange;

            compare(mockPrinting.printerJob.printRangeMode, PrinterEnum.PageRange);
            tryCompare(printRange, "visible", true, timeout);
        }

        function test_quality() {
            var quality = findChild(printPage, "qualitySelector");

            compare(quality.model.length, dataPrintQualities.length);

            for (var i=0; i < dataPrintQualities.length; i++) {
                var option = findChild(quality, "option" + i);
                compare(option.text, dataPrintQualities[i]);
            }
        }

        function test_qualityEmptyModels() {
            var quality = findChild(printPage, "qualitySelector");
            mockPrinting.printer.supportedPrintQualities = [];

            waitForRendering(quality, timeout)
            compare(quality.enabled, false);
        }

        function test_qualitySingleModel() {
            var quality = findChild(printPage, "qualitySelector");
            mockPrinting.printer.supportedPrintQualities = ["A"];

            waitForRendering(quality, timeout)
            compare(quality.enabled, false);
        }
    }
}
