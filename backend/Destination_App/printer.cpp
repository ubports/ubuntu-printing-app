#include "printer.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <QtCore/QDebug>

#include <QtPrintSupport/QPrinterInfo>

#include <cups/cups.h>

#define __CUPS_ADD_OPTION(dest, name, value) dest->num_options = \
    cupsAddOption(name, value, dest->num_options, &dest->options);

Printer::Printer(QObject *parent)
    : QObject(parent)
    , m_color_mode(Color)
    , m_copies(1)
    , m_duplex(false)
    , m_duplex_supported(false)
    , m_job_id(0)
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
    // Get all the dests
    int         num_dests;  Q_UNUSED(num_dests)
    cups_dest_t *dests;

    num_dests = cupsGetDests(&dests);  // FIXME: appear to need to run this first?
    cupsFreeDests(num_dests, dests);

    // Extract the name and instance from the m_name
    // TODO: instead do this in the setName() ? to remove QPrinterInfo ?
    QStringList split = m_name.split("/");

    const char *name = split.takeFirst().toLocal8Bit();
    const char *instance = split.isEmpty() ? "" : split.takeFirst().toLocal8Bit();

    if (split.length() > 0) {
        qWarning() << "Unknown printer name pattern:" << m_name;
        return false;
    }


    // Ensure the destination is free
    if (m_cups_dest) {
        cupsFreeDests(1, m_cups_dest);

        m_cups_dest = 0;
    }

    // Get the destination
    cups_dest_t *m_cups_dest = cupsGetNamedDest(CUPS_HTTP_DEFAULT, name, instance);


    if (!m_cups_dest) {
        qWarning() << "Could not find printer:" << m_name;
        return false;
    }

//    cups_dinfo_t *cap = cupsCopyDestInfo(CUPS_HTTP_DEFAULT, m_cups_dest);

//    qDebug() << "CAP" << (cap & CUPS_PRINTER_DUPLEX);

//    for (int i=0; i < m_cups_dest->num_options; i++) {
//        qDebug() << m_cups_dest->options[i].name << m_cups_dest->options[i].value;
//    }

    // CUPS_PRINTER_DUPLEX

    // Load options
    // Copies
    if (m_copies > 1) {
        __CUPS_ADD_OPTION(m_cups_dest, "copies", QString::number(m_copies).toLocal8Bit());
    }

    // Colour mode
    if (m_color_mode == GrayScale) {
        __CUPS_ADD_OPTION(m_cups_dest, "ColorModel", "CMYGray");
    }

    // Duplex
    if (m_duplex_supported && m_duplex) {
        if (doc->orientation() == QPrinter::Portrait) {
            __CUPS_ADD_OPTION(m_cups_dest, "Duplex", "DuplexNoTumble");
//            __CUPS_ADD_OPTION(m_cups_dest, "sides", "two-sided-long-edge")
        } else {
            __CUPS_ADD_OPTION(m_cups_dest, "Duplex", "DuplexTumble");
//            __CUPS_ADD_OPTION(m_cups_dest, "sides", "two-sided-long-edge")
        }
    } else {
        __CUPS_ADD_OPTION(m_cups_dest, "Duplex", "None");
    }

    // Orientation
    if (doc->orientation() == QPrinter::Landscape) {
        __CUPS_ADD_OPTION(m_cups_dest, "landscape", "");
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
    __CUPS_ADD_OPTION(m_cups_dest, "OutputMode", printQuality.toLocal8Bit());

    // Load title
    QString title = doc->title();

    if (title.isEmpty()) {
        title = doc->url().fileName();
    }

    // Send to the printer
    m_job_id = cupsPrintFile(m_cups_dest->name,
                             doc->url().toLocalFile().toLocal8Bit().data(),
                             title.toLocal8Bit().data(),
                             m_cups_dest->num_options, m_cups_dest->options);
    // TODO: PDF mode!
    if (m_pdf_mode) {
        Q_EMIT exportRequest(doc->url().toLocalFile());
    }

    if (m_job_id == 0) {
        qWarning() << "Creating Job Failed:" << cupsLastErrorString();
        return false;
    } else {
        qDebug() << "JobID" << m_job_id;
        return true;
    }
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
