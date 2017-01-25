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

#include "notification.h"

#include <core/signal.h>
#include <gmock/gmock.h>

namespace ubuntu {
namespace printing {
namespace notifier {

class MockNotification: public Notification
{
 public:
    MockNotification(const std::string& summary, const std::string& body,
                     const std::string& icon_name):
        Notification(summary, body, icon_name)
    {
    }

    ~MockNotification()
    {
    }

    core::Signal<>& closed()
    {
        return m_closed;
    }

    MOCK_METHOD0(close, void());
    MOCK_METHOD0(show, void());

    core::Signal<> m_closed;
};

} // notifier
} // printing
} // ubuntu
