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

#include "actions.h"

#include <gmock/gmock.h>

namespace ubuntu {
namespace printing {
namespace notifier {

    class MockActions: public Actions
    {
    public:
        MockActions():
            Actions()
        {
        }

        ~MockActions() = default;

        MOCK_METHOD1(open_settings_app, void(const std::string&));
    };

} // notifier
} // printing
} // ubuntu
