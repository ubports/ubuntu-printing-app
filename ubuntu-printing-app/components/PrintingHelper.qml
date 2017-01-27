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
import UbuntuPrintingApp 1.0
import Ubuntu.Settings.Printers 0.1

QtObject {
    readonly property var model: Printers.allPrintersWithPdf
    readonly property bool pdfMode: printer ? printer.isPdf : true
    readonly property var printer: {
        if (Printers.allPrintersWithPdf.count > 0
                && 0 <= printerSelectedIndex
                && printerSelectedIndex < Printers.allPrintersWithPdf.count) {
            Printers.allPrintersWithPdf.get(printerSelectedIndex)
        } else {
            null
        }
    }
    readonly property PrinterJob printerJob: PrinterJob {
        printerName: printer ? printer.name : ""
    }
    property int printerSelectedIndex: -1
}
