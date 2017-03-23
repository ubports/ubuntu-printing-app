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
import "../../components"

Item {
    width: units.gu(100)
    height: units.gu(75)

    // Repeated test data is defined here
    QtObject {
        id: testData

        property var jobFilterData: [
            {
                objectName: "jobFilter0",
                filter: fakeQueueHelper.dataActiveFilter,
                isActive: true,
                visible: true,
                obj: {
                    id: 1,
                    impressionsCompleted: 3,
                    held: false,
                    printerName: "test-printer",
                    title: "test-active",
                },
            },
            {
                objectName: "jobFilter1",
                filter: fakeQueueHelper.dataQueuedFilter,
                isActive: false,
                visible: true,
                obj: {
                    id: 2,
                    impressionsCompleted: 2,
                    held: false,
                    printerName: "test-printer",
                    title: "test-queued",
                },
            },
            {
                objectName: "jobFilter2",
                filter: fakeQueueHelper.dataPausedFilter,
                isActive: false,
                visible: true,
                obj: {
                    id: 3,
                    impressionsCompleted: 1,
                    held: true,
                    printerName: "test-printer",
                    title: "test-paused",
                },
            }
        ]

        property var jobDelegateSignalData: [
            {
                filter: fakeQueueHelper.dataActiveFilter,
                obj: {
                    id: 1,
                    impressionsCompleted: 1,
                    held: false,
                    printerName: "test-printer-a",
                    title: "title-a",
                }
            },
            {
                filter: fakeQueueHelper.dataActiveFilter,
                obj: {
                    id: 2,
                    impressionsCompleted: 1,
                    held: false,
                    printerName: "test-printer-b",
                    title: "title-b",
                }
            }
        ]
    }

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

    SignalSpy {
        id: cancelSpy
        signalName: "cancelJob"
        target: fakeQueueHelper
    }

    SignalSpy {
        id: holdSpy
        signalName: "holdJob"
        target: fakeQueueHelper
    }

    SignalSpy {
        id: releaseSpy
        signalName: "releaseJob"
        target: fakeQueueHelper
    }

    ListModel {
        id: fakeFiltersModel
    }

    QueueView {
        id: queueView
        anchors {
            fill: parent
        }
        filtersModel: fakeFiltersModel
        queueHelper: fakeQueueHelper

        function addColumns() {
            filtersModel.clear();

            filtersModel.append(
                {
                    filter: queueHelper.createActiveFilter(),
                    name: "Active",
                    state: "active",
                }
            );

            filtersModel.append(
                {
                    filter: queueHelper.createQueuedFilter(),
                    name: "Queued",
                    state: "queued",
                }
            );

            filtersModel.append(
                {
                    filter: queueHelper.createPausedFilter(),
                    name: "Paused",
                    state: "paused",
                }
            );
        }
    }

    UbuntuTestCase {
        name: "QueuePageTestCase"
        when: windowShown

        function init() {
            fakeQueueHelper.count = 1;
            fakeQueueHelper.dataActiveFilter.clear();
            fakeQueueHelper.dataPausedFilter.clear();
            fakeQueueHelper.dataQueuedFilter.clear();

            cancelSpy.clear();
            holdSpy.clear();
            releaseSpy.clear();

            queueView.addColumns();

            waitForRendering(queueView);
        }

        // Check that the filter column is invisible when there are no jobs
        function test_filter_column_invisible_data() {
            return [
                {objectName: "filter0", visible: false},
                {objectName: "filter1", visible: false},
                {objectName: "filter2", visible: false},
            ]
        }

        function test_filter_column_invisible(data) {
            var column = findChild(queueView, data.objectName);
            compare(column.visible, data.visible);
        }

        // Check that the filter column becomes visible as jobs appear
        function test_filter_column_visible_data() {
            return [
                {
                    objectName: "filter0",
                    filter: fakeQueueHelper.dataActiveFilter,
                    visible: true,
                    obj: {
                        id: 1,
                        impressionsCompleted: 3,
                        held: false,
                        printerName: "test-printer",
                        title: "test-active",
                    },
                },
                {
                    objectName: "filter1",
                    filter: fakeQueueHelper.dataQueuedFilter,
                    visible: true,
                    obj: {
                        id: 2,
                        impressionsCompleted: 2,
                        held: false,
                        printerName: "test-printer",
                        title: "test-queued",
                    },
                },
                {
                    objectName: "filter2",
                    filter: fakeQueueHelper.dataPausedFilter,
                    visible: true,
                    obj: {
                        id: 3,
                        impressionsCompleted: 1,
                        held: true,
                        printerName: "test-printer",
                        title: "test-paused",
                    },
                }
            ]
        }

        function test_filter_column_visible(data) {
            data.filter.append(data.obj);
            waitForRendering(queueView);

            var column = findChild(queueView, data.objectName);
            compare(column.visible, data.visible);
        }

        // Check that the filter column header name is correct
        function test_filter_column_header_data() {
            return [
                {
                    objectName: "filterHeaderLabel0",
                    name: "Active",
                },
                {
                    objectName: "filterHeaderLabel1",
                    name: "Queued",
                },
                {
                    objectName: "filterHeaderLabel2",
                    name: "Paused",
                }
            ]
        }

        function test_filter_column_header(data) {
            var label = findChild(queueView, data.objectName);
            compare(label.text, data.name);
        }

        // Check that the queue delegate count matches
        function test_queue_delegate_count_data() {
            return testData.jobFilterData;
        }

        function test_queue_delegate_count(data) {
            var repeater = findChild(queueView, data.objectName);
            compare(repeater.count, 0);

            data.filter.append(data.obj);
            waitForRendering(queueView);

            compare(repeater.count, 1);
        }

        // Check that the isActive state of the delegate matches the filter
        function test_queue_delegate_isActive_data() {
            return testData.jobFilterData;
        }

        function test_queue_delegate_isActive(data) {
            data.filter.append(data.obj);
            waitForRendering(queueView);

            var repeater = findChild(queueView, data.objectName);
            compare(repeater.count, 1);

            var delegate = findChild(queueView, data.objectName + "delegate0");
            compare(delegate.visible, data.visible);
            compare(delegate.isActive, data.isActive);
        }

        // Test that clicking on cancel results in a cancel signal
        function test_queue_delegate_job_cancel_data() {
            return testData.jobDelegateSignalData;
        }

        function test_queue_delegate_job_cancel(data) {
            data.filter.append(data.obj);
            waitForRendering(queueView);

            var cancelButton = findChild(queueView, "cancel_button");
            mouseClick(cancelButton);

            cancelSpy.wait();
            compare(cancelSpy.count, 1);
            compare(cancelSpy.signalArguments[0][0], data.obj.printerName);
            compare(cancelSpy.signalArguments[0][1], data.obj.id);
        }

        // Test that clicking on hold results in a hold signal
        function test_queue_delegate_job_hold_data() {
            return testData.jobDelegateSignalData;
        }

        function test_queue_delegate_job_hold(data) {
            data.filter.append(data.obj);
            waitForRendering(queueView);

            var toggleButton = findChild(queueView, "toggle_button");
            mouseClick(toggleButton);

            holdSpy.wait();
            compare(holdSpy.count, 1);
            compare(holdSpy.signalArguments[0][0], data.obj.printerName);
            compare(holdSpy.signalArguments[0][1], data.obj.id);
        }

        // Test that clicking on release results in a release signal
        function test_queue_delegate_job_release_data() {
            return testData.jobDelegateSignalData;
        }

        function test_queue_delegate_job_release(data) {
            data.obj.held = true;  // force into the held position

            data.filter.append(data.obj);
            waitForRendering(queueView);

            var toggleButton = findChild(queueView, "toggle_button");
            mouseClick(toggleButton);

            releaseSpy.wait();
            compare(releaseSpy.count, 1);
            compare(releaseSpy.signalArguments[0][0], data.obj.printerName);
            compare(releaseSpy.signalArguments[0][1], data.obj.id);
        }
    }
}
