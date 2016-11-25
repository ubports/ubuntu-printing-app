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
