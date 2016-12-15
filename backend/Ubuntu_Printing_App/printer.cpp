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
#include "printer.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <QtCore/QDebug>

#include <cups/cups.h>
#include <cups/ppd.h>

#define __CUPS_ADD_OPTION(dest, name, value) dest->num_options = \
    cupsAddOption(name, value, dest->num_options, &dest->options);

Printer::Printer(QObject *parent)
    : QObject(parent)
    , m_color_mode(Color)
    , m_copies(1)
    , m_cups_dest(0)
    , m_cups_instance(QStringLiteral(""))
    , m_cups_name(QStringLiteral(""))
    , m_duplex(false)
    , m_duplex_modes({})
    , m_duplex_supported(false)
    , m_job_id(0)
    , m_name("")
    , m_ppd(0)
    , m_pdf_mode(false)
    , m_quality(Normal)
{

}

Printer::~Printer()
{
    if (m_ppd) {
        ppdClose(m_ppd);
        m_ppd = 0;
    }

    if (m_cups_dest) {
        cupsFreeDests(1, m_cups_dest);

        m_cups_dest = 0;
    }
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

void Printer::loadDuplexModes(const QString name)
{
    if (m_ppd) {
        ppdClose(m_ppd);
        m_ppd = 0;
    }

    const char *ppdFile = cupsGetPPD(name.toLocal8Bit().data());

    if (ppdFile) {
        m_ppd = ppdOpenFile(ppdFile);
        unlink(ppdFile);

        if (m_ppd) {
            ppdMarkDefaults(m_ppd);
        } else {
            m_ppd = 0;
            return;
        }
    }

    ppd_option_t *duplexModes = ppdFindOption(m_ppd, "Duplex");
    ppd_option_t *defaultDuplexModes = ppdFindOption(m_ppd, "DefaultDuplex");

    // Build list of supported duplex modes
    bool duplexSupported = false;
    m_duplex_modes.clear();

    for (int i=0; i < duplexModes->num_choices; i++) {
        m_duplex_modes.append(QString::fromLocal8Bit(duplexModes->choices[i].choice));

        if (QString::fromLocal8Bit(duplexModes->choices[i].choice) != "None") {
            duplexSupported = true;
        }
    }

    setDuplexSupported(duplexSupported);

    // Load default duplex mode
    // TODO: do we need as option gets disable?
    if (defaultDuplexModes) {
        if (QString::fromLocal8Bit(defaultDuplexModes->choices[0].choice) == "None") {
            setDuplex(false);
        } else {
            setDuplex(true);
        }
    }
}

bool Printer::loadOptions(cups_dest_t *cups_dest, Document *doc)
{
    // Load options
    // Copies
    if (m_copies > 1) {
        __CUPS_ADD_OPTION(cups_dest, "copies", QString::number(m_copies).toLocal8Bit());
    }

    // Colour mode
    if (m_color_mode == GrayScale) {
        __CUPS_ADD_OPTION(cups_dest, "ColorModel", "CMYGray");
    }

    // Duplex
    if (m_duplex_supported && m_duplex) {
        if (doc->orientation() == Document::Portrait && m_duplex_modes.contains("DuplexNoTumble")) {
            __CUPS_ADD_OPTION(cups_dest, "Duplex", "DuplexNoTumble");
//            __CUPS_ADD_OPTION(m_cups_dest, "sides", "two-sided-long-edge")
        } else if (doc->orientation() == Document::Portrait && m_duplex_modes.contains("DuplexTumble")) {
            __CUPS_ADD_OPTION(cups_dest, "Duplex", "DuplexTumble");
//            __CUPS_ADD_OPTION(m_cups_dest, "sides", "two-sided-long-edge")
        } else {
            // Fallback to pick first found duplex mode
            for (QString mode : m_duplex_modes) {
                if (mode != "None") {
                    __CUPS_ADD_OPTION(cups_dest, "Duplex", mode.toLocal8Bit().data());
                    break;
                }
            }
        }
    } else {
        __CUPS_ADD_OPTION(cups_dest, "Duplex", "None");
    }

    // Orientation
    if (doc->orientation() == Document::Landscape) {
        __CUPS_ADD_OPTION(cups_dest, "landscape", "");
    }

    // Page range
//    __CUPS_ADD_OPTION(cups_dest, "number-up", "4");  // 1, 2, 4, 6, 9
//    __CUPS_ADD_OPTION(cups_dest, "number-up-layout", "lrtb");  // "lrtb", "lrbt", "rlbt", "rltb", "btlr", "btrl", "tblr", "tbrl"
//    __CUPS_ADD_OPTION(cups_dest, "page-ranges", "1-2");  // 3-5,7,10-20

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
    __CUPS_ADD_OPTION(cups_dest, "OutputMode", printQuality.toLocal8Bit());

    return true;
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
    // TODO: PDF mode!
    if (m_pdf_mode) {
        Q_EMIT exportRequest(doc->url().toLocalFile());

//        cupsTempFd();
        return true;
    }

    // Get all the dests
    int         num_dests;  Q_UNUSED(num_dests)
    cups_dest_t *dests;

    num_dests = cupsGetDests(&dests);  // FIXME: appear to need to run this first?

    for (int i=0; i < num_dests; i++) {
        qDebug() << "Dest" << dests[i].name;
    }

    cupsFreeDests(num_dests, dests);

    // Ensure the destination is free
    if (m_cups_dest) {
        cupsFreeDests(1, m_cups_dest);

        m_cups_dest = 0;
    }

    // Get the destination
    // TODO: IPP, cupsGetDestWithURI()
    m_cups_dest = cupsGetNamedDest(CUPS_HTTP_DEFAULT, m_cups_name.toLocal8Bit().data(), m_cups_instance.toLocal8Bit().data());

    if (!m_cups_dest) {
        qWarning() << "Could not find printer:" << m_name;
        return false;
    }

    // Load the printer options
    loadOptions(m_cups_dest, doc);

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
            // Extract the cups name and instance from the m_name
            QStringList split = m_name.split("/");

            m_cups_name = split.takeFirst();
            m_cups_instance = split.isEmpty() ? "" : split.takeFirst();

            if (split.length() > 0) {
                qWarning() << "Unknown printer name pattern:" << m_name;
                return;
            }

            loadDuplexModes(m_cups_name);
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
