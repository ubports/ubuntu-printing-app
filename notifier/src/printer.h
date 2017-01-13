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

#include <string>

namespace unity {
namespace indicator {
namespace printers {

    struct Printer {
        // State to match ipp_jstate_t from cups.h
        typedef enum {
            IDLE = 3,
            PROCESSING,
            STOPPED
        } State;
        State state = IDLE;

        std::string name;
        std::string description;
        std::string text;
        std::string uri;
        std::string state_reasons;
        bool accepting_jobs = false;
    };

} // printers
} // indicators
} // unity
