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
import Ubuntu.Components.Popups 1.3
import Ubuntu.Content 1.1
import Ubuntu_Printing_App 1.0
import Ubuntu.Settings.Printers 0.1

import "components"

/*!
    \brief MainView with a Label and Button elements.
*/

MainView {
    id: mainView
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the "name" field of the click manifest
    applicationName: "ubuntu-printing-app"

    width: units.gu(45)
    height: units.gu(70)

    Connections {  // split into ContentHubHelper which has onDocumentUrlChanged, exportDocument(url)
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

    Component {  // cannot split due to issue
        id: dialogComponent
        Dialog {
            id: dialog

            Button {
                text: i18n.tr("OK")

                onClicked: PopupUtils.close(dialog)
            }
        }
    }

    Document {
        id: document

        onError: {
            var errorString;

            switch (errorType) {
            case Document.ErrorDocumentInvalid:
                errorString = i18n.tr("Document is invalid");
                break;
            case Document.ErrorNotFound:
                errorString = i18n.tr("Document not found");
                break;
            case Document.ErrorNotPdf:
                errorString = i18n.tr("Not a PDF document");
                break;
            default:
                errorString = i18n.tr("An unknown error occurred");
                break;
            }

            PopupUtils.open(
                dialogComponent,  // Qt.resolvedUrl("components/AlertDialog.qml"),  // FIXME: doesn't work when document is set onCompleted of MainView
                undefined,
                {
                    "text": errorString,
                    "title": i18n.tr("Error"),
                }
            )
        }
    }

    PageStack {
        id: pageStack
        anchors {
            fill: parent
        }
    }

    property bool pdfMode: printer ? printer.isPdf : true
    property var printer: {
        if (Printers.allPrintersWithPdf.count > 0
                && 0 <= printerSelectedIndex
                && printerSelectedIndex < Printers.allPrintersWithPdf.count) {
            Printers.allPrintersWithPdf.get(printerSelectedIndex)
        } else {
            null
        }
    }
    property int printerSelectedIndex: -1

    PrinterJob {
        id: printerJob
        printerName: printer ? printer.name : ""
        landscape: document.orientation === Document.Landscape
    }

    Page {
        id: page
        anchors {
            fill: parent
        }
        width: mainView.width
        visible: false

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

            title: pdfMode ? i18n.tr("Page Setup") : i18n.tr("Printer Options")
        }

        ScrollView {
            id: scrollView
            anchors {
                bottom: printRow.top
                left: parent.left
                right: parent.right
                top: page.header.bottom
            }

            ColumnLayout {
                id: columnLayout
                spacing: units.gu(1)
                width: mainView.width

                PreviewRow {
                    document: document
                    Layout.fillHeight: true
                    printerJob: printerJob
                    view: scrollView
                }

                SelectorRow {
                    id: printerSelector
                    delegate: OptionSelectorDelegate {
                        text: name
                    }
                    model: Printers.allPrintersWithPdf
                    text: i18n.tr("Printer")

                    onSelectedIndexChanged: printerSelectedIndex = selectedIndex

                    Component.onCompleted: printerSelectedIndex = selectedIndex
                 }

                TextFieldRow {
                    enabled: !pdfMode
                    inputMethodHints: Qt.ImhDigitsOnly
                    text: i18n.tr("Copies")
                    validator: IntValidator {
                        bottom: 1
                        top: 999
                    }
                    value: printerJob.copies

                    onValueChanged: {
                        if (acceptableInput) {
                            printerJob.copies = Number(value);
                        }
                    }
                }

//                CheckBoxRow {
////                    checked: printer.collate
//                    checkboxText: i18n.tr("Collate")
//                    enabled: printerJob.copies > 1 //&& !printer.pdfMode

////                    onCheckedChanged: printer.collate = checked
//                }

                SelectorRow {
                    id: duplexSelector
                    enabled: printer && !pdfMode ? printer.supportedDuplexModes.length > 1 : false
                    model: printer ? printer.supportedDuplexModes : [""]
                    text: i18n.tr("Two-sided")

                    onSelectedIndexChanged: {
                        if (printerJob.duplexMode !== selectedIndex) {
                            printerJob.duplexMode = selectedIndex
                        }
                    }

                    Binding {
                        target: duplexSelector
                        property: "selectedIndex"
                        when: printerJob && duplexSelector.enabled
                        value: printerJob.duplexMode
                    }
                }

                SelectorRow {
                    id: pageRangeSelector
                    enabled: !pdfMode
                    model: [i18n.tr("All"), i18n.tr("Range")]
                    modelValue: [PrinterEnum.AllPages, PrinterEnum.PageRange]
                    selectedIndex: 0
                    text: i18n.tr("Pages")

                    onSelectedValueChanged: printerJob.printRangeMode = selectedValue
                }

                TextFieldRow {
                    enabled: !pdfMode
                    validator: RegExpValidator {
//                        regExp: ""  // TODO: validate to only 0-9||9-0||0 ,
                    }
                    visible: pageRangeSelector.selectedValue === PrinterEnum.PageRange

                    onValueChanged: printerJob.printRange = value
                }

                LabelRow {
                    enabled: !pdfMode
                    secondaryText: i18n.tr("eg 1-3,8")
                    visible: pageRangeSelector.selectedValue === PrinterEnum.PageRange
                }

//                SelectorRow {
//                    enabled: !printer.pdfMode
//                    model: [1, 2, 4, 6, 9]
//                    selectedIndex: 0
//                    text: i18n.tr("Pages per side")
//                }

                SelectorRow {
                    id: colorModelSelector
                    enabled: printer && !pdfMode ? printer.supportedColorModels.length > 1 : false
                    model: printer ? printer.supportedColorModels : [""]
                    text: i18n.tr("Color")

                    onSelectedIndexChanged: {
                        if (printerJob.colorModel !== selectedIndex) {
                            printerJob.colorModel = selectedIndex
                        }
                    }

                    Binding {
                        target: colorModelSelector
                        property: "selectedIndex"
                        when: printerJob && colorModelSelector.enabled
                        value: printerJob.colorModel
                    }
                }

                SelectorRow {
                    id: qualitySelector
                    enabled: !pdfMode
                    model: [i18n.tr("Draft"), i18n.tr("Normal"), i18n.tr("Best"), i18n.tr("Photo")]
                    modelValue: [PrinterEnum.DraftQuality, PrinterEnum.NormalQuality, PrinterEnum.BestQuality, PrinterEnum.PhotoQuality]
                    text: i18n.tr("Quality")

                    Binding {
                        target: qualitySelector
                        property: "selectedIndex"
                        when: printerJob && qualitySelector.enabled
                        value: qualitySelector.modelValue.indexOf(printerJob.quality)
                    }
                }

                Item {
                    height: units.gu(2)
                    width: parent.width
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
            pdfMode: mainView.pdfMode
            sheets: document.count

            onCancel: Qt.quit()
            onConfirm: {
                if (pdfMode) {
                    // TODO: check if .toLocalFilepath() needs to be called?
                    pageStack.push(Qt.resolvedUrl("components/ContentPeerPickerDialog.qml"), {"url": document.url});
                } else {
                    printerJob.printFile(document.url);  // TODO: check document is valid raise error if not?
                }
            }
        }
    }

    Arguments {
        id: args
        Argument {
            name: "url"
            help: i18n.tr("Url of PDF to print")
            required: false
            valueNames: ["url"]
        }
    }

    Component.onCompleted: {
        pageStack.push(page)

        if (args.values.url) {
            document.url = Qt.resolvedUrl(args.values.url);
        }
    }
}


