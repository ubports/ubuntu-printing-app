/*
 * Copyright 2013-2016 Canonical Ltd.
 *
 * This file is part of webbrowser-app.
 *
 * webbrowser-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * webbrowser-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtQml/QQmlEngine>
#include <QtQml/QtQml>
#include <QtQuickTest/QtQuickTest>

// Mock the PrinterEnum's from Ubuntu.Settings.Printers
class PrinterEnum
{
    Q_GADGET
public:
    enum class ColorModelType
    {
        GrayType = 0,
        ColorType,
        UnknownType,
    };
    Q_ENUM(ColorModelType)

    enum class DuplexMode
    {
        DuplexNone = 0,
        DuplexLongSide,
        DuplexShortSide,
    };
    Q_ENUM(DuplexMode)

    enum class PrintRange
    {
        AllPages = 0,
        PageRange,
    };
    Q_ENUM(PrintRange)
};

int main(int argc, char** argv)
{
    const char* uri = "Ubuntu.Settings.Printers";

    qmlRegisterUncreatableType<PrinterEnum>(uri, 0, 1, "PrinterEnum", "Is an enum");

    return quick_test_main(argc, argv, "QmlTests", nullptr);
}

#include "tst_QmlTests.moc"
