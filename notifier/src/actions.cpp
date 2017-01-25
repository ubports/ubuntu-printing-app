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

#include "actions.h"

#include <glib.h>
#include <url-dispatcher.h>

namespace ubuntu {
namespace printing {
namespace notifier {

Actions::Actions()
{
}

Actions::~Actions()
{
}

void Actions::open_settings_app(const std::string& url)
{
    g_debug("Dispatching url '%s'", url.c_str());
    url_dispatch_send(url.c_str(), nullptr, nullptr);
}

} // notifier
} // printing
} // ubuntu
