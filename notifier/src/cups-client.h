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

#include "client.h"
#include "job.h"
#include "printer.h"

#include <core/signal.h>

#include <memory>

namespace unity {
namespace indicator {
namespace printers {

#define CUPS_DBUS_NAME "org.cups.cupsd.Notifier"
#define CUPS_DBUS_PATH "/org/cups/cupsd/Notifier"
#define CUPS_DBUS_INTERFACE "org.cups.cupsd.Notifier"

    class CupsClient : public Client {
    public:
        CupsClient();
        virtual ~CupsClient();

        // Signals corresponding to printers
        core::Signal<const Printer&>& printer_state_changed() override;

        // Signals corresponding to jobs
        core::Signal<const Job&>& job_state_changed() override;

        // Methods to manage notification monitoring
        virtual void create_subscription() override;
        virtual void renew_subscription() override;
        virtual void cancel_subscription() override;

        // To initialize the indicator with current jobs
        virtual void refresh() override;

    private:
        class Impl;
        std::unique_ptr<Impl> p;

        // disable copying 
        CupsClient(const CupsClient&) = delete; 
        CupsClient& operator=(const CupsClient&) = delete; 
    };

} // printers
} // indicators
} // unity
