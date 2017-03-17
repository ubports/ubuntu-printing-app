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

import Ubuntu.Components.Extras.Printers 0.1

QtObject {
    id: printJobs

    readonly property int count: Printers.printJobs.count

    function cancelJob(printerName, jobId) {
        return Printers.cancelJob(printerName, jobId);
    }

    function createActiveFilter() {
        var filter = Printers.createJobFilter();
        filter.filterOnActive();
        return filter;
    }

    function createPausedFilter() {
        var filter = Printers.createJobFilter();
        filter.filterOnPaused();
        return filter;
    }

    function createQueuedFilter() {
        var filter = Printers.createJobFilter();
        filter.filterOnQueued();
        return filter;
    }

    function holdJob(printerName, jobId) {
        return Printers.holdJob(printerName, jobId);
    }

    function releaseJob(printerName, jobId) {
        return Printers.releaseJob(printerName, jobId);
    }
}
