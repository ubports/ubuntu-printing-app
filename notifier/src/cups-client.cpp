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

#include "cups-client.h"
#include "cups-cupsd-notifier.h"

#include <cups/cups.h>

#include <stdexcept>

namespace unity {
namespace indicator {
namespace printers {

#define NOTIFY_LEASE_DURATION (24 * 60 * 60)


class CupsClient::Impl {
public:
    Impl()
    {
        GError *error = nullptr;

        m_notifier_proxy = notifier_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                           G_DBUS_PROXY_FLAGS_NONE,
                                                           nullptr,
                                                           CUPS_DBUS_PATH,
                                                           nullptr,
                                                           &error);

        if (error != nullptr) {
            std::string msg{"Error creating cups notifier proxy: "};
            throw std::runtime_error(msg + error->message);
            g_clear_error(&error);
        }

        g_object_connect(m_notifier_proxy,
                         "signal::job-created", on_job_changed, this,
                         "signal::job-state", on_job_changed, this,
                         "signal::job-completed", on_job_changed, this,
                         "signal::printer-state-changed", on_printer_state_changed, this,
                         nullptr);
    }

    ~Impl()
    {
        if (m_notifier_proxy != nullptr) {
            g_signal_handlers_disconnect_by_data(m_notifier_proxy, this);
            g_clear_object(&m_notifier_proxy);
        }

        // Cancel the subscription from cups, so its notifier can exit.
        cancel_subscription();
    }

    // Signals to propagate
    core::Signal<const Printer&>& printer_state_changed()
    {
        return m_printer_state_changed;
    }

    core::Signal<const Job&>& job_state_changed()
    {
        return m_job_state_changed;
    }

    void create_subscription()
    {
        ipp_t *req;
        ipp_t *resp;
        ipp_attribute_t *attr;

        req = ippNewRequest (IPP_CREATE_PRINTER_SUBSCRIPTION);
        ippAddString (req, IPP_TAG_OPERATION, IPP_TAG_URI,
                      "printer-uri", NULL, "/");
        ippAddString (req, IPP_TAG_SUBSCRIPTION, IPP_TAG_KEYWORD,
                      "notify-events", NULL, "all");
        ippAddString (req, IPP_TAG_SUBSCRIPTION, IPP_TAG_URI,
                      "notify-recipient-uri", NULL, "dbus://");
        ippAddInteger (req, IPP_TAG_SUBSCRIPTION, IPP_TAG_INTEGER,
                       "notify-lease-duration", NOTIFY_LEASE_DURATION);

        resp = cupsDoRequest (CUPS_HTTP_DEFAULT, req, "/");
        if (!resp || cupsLastError() != IPP_OK) {
            g_warning ("Error subscribing to CUPS notifications: %s\n",
                       cupsLastErrorString ());
            return;
        }

        attr = ippFindAttribute (resp, "notify-subscription-id", IPP_TAG_INTEGER);
        if (attr) {
            m_subscription_id = ippGetInteger (attr, 0);
        } else {
            g_warning ("ipp-create-printer-subscription response doesn't contain "
                       "subscription id.\n");
        }
        ippDelete (resp);

        // Set up to renew the subscription a minute before it expires
        g_timeout_add_seconds(NOTIFY_LEASE_DURATION - 60,
                              on_subscription_timeout,
                              this);
    }

    void renew_subscription()
    {
        ipp_t *req;
        ipp_t *resp;

        req = ippNewRequest (IPP_RENEW_SUBSCRIPTION);
        ippAddInteger (req, IPP_TAG_OPERATION, IPP_TAG_INTEGER,
                       "notify-subscription-id", m_subscription_id);
        ippAddString (req, IPP_TAG_OPERATION, IPP_TAG_URI,
                      "printer-uri", NULL, "/");
        ippAddString (req, IPP_TAG_SUBSCRIPTION, IPP_TAG_URI,
                      "notify-recipient-uri", NULL, "dbus://");
        ippAddInteger (req, IPP_TAG_SUBSCRIPTION, IPP_TAG_INTEGER,
                       "notify-lease-duration", NOTIFY_LEASE_DURATION);

        resp = cupsDoRequest (CUPS_HTTP_DEFAULT, req, "/");
        if (!resp || cupsLastError() != IPP_OK) {
            g_warning ("Error renewing CUPS subscription %d: %s\n",
                       m_subscription_id, cupsLastErrorString ());
            create_subscription();
        }

        ippDelete (resp);
    }

