import QtQuick 2.4
import QtQuick.Layouts 1.1

import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Content 0.1
import Destination_App 1.0

import "components"

/*!
    \brief MainView with a Label and Button elements.
*/

MainView {
    id: mainView
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the "name" field of the click manifest
    applicationName: "destination-app.ahayzen"

    width: units.gu(40)
    height: units.gu(60)

    Connections {
        target: ContentHub

        onImportRequested: {
            console.debug("State:", transfer.state)
            console.debug("Transfer:", transfer)
            console.debug("Items:", transfer.items)

            for (var i=0; i < transfer.items.length; i++) {
                var item = transfer.items[i];

                document.url = item.url;
                console.debug("Item URL:", item.url);
            }
        }
    }

    Printer {
        id: printer
        name: PrinterInfo.defaultPrinterName
    }

    Document {
        id: document


//        onUrlChanged: {
//            var image = renderImage(printer.size(), 0);

//            console.debug("Image:", image);

//            image.parent = container;
//        }
    }

    /*
    PrinterInfo.availablePrinterNames

    PrinterInfo.defaultName

    Printer {
      error: false
      errorString: ""
      name: PrinterInfo.defaultName

      colorMode: ColorModes.greyscale
      orientation: Paper.portrait
      paperSize: Paper.A4

      print(Document document)
    }

    Document {
        error: false
        errorString: ""
        url: "/tmp/my.pdf"

        renderPage(QPainter, Int)
    }

    PreviewImage {
        document: doc
        page: 2
        printer: printer
    }

    PreviewImage {
        document:
    }

    */

    Page {
        id: page
        anchors {
            fill: parent
        }
        width: mainView.width

        header: PageHeader {
            id: pageHeader
            leadingActionBar {
                actions: [
                    Action {
                        iconName: "back"

                        onTriggered: Qt.quit()
                    }
                ]
            }

            title: i18n.tr("Printer Options")
        }

        ScrollView {
            anchors {
                bottom: printRow.top
                left: parent.left
                right: parent.right
                top: page.header.bottom
            }

            Item {
                height: columnLayout.height + units.gu(2)
                width: mainView.width - units.gu(2)
                x: units.gu(1)
                y: x

                ColumnLayout {
                    id: columnLayout
                    spacing: units.gu(1)
                    width: parent.width

                    onWidthChanged: console.debug("ColW", width / units.gu(1))

//                    Image {
//                        id: previewImage
//                        asynchronous: true
//                        height: units.gu(25)
//                        width: mainView.width - units.gu(5)
//                        source: document.url.toString() !== "" ? "image://poppler/" + document.url : ""  //.toString().substring(8)
//                        sourceSize {
//                            height: units.gu(25)
//                            width: mainView.width - units.gu(5)
//                        }

//                        onStatusChanged: console.debug("Status", status)
//                    }

                    Rectangle {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        color: "#DDD"
                        height: units.gu(22)

                        PreviewImage {
                            anchors {
                                centerIn: parent
                            }
                            document: document
                            implicitHeight: units.gu(20)
                            printer: printer
                            implicitWidth: parent.width - units.gu(2)
                        }
                    }

                    SelectorRow {
                        model: PrinterInfo.availablePrinterNames
                        selectedIndex: model.indexOf(printer.name)
                        text: i18n.tr("Printer")

                        onSelectedIndexChanged: printer.name = model[selectedIndex]
                    }

                    /*
                    ListItem {
                        id: listItem

                        Label {
                            text: "a"  //PrinterInfo.defaultPrinterName
                        }

    //                        Label {
    //                            text: "a, b, c"
    //                            visible: listItem.expansion.expanded
    //                        }

                        //expansion.height: units.gu(15)

                        onClicked: expansion.expanded = true
                    }
                    */

                    RowLayout {
                        Label {
                            Layout.preferredWidth: units.gu(10)
                            text: i18n.tr("Copies")
                        }

                        TextField {
                            inputMethodHints: Qt.ImhDigitsOnly
                            Layout.fillWidth: true
                            Layout.preferredWidth: units.gu(5)
                            text: "1"
                            validator: IntValidator {
                                bottom: 1
                                top: 999
                            }

                            onAcceptableInputChanged: console.debug("AcceptableInput:", acceptableInput)
                        }
                    }

                    SelectorRow {
                        model: ["A4", "A5"]
                        selectedIndex: model.indexOf(printer.paperSize)
                        text: i18n.tr("Paper Size")

                        onSelectedIndexChanged: printer.paperSize = model[selectedIndex]
                    }

                    SelectorRow {
                        model: ["Landscape", "Portrait"]
                        selectedIndex: printer.orientation === Qt.Horizontal ? 0 : 1
                        text: i18n.tr("Orientation")

                        onSelectedIndexChanged: {
                            if (selectedIndex == 0) {
                                printer.orientation = Qt.Horizontal;
                            } else {
                                printer.orientation = Qt.Vertical
                            }
                        }
                    }

    //                Label {
    //                    id: label
    //                    objectName: "label"
    //                    anchors {
    //                        horizontalCenter: parent.horizontalCenter
    //                    }
    //                    text: "URL to print: " + PrinterInfo.availablePrinterNames + PrinterInfo.defaultPrinterName
    //                }
                }
            }
        }

        PrintRow {
            id: printRow
            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: units.gu(1)
                right: parent.right
                rightMargin: units.gu(1)
            }
            sheets: document.count

            onCancel: Qt.quit()
            onConfirm: printer.print(document);  // TODO: check document is valid raise error if not?
        }
    }

    Component.onCompleted: {
//        document.url = Qt.resolvedUrl("/home/andy/Workspace/Work/Canonical/dump/2016-11-17T12:00:08");
//        document.url = Qt.resolvedUrl("/home/andy/Downloads/UbuntuPhone.pdf");
        console.debug("Printers:", PrinterInfo.availablePrinterNames);
    }
}


