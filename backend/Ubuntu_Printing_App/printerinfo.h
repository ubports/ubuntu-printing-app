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
 *
 * Authored-by: Andrew Hayzen <andrew.hayzen@canonical.com>
 */
#ifndef PRINTERINFO_H
#define PRINTERINFO_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

class PrinterInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList availablePrinterNames READ availablePrinterNames NOTIFY availablePrinterNamesChanged)
    Q_PROPERTY(QString defaultPrinterName READ defaultPrinterName NOTIFY defaultPrinterNameChanged)
public:
    explicit PrinterInfo(QObject *parent = 0);
    QStringList availablePrinterNames() const;
    QString defaultPrinterName() const;
signals:
    void availablePrinterNamesChanged();
    void defaultPrinterNameChanged();
private slots:
    void updatePrinters();
private:
    QStringList m_available_printer_names;
    QString m_default_printer_name;
    QTimer *m_timer_printers;
};

#endif // PRINTERINFO_H