    void cancel_subscription()
    {
        ipp_t *req;
        ipp_t *resp;

        if (m_subscription_id <= 0) {
            return;
        }

        req = ippNewRequest (IPP_CANCEL_SUBSCRIPTION);
        ippAddString (req, IPP_TAG_OPERATION, IPP_TAG_URI,
                      "printer-uri", NULL, "/");
        ippAddInteger (req, IPP_TAG_OPERATION, IPP_TAG_INTEGER,
                       "notify-subscription-id", m_subscription_id);

        resp = cupsDoRequest (CUPS_HTTP_DEFAULT, req, "/");
        if (!resp || cupsLastError() != IPP_OK) {
            g_warning ("Error unsubscribing to CUPS notifications: %s\n",
                       cupsLastErrorString ());
            return;
        }

        ippDelete (resp);
    }

    void refresh()
    {
        int num_dests;
        cups_dest_t* dests;

        num_dests = cupsGetDests(&dests);
        for (int i = 0; i < num_dests; i++) {
            auto printer = get_printer_info(dests[i].name);

            cups_job_t* jobs;
            const auto num_jobs = cupsGetJobs(&jobs, dests[i].name,
                                              true, CUPS_WHICHJOBS_ACTIVE);
            for (int j = 0; j < num_jobs; j++) {
                Job job;
                job.id = jobs[j].id;
                job.state = static_cast<Job::State>(jobs[j].state);
                job.name = jobs[j].title;

                job.printer = printer;

                m_job_state_changed(job);
            }
            cupsFreeJobs(num_jobs, jobs);
        }
        cupsFreeDests(num_dests, dests);
    }

private:
    // Method to get Printer object from the name
    Printer get_printer_info(const std::string& name)
    {
        int num_dests;
        cups_dest_t* dests;
        cups_dest_t* our_dest;
        Printer printer;
        printer.name = name;

        num_dests = cupsGetDests(&dests);
        our_dest = cupsGetDest(name.c_str(), nullptr,
                               num_dests, dests);
        
        // Get the printer's description
        auto description = cupsGetOption("printer-info",
                                         our_dest->num_options,
                                         our_dest->options);
        if (description != nullptr) {
            printer.description = description;
        }

        cupsFreeDests(num_dests, dests);
        return printer;
    }
 
    static gboolean on_subscription_timeout(gpointer gthis)
    {
        static_cast<Impl*>(gthis)->renew_subscription();
        return G_SOURCE_CONTINUE;
    }

    static void on_job_changed (Notifier*,
            const char* printer_text,
            const char* printer_uri,
            const char *printer_name,
            unsigned int printer_state,
            const char *printer_state_reasons,
            bool printer_is_accepting_jobs,
            unsigned int job_id,
            unsigned int job_state,
            const char *job_state_reasons,
            const char *job_name,
            unsigned int job_impressions_completed,
            gpointer gthis)
    {
        auto self = static_cast<Impl*>(gthis);

        auto printer = self->get_printer_info(printer_name);
        printer.state = static_cast<Printer::State>(printer_state);
        printer.text = printer_text;
        printer.uri = printer_uri;
        printer.state_reasons = printer_state_reasons;
        printer.accepting_jobs = printer_is_accepting_jobs;

        Job job;
        job.printer = printer;
        job.state = static_cast<Job::State>(job_state);
        job.id = job_id;
        job.name = job_name;
        job.state_reasons = job_state_reasons;
        job.impressions_completed = job_impressions_completed;

        self->m_job_state_changed(job);
    }

    static void on_printer_state_changed(Notifier*,
                                         const char* text,
                                         const char* uri,
                                         const char* name,
                                         unsigned int state,
                                         const char* state_reasons,
                                         bool is_accepting_jobs,
                                         gpointer gthis)
    {
        auto self = static_cast<Impl*>(gthis);

        auto printer = self->get_printer_info(name);
        printer.state = static_cast<Printer::State>(state);
        printer.text = text;
        printer.uri = uri;
        printer.state_reasons = state_reasons;
        printer.accepting_jobs = is_accepting_jobs;
        self->m_printer_state_changed(printer);
    }

    int m_subscription_id = 0;
    Notifier* m_notifier_proxy = nullptr;
    core::Signal<const Printer&> m_printer_state_changed;
    core::Signal<const Job&> m_job_state_changed;
};

CupsClient::CupsClient() :
    p(new Impl())
{
}

CupsClient::~CupsClient()
{
}

core::Signal<const Printer&>& CupsClient::printer_state_changed()
{
    return p->printer_state_changed();
}

core::Signal<const Job&>& CupsClient::job_state_changed()
{
    return p->job_state_changed();
}

void CupsClient::create_subscription()
{
    p->create_subscription();
}

void CupsClient::renew_subscription()
{
    p->renew_subscription();
}

void CupsClient::cancel_subscription()
{
    p->cancel_subscription();
}

void CupsClient::refresh()
{
    p->refresh();
}

} // printers
} // indicators
} // unity
