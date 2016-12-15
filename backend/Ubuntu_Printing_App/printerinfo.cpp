/*
 * Copyright 2016 Canonical Ltd.
 *
 * This file is part of ubuntu-printing-app.
 *
 * ubuntu-printing-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * ubuntu-printing-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "printerinfo.h"

#include <QtPrintSupport/QPrinterInfo>

PrinterInfo::PrinterInfo(QObject *parent) : QObject(parent)
{
    m_available_printer_names = QStringList() << QPrinterInfo::availablePrinterNames() << "Create PDF";
    m_default_printer_name = QPrinterInfo::defaultPrinterName();

    // Watch printers for updates
    m_timer_printers = new QTimer(this);
    connect(m_timer_printers, SIGNAL(timeout()), this, SLOT(updatePrinters()));
    m_timer_printers->start(5000);
}

QStringList PrinterInfo::availablePrinterNames() const
{
    return m_available_printer_names;
}

QString PrinterInfo::defaultPrinterName() const
{
    return m_default_printer_name;
}

void PrinterInfo::updatePrinters()
{
    if (m_available_printer_names != QStringList() << QPrinterInfo::availablePrinterNames() << "Create PDF") {
        m_available_printer_names = QStringList() << QPrinterInfo::availablePrinterNames() << "Create PDF";

        Q_EMIT availablePrinterNamesChanged();
    }

    if (m_default_printer_name != QPrinterInfo::defaultPrinterName()) {
        m_default_printer_name = QPrinterInfo::defaultPrinterName();

        Q_EMIT defaultPrinterNameChanged();
    }
}
