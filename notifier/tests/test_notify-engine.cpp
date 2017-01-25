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

#include "actions-mock.h"
#include "client-mock.h"
#include "mock-notification.h"
#include "notify-engine.h"

#include <memory>

#include <gmock/gmock.h>

using namespace ubuntu::printing::notifier;
using namespace ::testing;

class EngineFixture: public ::testing::Test
{
};

class MockEngine: public NotifyEngine
{
public:
    MockEngine(const std::shared_ptr<Client>& client,
               const std::shared_ptr<Actions>& actions):
        NotifyEngine(client, actions)
    {
    }

    ~MockEngine()
    {
    }

    MOCK_METHOD1(build_job_notification,
                 std::shared_ptr<Notification>(const Job&));
    MOCK_METHOD2(build_printer_notification,
                 std::shared_ptr<Notification>(const Printer&,
                                               const std::string&));
};

TEST_F(EngineFixture, NotifyEngine)
{
    auto client = std::make_shared<MockClient>();
    auto actions = std::make_shared<MockActions>();

    // Test for initialization
    auto engine = std::make_shared<NotifyEngine>(client, actions);
    ASSERT_FALSE(nullptr == engine);

    // Test refresh
    EXPECT_CALL(*client, refresh()).Times(1)
        .WillOnce(Invoke([&client](){
                    // Notify a printer state change
                    Printer printer;
                    printer.name = "a-printer";
                    printer.description = "A Printer";
                    printer.accepting_jobs = true;
                    printer.num_jobs = 1;
                    printer.state_reasons = "door-open-report";
                    client->m_printer_state_changed(printer);

                    // Now with 2 reasons
                    printer.state_reasons = "door-open-report,something-else";
                    client->m_printer_state_changed(printer);

                    // Notify a COMPLETED job
                    Job fake_job;
                    fake_job.id = 42;
                    fake_job.state = Job::State::COMPLETED;
                    fake_job.name = "Life, The Universe, and Everything";
                    fake_job.printer.description = "Deep Thought";
                    fake_job.printer.name = "deep-thought";
                    client->m_job_state_changed(fake_job);
                }));
    client->refresh();
}

TEST_F(EngineFixture, JobNotification)
{
    auto client = std::make_shared<MockClient>();
    auto actions = std::make_shared<MockActions>();
    auto engine = std::make_shared<MockEngine>(client, actions);

    auto notification = std::make_shared<MockNotification>("Job finished",
                                                           "",
                                                           NOTIFY_PRINTER_ICON);
    EXPECT_CALL(*engine, build_job_notification(_))
        .WillOnce(Return(notification));
    EXPECT_CALL(*notification, show()).Times(1)
        .WillOnce(Invoke([&notification](){
                    notification->m_closed();
                }));

    Job fake_job;
    fake_job.id = 42;
    fake_job.state = Job::State::COMPLETED;
    fake_job.name = "Life, The Universe, and Everything";
    fake_job.printer.description = "Deep Thought";
    fake_job.printer.name = "deep-thought";

    client->m_job_state_changed(fake_job);
}

TEST_F(EngineFixture, PrinterNotification)
{
    auto client = std::make_shared<MockClient>();
    auto actions = std::make_shared<MockActions>();
    auto engine = std::make_shared<MockEngine>(client, actions);

    auto notification = std::make_shared<MockNotification>("Printer fail",
                                                           "Some jobs",
                                                           NOTIFY_ERROR_ICON);
    EXPECT_CALL(*engine, build_printer_notification(_, _))
        .WillOnce(Return(notification));
    EXPECT_CALL(*notification, show()).Times(1)
        .WillOnce(Invoke([&notification](){
                    notification->m_closed();
                }));

    Printer printer;
    printer.name = "a-printer";
    printer.description = "A Printer";
    printer.accepting_jobs = true;
    printer.num_jobs = 1;
    printer.state_reasons = "door-open-report";

    client->m_printer_state_changed(printer);
}
