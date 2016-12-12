#include "printer.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <QtCore/QDebug>

#include <QtGui/QPainter>

#include <QtPrintSupport/QPrinterInfo>

#include <cups/cups.h>

#include <iostream>

Printer::Printer(QObject *parent)
    : QObject(parent)
    , m_color_mode(Color)
    , m_copies(1)
    , m_duplex(false)
    , m_duplex_supported(false)
    , m_name("")
    , m_pdf_mode(false)
    , m_quality(Normal)
{
    m_cups_dest = 0;
}

Printer::ColorMode Printer::colorMode() const
{
    return m_color_mode;
}

int Printer::copies() const
{
    return m_copies;
}

bool Printer::duplex() const
{
    return m_duplex;
}

bool Printer::duplexSupported() const
{
    return m_duplex_supported;
}

QString Printer::makeOutputFilepath() const
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFileInfo fileInfo(dir.path(), QDateTime::currentDateTime().toString(Qt::ISODate));
    return fileInfo.absoluteFilePath();
}

QString Printer::name() const
{
    return m_name;
}

bool Printer::pdfMode() const
{
    return m_pdf_mode;
}

bool Printer::print(Document *doc)
{
//    // Get all the dests
    int         num_dests;
    cups_dest_t *dests;

    num_dests = cupsGetDests(&dests);  // appear to need to run this first?

//    // List dests

//    for (int i=0; i < num_dests; i++) {
//        qDebug() << "P" << i << dests[i].name << dests[i].instance;
//    }

//    cupsFreeDests(num_dests, dests);

    // Extract the name and instance from the name
    QStringList split = m_name.split("/");

    const char *name = split.takeFirst().toLocal8Bit();
    const char *instance = split.isEmpty() ? "" : split.takeFirst().toLocal8Bit();

    if (split.length() > 0) {
        qWarning() << "Unknown printer name pattern:" << m_name;
        return false;
    }

    std::cout << "Name:" << name << "Instance:" << instance << "\n";

    // Ensure the destination is free
    if (m_cups_dest) {
        cupsFreeDests(1, m_cups_dest);

        m_cups_dest = 0;
    }

    // Get the destination

//    cups_dest_t *dest = cupsGetDest(dests[0].name, dests[0].instance, num_dests, dests);
    cups_dest_t *m_cups_dest = cupsGetNamedDest(CUPS_HTTP_DEFAULT, name, instance);

    if (!m_cups_dest) {
        qWarning() << "Could not find printer:" << m_name;
        return false;
    }

    // TODO: set options on dest?
    // Load options
    QList<QPair<QByteArray, QByteArray>> options;
    QVector<cups_option_t> cupsOptStruct;

    // Copies
    if (m_copies > 1) {
        options.append(QPair<QByteArray, QByteArray>("copies", QString::number(m_copies).toLocal8Bit()));
    }

    // Colour mode
    if (m_color_mode == GrayScale) {
        options.append(QPair<QByteArray, QByteArray>("ColorModel", "CMYGray"));
    }

    // Duplex
    if (m_duplex_supported && m_duplex) {
        if (doc->orientation() == QPrinter::Portrait) {
            options.append(QPair<QByteArray, QByteArray>("sides", "two-sided-long-edge"));
        } else {
            options.append(QPair<QByteArray, QByteArray>("sides", "two-sided-short-edge"));
        }
    }

    // Orientation
    if (doc->orientation() == QPrinter::Landscape) {
        options.append(QPair<QByteArray, QByteArray>("landscape", ""));
    }

    // Print quality
    QString printQuality = "";
    switch (m_quality) {
    case Draft:
        printQuality = "FastDraft";
        break;
    case Best:
        printQuality = "Best";
        break;
    case Photo:
        printQuality = "Photo";
        break;
    case Normal:
    default:
        printQuality = "Normal";
        break;
    }
    //options.append(QPair<QByteArray, QByteArray>("OutputMode", printQuality.toLocal8Bit()));
    m_cups_dest->num_options = cupsAddOption("OutputMode", printQuality.toLocal8Bit(), m_cups_dest->num_options, &m_cups_dest->options);

    // Add default options
    for (int i=0; i < m_cups_dest->num_options; i++) {
        options.append(QPair<QByteArray, QByteArray>(QByteArray(m_cups_dest->options[i].name), QByteArray(m_cups_dest->options[i].value)));
    }

    // Build all our options
    for (int i=options.size() - 1; i > -1; i--) {
        cups_option_t opt;
        opt.name = options[i].first.data();
        opt.value = options[i].second.data();

        qDebug() << "O" << i << opt.name << opt.value;

        cupsOptStruct.append(opt);
    }

    cups_option_t *optionPtr = cupsOptStruct.size() > 0 ? &cupsOptStruct.first() : 0;
    int jobId = cupsPrintFile(m_cups_dest->name,
                              QUrl(doc->url()).toLocalFile().toStdString().c_str(),
                              "Test Print!",  // TODO: build title from document
                              cupsOptStruct.size(), optionPtr);

    qDebug() << "JobID" << jobId;

    // TODO: PDF mode!
    if (m_pdf_mode) {
        Q_EMIT exportRequest(doc->url().toLocalFile());
    }

    return true;
}

void Printer::setColorMode(Printer::ColorMode colorMode)
{
    if (m_color_mode != colorMode) {
        m_color_mode = colorMode;

        Q_EMIT colorModeChanged();
    }
}

void Printer::setCopies(int copies)
{
    if (m_copies != copies) {
        m_copies = copies;

        Q_EMIT copiesChanged();
    }
}

void Printer::setDuplex(bool duplex)
{
    if (m_duplex != duplex) {
        m_duplex = duplex;

        Q_EMIT duplexChanged();
    }
}

void Printer::setDuplexSupported(bool duplexSupported)
{
    if (m_duplex_supported != duplexSupported) {
        m_duplex_supported = duplexSupported;

        Q_EMIT duplexSupportedChanged();
    }
}

void Printer::setName(QString name)
{
    if (m_name != name) {
        m_name = name;

        if (m_pdf_mode) {
            setDuplexSupported(false);
        } else {
            QPrinterInfo printerInfo = QPrinterInfo::printerInfo(m_name);

            if (printerInfo.isNull()) {
                qWarning() << "No printer found:" << m_name;
            } else {
                setDuplexSupported(printerInfo.supportedDuplexModes().contains(QPrinter::DuplexAuto));
            }
        }

        Q_EMIT nameChanged();
        Q_EMIT settingsChanged();
    }
}

void Printer::setPdfMode(bool pdfMode)
{
    if (m_pdf_mode != pdfMode) {
        m_pdf_mode = pdfMode;

        Q_EMIT pdfModeChanged();
    }
}

void Printer::setQuality(Quality quality)
{
    if (m_quality != quality) {
        m_quality = quality;

        Q_EMIT qualityChanged();
    }
}

Printer::Quality Printer::quality() const
{
    return m_quality;
}
