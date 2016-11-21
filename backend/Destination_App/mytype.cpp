#include "mytype.h"

#include <QtCore/QFile>
#include <QtCore/qmimedatabase.h>
#include <QDebug>

#include <QPainter>
#include <QPrinterInfo>
#include <QPrinter>

#include <QUrl>

#include <poppler-qt5.h>

MyType::MyType(QObject *parent) :
    QObject(parent),
    m_message(""),
    m_url("")
{

}

MyType::~MyType() {

}

void MyType::setUrl(QString url)
{
    if (url != m_url) {
        m_url = QUrl(url).toLocalFile();

        Q_EMIT urlChanged();
    }
}

void MyType::printType()
{
    QFile file(m_url);

    qDebug() << "File exists:" << file.exists();
    qDebug() << "MimeType:" << QMimeDatabase().mimeTypeForFile(m_url) << QMimeDatabase().mimeTypeForFile(m_url).name();
    qDebug() << "Open:" << file.open(QFile::ReadOnly);

    Poppler::Document *document = Poppler::Document::load(m_url);
    document->setRenderBackend(Poppler::Document::ArthurBackend);

    if (!document || document->isLocked()) {
        qDebug() << "Invalid document";
        delete document;
        return;
    }

    QPrinter printer(QPrinterInfo::defaultPrinter());
    printer.setPaperSize(QPrinter::A4);

    qDebug() << "Printers" << QPrinterInfo::availablePrinterNames();
    qDebug() << "Default Printer" << QPrinterInfo::defaultPrinterName();

    QPainter painter(&printer);

    for (int i=0; i < document->numPages(); i++) {
        if (i != 0) {
            printer.newPage();
        }

        Poppler::Page *page = document->page(i);

        if (page == 0) {
            qDebug() << "Invalid page:" << i;
            return;
        }

        if (!page->renderToPainter(&painter)) {
            qDebug() << "Rendering failed";
            return;
        }

        qDebug() << "Rendered page!";
        delete page;
    }

    delete document;
}
