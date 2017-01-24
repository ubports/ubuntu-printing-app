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

#include <boost/algorithm/string.hpp>
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
            // NOTE: sorted alphabetically by state
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

private:
    std::shared_ptr<Client> m_client;

    // The map of "printer" -> set of notified notified states
    std::map<std::string, std::unordered_set<std::string>> m_notified;

    // The map of "reason" -> _("Translated displayable reason") strings
    std::map<std::string, std::string> m_reasons;
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
                notification->show();
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
                        notification->show();
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
    auto notification = std::make_shared<Notification>();
    notification->set_icon_name(NOTIFY_PRINTER_ICON);

    auto summary = boost::format(_("“%s” has printed.")) % job.name;
    notification->set_summary(summary.str());

    return notification;
}

std::shared_ptr<Notification> NotifyEngine::build_printer_notification(const Printer& printer,
                                                                       const std::string& reason)
{
    auto notification = std::make_shared<Notification>();
    notification->set_icon_name(NOTIFY_ERROR_ICON);

    const auto& displayname = printer.description.empty() ? printer.description : printer.name;

    // Get the reason text and add summary/body if valid
    auto untranslated = p->get_displayable_reason(reason);
    if (!untranslated.empty()) {
        auto summary = boost::format(untranslated) % displayname;
        notification->set_summary(summary.str());

        auto jobtext = ngettext("You have %d job queued to print on this printer.", 
                                "You have %d jobs queued to print on this printer.",
                                printer.num_jobs);
        auto body = boost::format(jobtext) % printer.num_jobs;
        notification->set_body(body.str());
    }

    return notification;
}


} //notifier
} // printing
} // ubuntu
