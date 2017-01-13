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

#include "utils.h"

#include <gtest/gtest.h>

using namespace unity::indicator::printers;


TEST(Utilities, testPrependSnapPathSet) {
    ASSERT_EQ(0, setenv("SNAP", "/snap", 1));
    EXPECT_EQ("/snap/bar", Utilities::prepend_snap_path("/bar"));
    ASSERT_EQ(0, unsetenv("SNAP"));
}

TEST(Utilities, testPrependSnapPathUnset) {
    ASSERT_EQ(0, unsetenv("SNAP"));
    EXPECT_EQ("/bar", Utilities::prepend_snap_path("/bar"));
}
