/*
 * Copyright 2017 Canonical Ltd.
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

#include "actions.h"
#include "client.h"
#include "notification.h"

#include <memory>

namespace ubuntu {
namespace printing {
namespace notifier {

    // Icons for notifications
    static constexpr const char* NOTIFY_PRINTER_ICON{"printer-symbolic"};
    static constexpr const char* NOTIFY_ERROR_ICON{"printer-error-symbolic"};

    // Hints for notifications
    static constexpr const char* NOTIFY_HINT_SNAP{"x-canonical-snap-decisions"};

    class NotifyEngine {
    public:
        NotifyEngine(const std::shared_ptr<Client>& client,
                     const std::shared_ptr<Actions>& actions);
        virtual ~NotifyEngine();

        virtual std::shared_ptr<Notification> build_job_notification(const Job& job);
        virtual std::shared_ptr<Notification> build_printer_notification(const Printer& printer,
                                                                         const std::string& reason);

    private:
        class Impl;
        std::unique_ptr<Impl> p;

        // disable copying
        NotifyEngine(const NotifyEngine&) = delete;
        NotifyEngine& operator=(const NotifyEngine&) = delete;
    };

} // notifier
} // printing
} // ubuntu
