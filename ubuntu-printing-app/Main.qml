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

    property PrintingHelper printing: PrintingHelper {
        printerJob {
            landscape: document.orientation === Document.Landscape
        }
    }
    property Page printPage: null

    PageStack {
        id: pageStack
        anchors {
            fill: parent
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

    Connections {
        target: ContentHub

        onImportRequested: {
            // FIXME: would this ever get more than one?
            for (var i=0; i < transfer.items.length; i++) {
                var item = transfer.items[i];
                document.url = item.url;
            }
        }
    }

    Connections {
        target: printPage

        onCancel: Qt.quit()
        onConfirm: {
            if (printing.pdfMode) {
                // TODO: check if .toLocalFilepath() needs to be called?
                pageStack.push(Qt.resolvedUrl("pages/ContentPeerPickerPage.qml"), {"url": url});
            } else {
                printing.printerJob.printFile(url);  // TODO: check document is valid raise error if not?
            }
        }
    }

    Component.onCompleted: {
        printPage = pageStack.push(
            Qt.resolvedUrl("pages/PrintPage.qml"),
            {
                "currentDocument": document,
                "printing": printing,
            }
        );

        if (args.values.url) {
            document.url = Qt.resolvedUrl(args.values.url);
        }
    }
}


