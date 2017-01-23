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
#include <unordered_set>

#include <boost/format.hpp>

#include <glib/gi18n.h>
#include <libnotify/notify.h>

namespace ubuntu {
namespace printing {
namespace notifier {

class NotifyEngine::Impl
{
public:
    Impl(const std::shared_ptr<Client>& client):
        m_client(client),
        m_reasons({
            {"media-low", _("The printer “%s” is low on paper.")},
            {"media-empty", _("The printer “%s” is out of paper.")},
            {"toner-low", _("The printer “%s” is low on toner.")},
            {"toner-empty", _("The printer “%s” is out of toner.")},
            {"cover-open", _("A cover is open on the printer “%s”.")},
            {"door-open", _("A door is open on the printer “%s”.")},
            {"cups-missing-filter", _("The printer “%s” can’t be used, because required software is missing.")},
            {"offline", _("The printer “%s” is currently off-line.")},
            {"other", _("The printer “%s” has an unknown problem.")},
        })
    {
    }

    ~Impl()
    {
    }

private:
    std::shared_ptr<Client> m_client;

    // The map of "printer" -> set of notified notified states
    const std::map<std::string, std::unordered_set<std::string>> m_notified;

    // The map of "reason" -> _("Translated displayable reason") strings
    const std::map<std::string, std::string> m_reasons;
}; // class Impl


NotifyEngine::NotifyEngine(const std::shared_ptr<Client>& client):
    p(new Impl(client))
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
                notification.show();
            }
        });
    client->printer_state_changed().connect([this](const Printer& printer) {
            g_debug("Printer state changed for reasons: '%s'",
                    printer.state_reasons.c_str());
            if (printer.num_jobs > 0 && printer.state_reasons != "none") {
                auto notification = build_printer_notification(printer);
                notification.show();
            }
        });
}

NotifyEngine::~NotifyEngine()
{
}

Notification NotifyEngine::build_job_notification(const Job& job)
{
    Notification notification;
    notification.set_icon_name(NOTIFY_PRINTER_ICON);

    auto summary = boost::format(_("“%s” has printed.")) % job.name;
    notification.set_summary(summary.str());

    return notification;
}

Notification NotifyEngine::build_printer_notification(const Printer& printer)
{
    Notification notification;
    notification.set_icon_name(NOTIFY_ERROR_ICON);

    auto body = boost::format(ngettext("You have %d job queued to print on this printer.", 
                                       "You have %d jobs queued to print on this printer.",
                                 printer.num_jobs)) % printer.num_jobs;
    notification.set_body(body.str());

    return notification;
}


} //notifier
} // printing
} // ubuntu
