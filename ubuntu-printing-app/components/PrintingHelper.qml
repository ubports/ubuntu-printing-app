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
import QtQml 2.2
import UbuntuPrintingApp 1.0
import Ubuntu.Components.Extras.Printers 0.1

Item {
    readonly property bool isEditable: isLoaded && !pdfMode && !printer.isRaw
    readonly property bool isLoaded: printer && printer.isLoaded
    readonly property alias model: instantiator.model
    readonly property bool pdfMode: isLoaded && printer.isPdf

    // This is the current printer and is preloaded
    readonly property var printer: instantiator.printer && instantiator.printer.isLoaded ? instantiator.printer : null

    property var printerJob: Printers.createJob("")
    property int printerSelectedIndex: -1

    Instantiator {
        id: instantiator
        model: Printers.allPrintersWithPdf
        delegate: Item {
            readonly property var printer: model
        }

        // Extract the printer from the Instantiator so we can use it
        // this printer may not be loaded yet
        readonly property var printer: {
            if (model.count > 0
                    && 0 <= printerSelectedIndex
                    && printerSelectedIndex < model.count) {
                instantiator.objectAt(printerSelectedIndex).printer
            } else {
                null
            }
        }

        // The information about the Printer is lazy loaded, this requests
        // the loading of a Printer's information
        function loadPrinter() {
            if (printer) {
                Printers.loadPrinter(printer.name)
            }
        }

        onPrinterChanged: loadPrinter()

        Component.onCompleted: loadPrinter()
    }

    Binding {
        property: "printer"
        target: printerJob
        when: printer && isLoaded
        value: printer ? printer.printer : null
    }
}
