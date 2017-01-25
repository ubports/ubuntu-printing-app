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
    Impl(const std::string& summary,
         const std::string& body,
         const std::string& icon_name):
        m_summary(summary),
        m_body(body),
        m_icon_name(icon_name)
    {
        m_nn.reset(notify_notification_new(m_summary.c_str(),
                                           m_body.c_str(),
                                           m_icon_name.c_str()),
                   [this](NotifyNotification* n) {
                       g_signal_handlers_disconnect_by_data(n, this);
                       g_object_unref(n);
                   });

        g_signal_connect(m_nn.get(), "closed",
                         G_CALLBACK(on_notification_closed), this);
    }

    ~Impl()
    {
    }

    core::Signal<const std::string&>& activated()
    {
        return m_activated;
    }

    core::Signal<>& closed()
    {
        return m_closed;
    }

    void add_action(const std::string& action, const std::string& label)
    {
        notify_notification_add_action(m_nn.get(),
                                       action.c_str(), label.c_str(),
                                       on_notify_activated,
                                       this, nullptr);
    }

    void close()
    {
        if (!notify_is_initted()) {
            g_warning("Tried to close a notification without notify_init().");
            return;
        }

        GError* error = nullptr;
        notify_notification_close(m_nn.get(), &error);

        if (error != nullptr) {
            g_critical("Error closing notification: %s", error->message);
            g_clear_error(&error);
        }
    }

    void show()
    {
        if (!notify_is_initted()) {
            g_critical("Unable to display notifications without notify_init().");
            return;
        }

        if (m_summary.empty()) {
            g_critical("Attempting to show notification with no summary!");
            return;
        }

        GError* error = nullptr;
        notify_notification_show(m_nn.get(), &error);

        if (error != nullptr) {
            g_critical("Error showing notification: %s", error->message);
            g_clear_error(&error);
        }
    }

private:
    static void on_notify_activated(NotifyNotification*,
                                    char* action,
                                    gpointer gthis)
    {
        auto self = static_cast<Impl*>(gthis);
        self->m_activated(action);
    }

    static void on_notification_closed(NotifyNotification*,
                                       gpointer gthis)
    {
        auto self = static_cast<Impl*>(gthis);
        g_debug("Notification was closed.");
        self->m_closed();
    }

    std::shared_ptr<NotifyNotification> m_nn;

    std::string m_summary;
    std::string m_body;
    std::string m_icon_name;

    core::Signal<const std::string&> m_activated;
    core::Signal<> m_closed;
};

Notification::Notification(const std::string& summary,
                           const std::string& body,
                           const std::string& icon_name):
    p(new Impl(summary, body, icon_name))
{
}

Notification::~Notification()
{
}

core::Signal<const std::string&>& Notification::activated()
{
    return p->activated();
}

core::Signal<>& Notification::closed()
{
    return p->closed();
}

void Notification::add_action(const std::string& action,
                              const std::string& label)
{
    p->add_action(action, label);
}

void Notification::close()
{
    p->close();
}

void Notification::show()
{
    p->show();
}

} // notifier
} // printing
} // ubuntu
