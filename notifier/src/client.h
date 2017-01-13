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

#include "job.h"
#include "printer.h"

#include <core/signal.h>


namespace unity {
namespace indicator {
namespace printers {

    class Client {
    public:
        explicit Client() = default;
        virtual ~Client() = default;

        // Signals corresponding to printers
        virtual core::Signal<const Printer&>& printer_state_changed() = 0;

        // Signals corresponding to jobs
        virtual core::Signal<const Job&>& job_state_changed() = 0;

        // Methods to manage notification monitoring
        virtual void create_subscription() = 0;
        virtual void renew_subscription() = 0;
        virtual void cancel_subscription() = 0;

        // To iniitalize the indicator with current jobs
        virtual void refresh() = 0;

    private:
        // disable copying 
        Client(const Client&) = delete; 
        Client& operator=(const Client&) = delete; 
    };

} // printers
} // indicators
} // unity
