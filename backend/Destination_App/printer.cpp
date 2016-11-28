#include "printer.h"

#include <QtCore/QDateTime>

#include <QtCore/QDebug>

#include <QtGui/QPainter>

#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrinterInfo>

#include <cups/cups.h>

Printer::Printer(QObject *parent)
    : QObject(parent),
      m_name(""),
      m_paper_size("A4")
{

}

QString Printer::name() const
{
    return m_name;
}

Qt::Orientation Printer::orientation() const
{
    return m_orientation;
}

QString Printer::paperSize() const
{
    return m_paper_size;
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

    QPainter painter(printer);

    for (int i=0; i < doc->count(); i++) {
        if (i != 0) {
            if (!printer->newPage()) {
                qWarning() << "Unable to create newPage in printer";
                return false;
            }
        }

        if (!doc->renderPage(&painter, i)) {
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

//    printer->setPageMargins(1, 1, 1, 1, QPrinter::Inch);

    // TODO: load settings for printer
    if (m_orientation == Qt::Orientation::Vertical) {
        printer->setOrientation(QPrinter::Orientation::Portrait);
    } else {
        printer->setOrientation(QPrinter::Orientation::Landscape);
    }

    if (m_paper_size == "A4") {
        printer->setPaperSize(QPrinter::PaperSize::A4);
    } else if (m_paper_size == "A5") {
        printer->setPaperSize(QPrinter::PaperSize::A5);
    }

    return printer;
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

void Printer::setOrientation(Qt::Orientation orientation)
{
    if (m_orientation != orientation) {
        m_orientation = orientation;

        Q_EMIT orientationChanged();
        Q_EMIT settingsChanged();
    }
}

void Printer::setPaperSize(QString paperSize)
{
    if (m_paper_size != paperSize) {
        m_paper_size = paperSize;

        Q_EMIT paperSizeChanged();
        Q_EMIT settingsChanged();
    }
}

//QSize Printer::size() const
//{
//    QPrinter *printer = printerInstance();
//    return QSize(printer->width(), printer->height());
//}
