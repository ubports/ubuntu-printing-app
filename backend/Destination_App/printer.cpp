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
      m_paper_size("A4"),
      m_printer(Q_NULLPTR)
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

    // TODO: load settings for printer
    if (m_orientation == Qt::Orientation::Vertical) {
        m_printer->setOrientation(QPrinter::Orientation::Portrait);
    } else {
        m_printer->setOrientation(QPrinter::Orientation::Landscape);
    }

    QPainter painter(m_printer);

    for (int i=0; i < doc->count(); i++) {
        if (i != 0) {
            if (!m_printer->newPage()) {
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

void Printer::setName(QString name)
{
    if (m_name != name) {
        m_name = name;

        if (name == "PDF") {
            if (m_printer) {
                delete m_printer;
            }

            m_printer = new QPrinter(QPrinter::ScreenResolution);
            m_printer->setOutputFileName("/tmp/" + QDateTime::currentDateTime().toString(Qt::ISODate));
            m_printer->setOutputFormat(QPrinter::PdfFormat);
            qDebug() << "Loaded PDF printer:" << m_printer->outputFileName();
        } else {
            QPrinterInfo printerInfo = QPrinterInfo::printerInfo(m_name);

            if (!printerInfo.isNull()) {
                if (m_printer) {
                    delete m_printer;
                }

                m_printer = new QPrinter(printerInfo);  // FIXME: set high res here?
                qDebug() << "Loaded printer:" << m_name;
            } else {
                qWarning() << "No printer found:" << m_name;
            }
        }

        Q_EMIT nameChanged();
    }
}

void Printer::setOrientation(Qt::Orientation orientation)
{
    if (m_orientation != orientation) {
        m_orientation = orientation;

        Q_EMIT orientationChanged();
    }
}

void Printer::setPaperSize(QString paperSize)
{
    if (paperSize == "A4") {
        m_printer->setPaperSize(QPrinter::PaperSize::A4);
    } else if (paperSize == "A5") {
        m_printer->setPaperSize(QPrinter::PaperSize::A5);
    }
}

QSize Printer::size() const
{
    return QSize(m_printer->width(), m_printer->height());
}
