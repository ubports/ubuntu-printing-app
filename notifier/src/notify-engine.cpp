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

#include "notify-engine.h"

#include <map>
#include <regex>
#include <unordered_set>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <glib/gi18n.h>
#include <libnotify/notify.h>

namespace ubuntu {
namespace printing {
namespace notifier {

class NotifyEngine::Impl
{
    struct notification_data
    {
        std::shared_ptr<Notification> notification;
        std::unordered_set<std::shared_ptr<Notification>> notifications;
    };

public:
    Impl(const std::shared_ptr<Client>& client,
         const std::shared_ptr<Actions>& actions):
        m_client(client),
        m_actions(actions),
        // NOTE: sorted alphabetically by state
        m_reasons({
            {"cover-open", _("A cover is open on the printer “%s”.")},
            {"cups-missing-filter", _("The printer “%s” can’t be used, because required software is missing.")},
            {"door-open-report", _("A door is open on the printer “%s”.")},
            {"media-empty", _("The printer “%s” is out of paper.")},
            {"media-low", _("The printer “%s” is low on paper.")},
            {"offline", _("The printer “%s” is currently off-line.")},
            {"other", _("The printer “%s” has an unknown problem.")},
            {"toner-empty", _("The printer “%s” is out of toner.")},
            {"toner-low", _("The printer “%s” is low on toner.")},
        })
    {
    }

    ~Impl()
    {
    }

    std::string get_displayable_reason(const std::string& reason)
    {
        return m_reasons[reason];
    }

    std::unordered_set<std::string> get_notified_reasons(const Printer& printer)
    {
        return m_notified[printer.name];
    }

    void set_notified_reasons(const Printer& printer,
                              std::unordered_set<std::string> reasons)
    {
        m_notified[printer.name] = reasons;
    }

    void show_notification(const std::shared_ptr<Notification>& notification)
    {
        if (notification.get() == nullptr) {
            return;
        }

        notification->activated().connect([this](const std::string& action) {
                static const std::regex actionsettings{"^settings:///.*"};
                if (std::regex_match(action, actionsettings)) {
                    m_actions->open_settings_app(action);
                }
                // Otherwise we just ignore the action.
            });
        notification->closed().connect([this, &notification]() {
                g_debug("Closed notification.");
                auto data = new notification_data({notification, m_notifications});
                g_idle_add(on_delete_later, data);
            });
        m_notifications.emplace(notification);
        notification->show();
    }

private:
    static gboolean on_delete_later(gpointer gdata)
    {
        auto data = static_cast<notification_data*>(gdata);
        data->notifications.erase(data->notification);
        return G_SOURCE_REMOVE;
    }

    std::shared_ptr<Client> m_client;
    std::shared_ptr<Actions> m_actions;

    // The set of current notifications
    std::unordered_set<std::shared_ptr<Notification>> m_notifications;

    // The map of "printer" -> set of notified notified states
    std::map<std::string, std::unordered_set<std::string>> m_notified;

    // The map of "reason" -> _("Translated displayable reason") strings
    std::map<std::string, std::string> m_reasons;
}; // class Impl


NotifyEngine::NotifyEngine(const std::shared_ptr<Client>& client,
                           const std::shared_ptr<Actions>& actions):
    p(new Impl(client, actions))
{
    client->job_state_changed().connect([this](const Job& job) {
            const auto& printer = job.printer;
            g_debug("State changed for job %u '%s` on printer '%s' and reasons were '%s'",
                    job.id,
                    job.name.c_str(),
                    printer.description.empty() ? printer.name.c_str() : printer.description.c_str(),
                    job.state_reasons.c_str());
            if (job.state == Job::State::COMPLETED) {
                auto notification = build_job_notification(job);
                p->show_notification(notification);
            }
        });
    client->printer_state_changed().connect([this](const Printer& printer) {
            g_debug("Printer state changed for reasons: '%s'",
                    printer.state_reasons.c_str());
            if (printer.num_jobs > 0) {
                auto notified = p->get_notified_reasons(printer);
                std::unordered_set<std::string> reasons;
                boost::split(reasons, printer.state_reasons, boost::is_any_of(","));
                for (const auto& reason: reasons) {
                    if (notified.count(reason) == 0) {
                        auto notification = build_printer_notification(printer, reason);
                        p->show_notification(notification);
                    }
                }
                p->set_notified_reasons(printer, reasons);
            }
        });
}

NotifyEngine::~NotifyEngine()
{
}

std::shared_ptr<Notification> NotifyEngine::build_job_notification(const Job& job)
{
    std::shared_ptr<Notification> notification;

    auto summary = boost::format(_("“%s” has printed.")) % job.name;
    notification.reset(new Notification(summary.str(), "", NOTIFY_PRINTER_ICON));

    return notification;
}

std::shared_ptr<Notification> NotifyEngine::build_printer_notification(const Printer& printer,
                                                                       const std::string& reason)
{
    std::shared_ptr<Notification> notification;

    const auto& displayname = printer.description.empty() ? printer.description : printer.name;

    // Get the reason text and add summary/body if valid
    auto untranslated = p->get_displayable_reason(reason);
    if (!untranslated.empty()) {
        auto summary = boost::format(untranslated) % displayname;

        auto jobtext = ngettext("You have %d job queued to print on this printer.", 
                                "You have %d jobs queued to print on this printer.",
                                printer.num_jobs);
        auto body = boost::format(jobtext) % printer.num_jobs;

        notification.reset(new Notification(summary.str(), body.str(), NOTIFY_ERROR_ICON));

        notification->set_hint(NOTIFY_HINT_SNAP, "true");
        notification->add_action("PRINTER_ACTION_IGNORE", _("OK"));
        std::string settings_url{"settings:///system/printers/"};
        notification->add_action(settings_url + printer.name, _("Settings…"));
    }

    return notification;
}


} //notifier
} // printing
} // ubuntu
