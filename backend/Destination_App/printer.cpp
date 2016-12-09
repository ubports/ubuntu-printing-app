#include "printer.h"

#include <QtCore/QDateTime>

#include <QtCore/QDebug>

#include <QtGui/QPainter>

#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrinterInfo>

//#include <cups/cups.h>

Printer::Printer(QObject *parent)
    : QObject(parent),
      m_color_mode(Printer::ColorMode::Color),
      m_copies(1),
      m_name(""),
      m_resolution(300)
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

QString Printer::name() const
{
    return m_name;
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

    printer->setResolution(resolution());

    qDebug() << "Paper/Page" << printer->paperRect().width() << printer->pageRect().width();

    qDebug() << "Printer (assumed) Resolution:" << printer->resolution();

    qDebug() << "Printer PageRect" << printer->pageRect().x() << printer->pageRect().y() << printer->pageRect().width() << printer->pageRect().height();
    qDebug() << "Printer PaperRect" << printer->paperRect().x() << printer->paperRect().y() << printer->paperRect().width() << printer->paperRect().height();

    // TODO: load printer settings from PDF file ?

    if (m_color_mode == Printer::ColorMode::Color) {
        printer->setColorMode(QPrinter::ColorMode::Color);
    } else if (m_color_mode == Printer::ColorMode::GrayScale) {
        printer->setColorMode(QPrinter::ColorMode::GrayScale);
    } else {
        qWarning() << "Unknown color mode";
    }

    printer->setCopyCount(m_copies);
    printer->setOrientation(doc->orientation());

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

    return true;
}

QPrinter *Printer::printerInstance()
{
    QPrinter *printer = Q_NULLPTR;

    if (m_name == "PDF") {
        printer = new QPrinter(QPrinter::ScreenResolution);
        printer->setOutputFileName("/tmp/" + QDateTime::currentDateTime().toString(Qt::ISODate));
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

void Printer::setName(QString name)
{
    if (m_name != name) {
        m_name = name;

        if (name != "PDF" && QPrinterInfo::printerInfo(m_name).isNull()) {
            qWarning() << "No printer found:" << m_name;
        }

        Q_EMIT nameChanged();
        Q_EMIT settingsChanged();
    }
}

void Printer::setResolution(int resolution)
{
    if (m_resolution != resolution) {
        m_resolution = resolution;

        Q_EMIT resolutionChanged();
    }
}
