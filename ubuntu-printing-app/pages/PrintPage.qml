/*
 * Copyright 2016, 2017 Canonical Ltd.
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
import Ubuntu.Components.Popups 1.3
import UbuntuPrintingApp 1.0
import Ubuntu.Components.Extras.Printers 0.1

import "../components"

Page {
    id: page
    anchors {
        fill: parent
    }
    header: PageHeader {
        id: pageHeader
        leadingActionBar {
            actions: [
                Action {
                    iconName: "back"
                    objectName: "headerBack"

                    onTriggered: page.cancel()
                }
            ]
        }
        title: printing.pdfMode ? i18n.tr("Page Setup") : i18n.tr("Printer Options")
    }

    property Document currentDocument: null
    property QtObject printing: null

    signal cancel()
    signal confirm(string url)

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
            width: page.width

            PreviewRow {
                document: currentDocument
                monitorMouseArea: globalMouseArea
                Layout.fillHeight: true
                printerJob: printing.printerJob
                view: scrollView
            }

            SelectorRow {
                id: printerSelector
                delegate: OptionSelectorDelegate {
                    objectName: "option" + index
                    text: name
                }
                model: printing.model
                objectName: "printerSelector"
                text: i18n.tr("Printer")

                Binding {
                    target: printing
                    property: "printerSelectedIndex"
                    value: printerSelector.selectedIndex
                }

                // Shows indicator at the end (right) of the row when loading
                ActivityIndicator {
                    objectName: "printerLoadingIndicator"
                    running: !printing.isLoaded
                    visible: running
                }
            }

            TextFieldRow {
                id: copiesSelector
                enabled: printing.isLoaded && !printing.pdfMode
                inputMethodHints: Qt.ImhDigitsOnly
                objectName: "copiesTextField"
                text: i18n.tr("Copies")
                validator: IntValidator {
                    bottom: 1
                    top: 999
                }
                value: printing.printerJob.copies

                onValueChanged: {
                    if (acceptableInput && printing.printerJob.copies !== Number(value)) {
                        printing.printerJob.copies = Number(value);
                    }
                }

                Binding {
                    target: copiesSelector
                    property: "value"
                    when: printing.printerJob && copiesSelector.enabled
                    value: printing.printerJob.copies
                }
            }

            SelectorRow {
                id: pageRangeSelector
                enabled: printing.isLoaded && !printing.pdfMode
                model: [i18n.tr("All"), i18n.tr("Range")]
                modelValue: [PrinterEnum.AllPages, PrinterEnum.PageRange]
                objectName: "pageRangeSelector"
                selectedIndex: 0
                text: i18n.tr("Pages")

                onSelectedValueChanged: {
                    if (printing.printerJob && printing.printerJob.printRangeMode !== selectedValue) {
                        printing.printerJob.printRangeMode = selectedValue
                    }
                }

                Binding {
                    target: pageRangeSelector
                    property: "selectedIndex"
                    when: printing.printerJob && pageRangeSelector.enabled
                    value: printing.printerJob.printRangeMode
                }
            }

            TextFieldRow {
                id: pageRangeTextField
                enabled: printing.isLoaded && !printing.pdfMode
                objectName: "pageRangeTextField"
                validator: RegExpValidator {
//                        regExp: ""  // TODO: validate to only 0-9||9-0||0 ,
                }
                visible: pageRangeSelector.selectedValue === PrinterEnum.PageRange

                onValueChanged: {
                    if (printing.printerJob.printRange !== value) {
                        printing.printerJob.printRange = value
                    }
                }

                Binding {
                    target: pageRangeTextField
                    property: "value"
                    when: printing.printerJob && pageRangeTextField.enabled
                    value: printing.printerJob.printRange
                }
            }

            LabelRow {
                enabled: printing.isLoaded && !printing.pdfMode
                objectName: "pageRangeLabel"
                secondaryText: i18n.tr("eg 1-3,8")
                visible: pageRangeSelector.selectedValue === PrinterEnum.PageRange
            }

            SelectorRow {
                id: duplexSelector
                enabled: printing.isEditable && currentDocument.count > 1 && printing.printer.supportedDuplexModes.length > 1
                model: printing.printer ? printing.printer.supportedDuplexModes : [""]
                objectName: "duplexSelector"
                text: i18n.tr("Two-sided")

                onSelectedIndexChanged: {
                    if (printing.printerJob.duplexMode !== selectedIndex) {
                        printing.printerJob.duplexMode = selectedIndex
                    }
                }

                Binding {
                    target: duplexSelector
                    property: "selectedIndex"
                    when: printing.printerJob && duplexSelector.enabled
                    value: printing.printerJob.duplexMode
                }
            }

            SelectorRow {
                id: colorModelSelector
                enabled: printing.isEditable && printing.printer.supportedColorModels.length > 1
                model: printing.printer ? printing.printer.supportedColorModels : [""]
                objectName: "colorModelSelector"
                text: i18n.tr("Color")

                onSelectedIndexChanged: {
                    if (printing.printerJob.colorModel !== selectedIndex) {
                        printing.printerJob.colorModel = selectedIndex
                    }
                }

                Binding {
                    target: colorModelSelector
                    property: "selectedIndex"
                    when: printing.printerJob && colorModelSelector.enabled
                    value: printing.printerJob.colorModel
                }
            }

            SelectorRow {
                id: qualitySelector
                enabled: printing.isEditable && printing.printer.supportedPrintQualities.length > 1
                model: printing.printer ? printing.printer.supportedPrintQualities : [""]
                objectName: "qualitySelector"
                text: i18n.tr("Quality")

                onSelectedIndexChanged: {
                    if (printing.printerJob.quality !== selectedIndex) {
                        printing.printerJob.quality = selectedIndex
                    }
                }

                Binding {
                    target: qualitySelector
                    property: "selectedIndex"
                    when: printing.printerJob && qualitySelector.enabled
                    value: qualitySelector.quality
                }
            }

            CheckBoxRow {
                id: checkboxSelector
                checkboxText: i18n.tr("Collate")
                enabled: printing.printerJob.copies > 1 && printing.isLoaded && !printing.pdfMode
                objectName: "collateCheckBox"

                onCheckedChanged: {
                    if (printing.printerJob.collate !== checked) {
                        printing.printerJob.collate = checked
                    }
                }

                Binding {
                    target: checkboxSelector
                    property: "checked"
                    when: printing.printerJob
                    value: printing.printerJob.collate
                }
            }

            CheckBoxRow {
                id: reverseSelector
                checkboxText: i18n.tr("Reverse")
                enabled: printing.isLoaded && !printing.pdfMode
                objectName: "reverseCheckBox"

                onCheckedChanged: {
                    if (printing.printerJob.reverse !== checked) {
                        printing.printerJob.reverse = checked
                    }
                }

                Binding {
                    target: reverseSelector
                    property: "checked"
                    when: printing.printerJob
                    value: printing.printerJob.reverse
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
        canPrint: printing.isLoaded
        objectName: "printRow"
        pdfMode: printing.pdfMode
        // TODO: This should count the range not all pages
        // roundUp((pageCount * copies) / duplex)
        sheets: {
            // If range is selected and a value exists, then set the sheets to zero
            // for now. Which results in not showing the number sheets
            if (printing.printerJob.printRangeMode === PrinterEnum.PageRange && printing.printerJob.printRange.length > 0) {
                0
            } else {
                Math.ceil((document.count * printing.printerJob.copies) / (printing.printerJob.isTwoSided ? 2 : 1))
            }
        }

        onCancel: page.cancel()
        onConfirm: page.confirm(document.url)
    }

    // ScrollView has a MouseArea which doesn't propagate hover events
    // so this is used to monitor for these events
    MouseArea {
        id: globalMouseArea
        anchors {
            fill: parent
        }
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
    }
}
