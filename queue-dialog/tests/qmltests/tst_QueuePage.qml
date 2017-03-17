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
import "../../pages"

Item {
    width: units.gu(100)
    height: units.gu(75)

    QtObject {
        id: fakeQueueHelper

        property int count: 1

        property ListModel dataActiveFilter: ListModel {

        }
        property ListModel dataPausedFilter: ListModel {

        }
        property ListModel dataQueuedFilter: ListModel {

        }

        function createActiveFilter() {
            return dataActiveFilter;
        }

        function createPausedFilter() {
            return dataPausedFilter;
        }

        function createQueuedFilter() {
            return dataQueuedFilter;
        }

        signal cancelJob(string printerName, int jobId);
        signal holdJob(string printerName, int jobId);
        signal releaseJob(string printerName, int jobId);
    }

    QueuePage {
        id: queuePage
        anchors {
            fill: parent
        }

        queueHelper: fakeQueueHelper
    }

    SignalSpy {
        id: closeSpy
        signalName: "close"
        target: queuePage
    }

    SignalSpy {
        id: settingsSpy
        signalName: "settings"
        target: queuePage
    }

    UbuntuTestCase {
        name: "QueuePageTestCase"
        when: windowShown

        function init() {
            fakeQueueHelper.count = 1;
            fakeQueueHelper.dataActiveFilter.clear();
            fakeQueueHelper.dataPausedFilter.clear();
            fakeQueueHelper.dataQueuedFilter.clear();

            closeSpy.clear();
            settingsSpy.clear();

            waitForRendering(queuePage);
        }

        // Test that clicking the close in the header emits the signal
        function test_header_close() {
            // Note SDK adds _button to objectNames in ActionBar
            var closeButton = findChild(queuePage, "headerClose_button");
            mouseClick(closeButton);

            closeSpy.wait();
            compare(closeSpy.count, 1);
        }

        // Test that clicking the settings in the header emits the signal
        function test_header_settings() {
            // Note SDK adds _button to objectNames in ActionBar
            var settingsButton = findChild(queuePage, "headerSettings_button");
            mouseClick(settingsButton);

            settingsSpy.wait();
            compare(settingsSpy.count, 1);
        }

        // Test that the subtitle reacts to count changes
        function test_header_subtitle() {
            var expected;

            // Test jobs = 1 (singular)
            expected = i18n.tr("%1 job", "%1 jobs", 1).arg(1);
            compare(queuePage.header.subtitle, expected);

            // Test jobs = 2 (plural)
            fakeQueueHelper.count = 2;
            waitForRendering(queuePage);

            expected = i18n.tr("%1 job", "%1 jobs", 2).arg(2);
            compare(queuePage.header.subtitle, expected);
        }

        // Test that the filters model is correct
        function test_filters_model() {
            var filtersModel = findChild(queuePage, "queueView").filtersModel;

            // Test empty models
            compare(filtersModel.count, 3);

            compare(filtersModel.get(0).filter.count, 0);
            compare(filtersModel.get(0).state, "active");

            compare(filtersModel.get(1).filter.count, 0);
            compare(filtersModel.get(1).state, "queued");

            compare(filtersModel.get(2).filter.count, 0);
            compare(filtersModel.get(2).state, "paused");

            // Test an entry in each
            fakeQueueHelper.dataActiveFilter.append(
                {
                    id: 1,
                    impressionsCompleted: 1,
                    held: false,
                    printerName: "test-printer",
                    title: "test-active",
                }
            )
            fakeQueueHelper.dataQueuedFilter.append(
                {
                    id: 1,
                    impressionsCompleted: 1,
                    held: false,
                    printerName: "test-printer",
                    title: "test-queued",
                }
            )
            fakeQueueHelper.dataPausedFilter.append(
                {
                    id: 1,
                    impressionsCompleted: 1,
                    held: true,
                    printerName: "test-printer",
                    title: "test-paused",
                }
            )

            compare(filtersModel.count, 3);

            compare(filtersModel.get(0).filter.count, 1);
            compare(filtersModel.get(0).state, "active");

            compare(filtersModel.get(1).filter.count, 1);
            compare(filtersModel.get(1).state, "queued");

            compare(filtersModel.get(2).filter.count, 1);
            compare(filtersModel.get(2).state, "paused");
        }

        // Test that the empty state label appears when there are no jobs
        function test_no_jobs() {
            fakeQueueHelper.count = 0;
            waitForRendering(queuePage);

            var emptyLabel = findChild(queuePage, "emptyLabel");
            compare(emptyLabel.visible, true)
        }
    }
}
