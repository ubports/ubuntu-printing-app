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
import "../../ubuntu-printing-app/components"

import UbuntuPrintingApp 1.0
import Ubuntu.Components.Extras.Printers 0.1

Item {
    id: root
    width: units.gu(100)
    height: units.gu(75)

    Document {
        id: testDocument
        url: Qt.resolvedUrl("../resources/pdf/a4_portrait.pdf")
    }

    QtObject {
        id: mockPrinterJob

        property int colorModelType: PrinterEnum.ColorType
    }

    Item {
        id: mockView
        height: units.gu(100)
        width: units.gu(100)
    }

    PreviewRow {
        id: previewRow
        document: testDocument
        printerJob: mockPrinterJob
        view: mockView
    }

    SignalSpy {
        id: documentCountSpy
        signalName: "onCountChanged"
        target: testDocument
    }

    SignalSpy {
        id: activityIndicatorSpy
        signalName: "onRunningChanged"
        // Target is set at runtime
    }

    UbuntuTestCase {
        name: "PreviewRowTestCase"
        when: windowShown

        function init() {
            testDocument.url = Qt.resolvedUrl("../resources/pdf/a4_portrait.pdf");

            mockPrinterJob.colorModelType = PrinterEnum.ColorType;

            activityIndicatorSpy.target = null;
            activityIndicatorSpy.clear();
            documentCountSpy.clear();

            waitForRendering(previewRow);
        }

        function test_activityIndicator() {
            var image = findChild(previewRow, "previewImage");
            var activityIndicator = findChild(previewRow, "activityIndicator");
            activityIndicatorSpy.target = activityIndicator;

            // Change the document and wait for image to finish loading
            testDocument.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");
            tryCompare(image, "status", Image.Ready);

            // Check that the running state changed twice false=>true=>falase
            compare(activityIndicatorSpy.count, 2);
        }

        function test_document_image() {
            var image = findChild(previewRow, "previewImage");

            // Change the document
            testDocument.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");

            // Ensure that the image has a valid state
            tryCompare(image, "status", Image.Ready);
        }

        function test_document_image_invalid() {
            var image = findChild(previewRow, "previewImage");

            // Change the document to an invalid doc
            testDocument.url = Qt.resolvedUrl("../resources/pdf/text.txt");

            // Ensure that the image has a valid state
            // This is because the poppler renderer always generates a valid image
            tryCompare(image, "status", Image.Ready);
        }

        function test_document_change_page_reset() {
            var next = findChild(previewRow, "nextButton");
            var pageHelper = previewRow.pageHelper;

            // Load a multi page document and change to 2nd page
            testDocument.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");

            documentCountSpy.wait();
            compare(documentCountSpy.count, 1);
            compare(testDocument.count, 3);

            mouseClick(next);
            tryCompare(pageHelper, "page", 1);

            // Load a single page document, check we go back to page 0
            testDocument.url = Qt.resolvedUrl("../resources/pdf/a4_portrait.pdf");

            documentCountSpy.wait();
            compare(documentCountSpy.count, 2);
            compare(testDocument.count, 1);
            compare(pageHelper.page, 0);
        }

        function test_overlay() {
            var label = findChild(previewRow, "overlayLabel");
            var pageHelper = previewRow.pageHelper;
            var textParts = label.text.split("/");

            // Check text starts with 1/1
            compare(textParts.length, 2);
            compare(textParts[0], "1");
            compare(textParts[1], "1");

            testDocument.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");

            waitForRendering(previewRow);
            textParts = label.text.split("/");

            // Check it has changed to 1/3
            compare(textParts.length, 2);
            compare(textParts[0], "1");
            compare(textParts[1], "3");

            pageHelper.page++;

            waitForRendering(previewRow);
            textParts = label.text.split("/");

            // Check it has changed to 2/3
            compare(textParts.length, 2);
            compare(textParts[0], "2");
            compare(textParts[1], "3");
        }

        function test_page_single() {
            var next = findChild(previewRow, "nextButton");
            var previous = findChild(previewRow, "previousButton");

            // Check with a single page doc navigation controls are disabled
            compare(next.enabled, false);
            compare(previous.enabled, false);
        }

        function test_page_next() {
            var image = findChild(previewRow, "previewImage");
            var next = findChild(previewRow, "nextButton");
            var pageHelper = previewRow.pageHelper;

            testDocument.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");

            documentCountSpy.wait();
            compare(documentCountSpy.count, 1);

            // Check with multi page doc, next button becomes enabled
            compare(testDocument.count, 3);
            compare(next.enabled, true);
            compare(pageHelper.page, 0);

            // Check the requested image page is 0
            // image://poppler/0/true/file:///path/to/file.pdf
            compare(image.source.toString().slice(0, 17), "image://poppler/0");

            // Click to the next page, check still enabled
            mouseClick(next);
            tryCompare(pageHelper, "page", 1);
            compare(next.enabled, true);

            // Check the requested image page is 1
            // image://poppler/1/true/file:///path/to/file.pdf
            compare(image.source.toString().slice(0, 17), "image://poppler/1");

            // Click to the last page, check next becomes diabled
            mouseClick(next);

            // Change the document
            testDocument.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");

            // Ensure that the image has a valid state
            tryCompare(image, "status", Image.Ready);
            tryCompare(pageHelper, "page", 2);
            compare(next.enabled, false);

            // Check the requested image page is 2
            // image://poppler/2/true/file:///path/to/file.pdf
            compare(image.source.toString().slice(0, 17), "image://poppler/2");
        }

        function test_page_previous() {
            var image = findChild(previewRow, "previewImage");
            var previous = findChild(previewRow, "previousButton");
            var pageHelper = previewRow.pageHelper;

            testDocument.url = Qt.resolvedUrl("../resources/pdf/mixed_portrait.pdf");

            documentCountSpy.wait();
            compare(documentCountSpy.count, 1);

            // Check with mulit page doc, previous is still disabled when on 1st
            compare(testDocument.count, 3);
            compare(previous.enabled, false);
            compare(pageHelper.page, 0)

            // Check the requested image page is 0
            // image://poppler/0/true/file:///path/to/file.pdf
            compare(image.source.toString().slice(0, 17), "image://poppler/0");

            // Check when changing to the last page, previous is enabled
            pageHelper.page = 2;
            tryCompare(pageHelper, "page", 2);
            compare(previous.enabled, true);

            // Check the requested image page is 2
            // image://poppler/2/true/file:///path/to/file.pdf
            compare(image.source.toString().slice(0, 17), "image://poppler/2");

            // Click to the previous page, check still enabled
            mouseClick(previous);
            tryCompare(pageHelper, "page", 1);
            compare(previous.enabled, true);

            // Check the requested image page is 1
            // image://poppler/1/true/file:///path/to/file.pdf
            compare(image.source.toString().slice(0, 17), "image://poppler/1");

            // Click to the first page, check previous becomes disabled
            mouseClick(previous);
            tryCompare(pageHelper, "page", 0);
            compare(previous.enabled, false);

            // Check the requested image page is 0
            // image://poppler/0/true/file:///path/to/file.pdf
            compare(image.source.toString().slice(0, 17), "image://poppler/0");
        }

        function test_printerJob_color() {
            var image = findChild(previewRow, "previewImage");
            mockPrinterJob.colorModelType = PrinterEnum.ColorType;

            waitForRendering(image);

            // Check the image requested is color
            // image://poppler/0/true/file:///path/to/file.pdf
            compare(image.source.toString().slice(0, 22), "image://poppler/0/true");
        }

        function test_printerJob_grayscale() {
            var image = findChild(previewRow, "previewImage");
            mockPrinterJob.colorModelType = PrinterEnum.GrayType;

            waitForRendering(image);

            // Check the image requested is grayscale
            // image://poppler/0/false/file:///path/to/file.pdf
            compare(image.source.toString().slice(0, 23), "image://poppler/0/false");
        }

        function test_view() {
            // test that the implicitHeight is set from the view height
            var pageHelper = previewRow.pageHelper;

            // min((WIDTH - 10) / ASPECT, HEIGHT / 1.5)

            // height/width of 100GU, aspect is 0.71
            // min((100 - 10) / 0.71, 100 / 1.5)
            // min(126.76, 66.66)
            fuzzyCompare(previewRow.implicitHeight, units.gu(66.66), units.gu(1));

            // Change the view to have a short width
            mockView.width = units.gu(50);
            waitForRendering(previewRow);

            // width 50GU, height 100GU, aspect is 0.71
            // min((50 - 10) / 0.71, 100 / 1.5)
            // min(56.33, 66.66)
            fuzzyCompare(previewRow.implicitHeight, units.gu(56.33), units.gu(1));
        }
    }
}
