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

#include <core/signal.h>
#include <gmock/gmock.h>

namespace unity {
namespace indicator {
namespace printers {

    class MockClient: public Client
    {
    public:
        explicit MockClient(): Client() {}
        ~MockClient() = default;

        core::Signal<const Printer&>& printer_state_changed()
        {
            return m_printer_state_changed;
        }
        core::Signal<const Job&>& job_state_changed()
        {
            return m_job_state_changed;
        }

        MOCK_METHOD0(create_subscription, void());
        MOCK_METHOD0(renew_subscription, void());
        MOCK_METHOD0(cancel_subscription, void());
        MOCK_METHOD0(refresh, void());

        core::Signal<const Printer&> m_printer_state_changed;
        core::Signal<const Job&> m_job_state_changed;
    };

} // printers
} // indicator
} // unity
