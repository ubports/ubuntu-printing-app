#include "printer.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QFileInfo>

#include <QtCore/QDebug>

#include <QtGui/QPainter>

#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrinterInfo>

//#include <cups/cups.h>

Printer::Printer(QObject *parent)
    : QObject(parent)
    , m_color_mode(Color)
    , m_copies(1)
    , m_duplex(false)
    , m_duplex_supported(false)
    , m_name("")
    , m_pdf_mode(false)
    , m_resolution(300)
{

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
//    int         num_dests;
//    cups_dest_t *dests;

//    num_dests = cupsGetDests(&dests);


//    dests++;

//    cups_dest_t *mydest;

//    mydest = cupsGetDest(dests->name, dests->instance, num_dests, dests);


//    qDebug() << "OP" << mydest->num_options;

//    cups_option_t *p = mydest->options;

//    for (int i=0; i < mydest->num_options; i++) {
//        qDebug() << "OP" << p->name << p->value;
//        p++;
//    }

//    int jobId = cupsPrintFile(dests->name, QUrl(doc->url()).toLocalFile().toStdString().c_str(), "Test Print!", mydest->num_options, mydest->options);

    QPrinter *printer = printerInstance();


    qDebug() << "Paper/Page" << printer->paperRect().width() << printer->pageRect().width();

    qDebug() << "Printer (assumed) Resolution:" << printer->resolution();

    qDebug() << "Printer PageRect" << printer->pageRect().x() << printer->pageRect().y() << printer->pageRect().width() << printer->pageRect().height();
    qDebug() << "Printer PaperRect" << printer->paperRect().x() << printer->paperRect().y() << printer->paperRect().width() << printer->paperRect().height();

    // TODO: load printer settings from PDF file ?

    if (m_pdf_mode) {
        printer->setOutputFileName(makeOutputFilepath());
    }

    if (m_color_mode == Printer::Color) {
        printer->setColorMode(QPrinter::Color);
    } else if (m_color_mode == Printer::GrayScale) {
        printer->setColorMode(QPrinter::GrayScale);
    } else {
        qWarning() << "Unknown color mode";
    }

    if (m_duplex_supported && m_duplex) {
        printer->setDuplex(QPrinter::DuplexAuto);
    } else {
        printer->setDuplex(QPrinter::DuplexNone);
    }

    printer->setCopyCount(m_copies);
    printer->setOrientation(doc->orientation());
    printer->setResolution(resolution());

    // TODO: implement this case
    if (!printer->supportsMultipleCopies()) {
        qWarning() << "Does not support multiple copies, will need to manually loop.";
    }

    QPainter painter(printer);

    for (int i=0; i < doc->count(); i++) {
        if (i != 0) {
            if (!printer->newPage()) {
                qWarning() << "Unable to create newPage in printer";
                return false;
            }
        }

        if (!doc->printFromImage(&painter, i, printer->pageRect(), printer->resolution())) {
            qWarning() << "Unable to render page to printer";
            return false;
        }
    }

    if (m_pdf_mode) {
        Q_EMIT exportRequest(printer->outputFileName());
    }

    return true;
}

QPrinter *Printer::printerInstance()
{
    QPrinter *printer = Q_NULLPTR;

    if (m_pdf_mode) {
        printer = new QPrinter(QPrinter::ScreenResolution);

        printer->setOutputFormat(QPrinter::PdfFormat);

        qDebug() << "Loaded PDF printer:" << printer->outputFileName();
    } else {
        QPrinterInfo printerInfo = QPrinterInfo::printerInfo(m_name);

        if (!printerInfo.isNull()) {
            printer = new QPrinter(printerInfo);  // FIXME: set high res here?

            qDebug() << "Loaded printer:" << m_name;
        } else {
            qWarning() << "No printer found:" << m_name;
        }
    }

    return printer;
}

int Printer::resolution() const
{
    return m_resolution;
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

void Printer::setResolution(int resolution)
{
    if (m_resolution != resolution) {
        m_resolution = resolution;

        Q_EMIT resolutionChanged();
    }
}
