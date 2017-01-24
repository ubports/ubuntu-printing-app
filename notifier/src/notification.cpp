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

#include <libnotify/notification.h>
#include <libnotify/notify.h>

namespace ubuntu {
namespace printing {
namespace notifier {

class Notification::Impl
{
public:
    Impl()
    {
    }

    ~Impl()
    {
    }

    std::string m_icon_name;
    std::string m_summary;
    std::string m_body;
};

Notification::Notification():
    p(new Impl())
{
}

Notification::~Notification()
{
}

void Notification::set_icon_name(const std::string& icon_name)
{
    p->m_icon_name = icon_name;
}

void Notification::set_summary(const std::string& summary)
{
    p->m_summary = summary;
}

void Notification::set_body(const std::string& body)
{
    p->m_body = body;
}

void Notification::show() const
{
    if (!p->m_summary.empty()) {
        GError* error = nullptr;
        auto nn = notify_notification_new(p->m_summary.c_str(),
                                          p->m_body.c_str(),
                                          p->m_icon_name.c_str());
        if (notify_is_initted()) {
            notify_notification_show(nn, &error);
        } else {
            g_warning("Unable to display notifications without notify_init().");
        }

        if (error != nullptr) {
            g_critical("Error showing notification: %s", error->message);
            g_clear_error(&error);
        }

        g_object_unref(nn);
    }
}

} // notifier
} // printing
} // ubuntu
