import QtQuick 2.4
import QtTest 1.1
import Ubuntu.Test 1.0
import "../../ubuntu-printing-app/pages"

import Ubuntu_Printing_App 1.0
import Ubuntu.Settings.Printers 0.1

Item {
    width: units.gu(100)
    height: units.gu(75)

    QtObject {
        id: mockPrinting

        property ListModel model: ListModel {

        }
        property bool pdfMode: false
        property var printer: QtObject {
            property var name: "PrinterA"
            property var supportedColorModels: ["A", "B", "C"]
            property var supportedDuplexModes: ["A", "B", "C"]
            property var supportedPrintQualities: ["A", "B", "C"]
        }
        property var printerJob: QtObject {
            property int colorModel: 0
            property int copies: 1
            property int duplexMode: 0
            property string printRange: ""
            property var printRangeMode: 0  // is enum so need to be var
            property int quality: 0

            signal printFile(string url);
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

    UbuntuTestCase {
        name: "PrintPageTestCase"

        when: windowShown

        property var dataPrinters: [
            {name: "PrinterA"},
            {name: "PrinterB"},
        ]

        function init() {
            mockPrinting.model.clear();
            for (var i=0; i < dataPrinters.length; i++) {
                mockPrinting.model.append(dataPrinters[i]);
            }

            mockPrinting.pdfMode = false;
            mockPrinting.printerSelectedIndex = 0;

            waitForRendering(printPage, 1000);
        }

        function test_printers() {
            var printers = findChild(printPage, "printerSelector");

            compare(printers.model.count, dataPrinters.length);

            for (var i=0; i < dataPrinters.length; i++) {
                var option = findChild(printers, "option" + i);
                compare(option.text, dataPrinters[i]["name"]);
            }
        }
    }
}
