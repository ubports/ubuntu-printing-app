/*
 * Copyright 2016-2017 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "printer.h"

#include <string>

namespace unity {
namespace indicator {
namespace printers {

    struct Job {
        // State to match ipp_jstate_t from cups.h
        typedef enum {
            PENDING = 3,
            HELD,
            PROCESSING,
            STOPPED,
            CANCELED,
            ABORTED,
            COMPLETED
        } State;
        State state = PENDING;

        uint32_t id = 0;
        std::string name;
        std::string state_reasons;
        uint32_t impressions_completed = 0;

        Printer printer;
    };

} // printers
} // indicators
} // unity
