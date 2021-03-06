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
