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
import QtQuick 2.12
import QtTest 1.1
import Ubuntu.Test 1.0
import "../../components"

Item {
    width: units.gu(100)
    height: units.gu(75)


    QueueDelegate {
        id: queueDelegate
        isActive: false
        width: parent.width

        property var model: QtObject {
            property bool held: false
            property int impressionsCompleted: 1
            property string printerName: "test-printer"
            property string title: "test-title"
        }
    }

    SignalSpy {
        id: cancelSpy
        signalName: "cancel"
        target: queueDelegate
    }

    SignalSpy {
        id: holdSpy
        signalName: "hold"
        target: queueDelegate
    }

    SignalSpy {
        id: releaseSpy
        signalName: "release"
        target: queueDelegate
    }

    UbuntuTestCase {
        name: "QueueDelegateTestCase"
        when: windowShown

        function init() {
            queueDelegate.isActive = false;
            queueDelegate.model.held = false;
            queueDelegate.model.impressionsCompleted = 1;
            queueDelegate.model.printerName = "test-printer";
            queueDelegate.model.title = "test-title";

            cancelSpy.clear();
            holdSpy.clear();
            releaseSpy.clear();

            waitForRendering(queueDelegate);
        }

        // Check that clicking cancel causes the cancel signal
        function test_cancel() {
            var cancelButton = findChild(queueDelegate, "cancel_button");
            mouseClick(cancelButton);

            cancelSpy.wait();
            compare(cancelSpy.count, 1);
        }

        // Check clicking toggle when held=false causes the hold signal
        function test_hold() {
            var pauseButton = findChild(queueDelegate, "toggle_button");
            mouseClick(pauseButton);

            holdSpy.wait();
            compare(holdSpy.count, 1);
        }

        // Check that when held=true the toggle icon is a play icon
        function test_pause_held() {
            queueDelegate.model.held = true;
            waitForRendering(queueDelegate);

            var pauseButton = findChild(queueDelegate, "toggle_button");
            compare(pauseButton.iconName, "media-playback-start");
            compare(pauseButton.text, i18n.tr("Release"));
        }

        // Check that when held=false the toggle icon is a pause icon
        function test_pause_released() {
            var pauseButton = findChild(queueDelegate, "toggle_button");
            compare(pauseButton.iconName, "media-playback-pause");
            compare(pauseButton.text, i18n.tr("Hold"));
        }

        // Check clicking toggle when held=true causes the release signal
        function test_release() {
            queueDelegate.model.held = true;
            waitForRendering(queueDelegate);

            var pauseButton = findChild(queueDelegate, "toggle_button");
            mouseClick(pauseButton);

            releaseSpy.wait();
            compare(releaseSpy.count, 1);
        }

        // Check when isActive=true the subtitle is showing that pages are being printed
        function test_subtitle_active() {
            queueDelegate.isActive = true;
            waitForRendering(queueDelegate);

            var expected;
            var subtitleLabel = findChild(queueDelegate, "subtitleLabel");

            // Test impressionsCompleted = 0 (doesn't show pages)
            queueDelegate.model.impressionsCompleted = 0;
            waitForRendering(queueDelegate);

            expected = i18n.tr("Printing")
                + "\n" + queueDelegate.model.printerName;

            compare(subtitleLabel.text, expected);

            // Test impressionsCompleted = 1 (singular)
            queueDelegate.model.impressionsCompleted = 1;
            waitForRendering(queueDelegate);

            expected = i18n.tr("Printing %1 page", "Printing %1 pages", 1).arg(1)
                + "\n" + queueDelegate.model.printerName;

            compare(subtitleLabel.text, expected);

            // Test impressionsCompleted = 2 (plural)
            queueDelegate.model.impressionsCompleted = 2;
            waitForRendering(queueDelegate);

            expected = i18n.tr("Printing %1 page", "Printing %1 pages", 2).arg(2)
                + "\n" + queueDelegate.model.printerName;

            compare(subtitleLabel.text, expected);
        }

        // Check when isActive=false the subtitle is showing that pages are ready to print
        function test_subtitle_inactive() {
            var expected;
            var subtitleLabel = findChild(queueDelegate, "subtitleLabel");

            // Test impressionsCompleted = 0 (doesn't show pages)
            queueDelegate.model.impressionsCompleted = 0;
            waitForRendering(queueDelegate);

            expected = i18n.tr("Ready to print")
                + "\n" + queueDelegate.model.printerName;

            compare(subtitleLabel.text, expected);

            // Test impressionsCompleted = 1 (singular)
            queueDelegate.model.impressionsCompleted = 1;
            waitForRendering(queueDelegate);

            expected = i18n.tr("Ready to print %1 page", "Ready to print %1 pages", 1).arg(1)
                + "\n" + queueDelegate.model.printerName;

            compare(subtitleLabel.text, expected);

            // Test impressionsCompleted = 2 (plural)
            queueDelegate.model.impressionsCompleted = 2;
            waitForRendering(queueDelegate);

            expected = i18n.tr("Ready to print %1 page", "Ready to print %1 pages", 2).arg(2)
                + "\n" + queueDelegate.model.printerName;

            compare(subtitleLabel.text, expected);
        }

        // Check that the title matches the model
        function test_title() {
            var titleLabel = findChild(queueDelegate, "titleLabel");
            compare(titleLabel.text, queueDelegate.model.title);

            queueDelegate.model.title = "another-title";
            waitForRendering(queueDelegate);

            compare(titleLabel.text, queueDelegate.model.title);
        }
    }
}
