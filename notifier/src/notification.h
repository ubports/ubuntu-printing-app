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

#include <string>

#include <core/signal.h>

namespace ubuntu {
namespace printing {
namespace notifier {

    class Notification {
    public:
        Notification(const std::string& summary,
                     const std::string& body,
                     const std::string& icon_name);
        virtual ~Notification();

        // Signal for activation of actions in notification
        virtual core::Signal<const std::string&>& activated();

        // Signal for forced closing of notification
        virtual core::Signal<>& closed();

        virtual void add_action(const std::string& action,
                                const std::string& label);

        // For hints
        virtual void set_hint(const std::string& hint,
                              const std::string& value);

        virtual void close();
        virtual void show();

    private:
        class Impl;
        std::unique_ptr<Impl> p;

        // disable copying
        Notification(const Notification&) = delete;
        Notification& operator=(const Notification&) = delete;
    };

} // notifier
} // printing
} // ubuntu
