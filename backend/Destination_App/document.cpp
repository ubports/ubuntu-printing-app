#include "document.h"

#include <QtCore/QDebug>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMimeDatabase>
#include <QtCore/QUrl>

#include <QtGui/QPainter>
#include <QtGui/QPaintDevice>

#include <poppler/qt5/poppler-qt5.h>

Document::Document(QObject *parent)
    : QObject(parent),
      m_count(0),
      m_document(Q_NULLPTR),
      m_url("")
{

}

Document::~Document()
{
    if (m_document) {
        delete m_document;
    }
}

int Document::count() const
{
    return m_count;
}

QImage Document::renderImage(QSize size, int pageNumber)
{
    if (pageNumber < 0) {
        qWarning() << "Invalid page number";
    }

    if (!m_document) {
        qWarning() << "No document loaded";
    }

//    m_document->setRenderBackend(Poppler::Document::SplashBackend);

    Poppler::Page *page = m_document->page(pageNumber);

    if (!page) {
        qWarning() << "Invalid page";
    }

    /*
     * width / res    width / res
     */

//    int res = size.width() / (page->pageSize().width() / 72);

//    qDebug() << "Res:" << res;



    QSizeF sourceSizeInch(page->pageSizeF().width() / 72, page->pageSizeF().height() / 72);

    QSizeF destDPI(size.width() / sourceSizeInch.width(), size.height()/ sourceSizeInch.height());

    float res;

    if (destDPI.width() > destDPI.height()) {
        res = destDPI.height();
    } else {
        res = destDPI.width();
    }



    QImage image = page->renderToImage(res, res);

    if (image.isNull()) {
        qWarning() << "Image is null";
    }

    delete page;

    return image;
}

bool Document::renderPage(QPainter *painter, int pageNumber)
{
    if (!painter) {
        qWarning() << "Invalid painter";
        return false;
    }

    if (pageNumber < 0) {
        qWarning() << "Invalid page number";
        return false;
    }

    if (!m_document) {
        qWarning() << "No document loaded";
        return false;
    }

    Poppler::Page *page = m_document->page(pageNumber);

    m_document->setRenderBackend(Poppler::Document::ArthurBackend);

    if (!page) {
        qWarning() << "Invalid page";
        return false;
    }

    /*
A4
DPI: 96 1200
Device Size: 767 1097
Doc Size: 595 842

A5
DPI: 96 1200
Device Size: 534 767
Doc Size: 595 842
    */

    QSizeF sourceSizeInch(page->pageSizeF().width() / 72, page->pageSizeF().height() / 72);

    QSizeF destDPI(painter->device()->width() / sourceSizeInch.width(), painter->device()->height()/ sourceSizeInch.height());

    float res;

    if (destDPI.width() > destDPI.height()) {
        res = destDPI.height();
    } else {
        res = destDPI.width();
    }

//    Poppler::PSConverter *psConv = m_document->psConverter();

//    psConv->setLeftMargin(5);
//    psConv->setRightMargin(1);
//    psConv->setOutputFileName("/tmp/out.ps");
//    psConv->setPaperHeight(painter->device()->height());
//    psConv->setPaperWidth(painter->device()->width());

//    QList<int> pages = { 1 };

//    psConv->setPageList(pages);

//    qDebug() << "PS" << psConv->convert();


//    float res = painter->device()->width() / (page->pageSize().width() / 72);  //painter->device()->logicalDpiX());

    //res = 70;

    qDebug() << "DPI:" << painter->device()->logicalDpiX() << painter->device()->physicalDpiX() << res;

    qDebug() << "Device Size:" << painter->device()->width() << painter->device()->height();
    qDebug() << "Doc Size:" << page->pageSize().width() << page->pageSize().height();

    // Create Image same dimensions as PageSize
    // Render to the image
    // Render the image to the printer
    // Image can also be used for preview?

    if (!page->renderToPainter(painter, res, res)) {
//    if (!page->renderToPainter(painter, painter->device()->logicalDpiX(), painter->device()->logicalDpiY(),
//                               0, 0, page->pageSize().width(), page->pageSize().height())) {
        qWarning() << "Rendering failed";
        return false;
    }

    delete page;

    return true;
}

void Document::setUrl(QUrl url)
{
    if (m_url != url) {
        if (url.isLocalFile() && QFileInfo::exists(url.toLocalFile())) {
            if (QMimeDatabase().mimeTypeForUrl(url).name() == "application/pdf") {
                m_url = url;

                // Load document
                m_document = Poppler::Document::load(m_url.toLocalFile());
                m_document->setRenderBackend(Poppler::Document::ArthurBackend);

                m_document->setRenderHint(Poppler::Document::Antialiasing, true);
                m_document->setRenderHint(Poppler::Document::TextAntialiasing, true);
                m_document->setRenderHint(Poppler::Document::TextHinting, true);
                m_document->setRenderHint(Poppler::Document::TextSlightHinting, true);
                m_document->setRenderHint(Poppler::Document::OverprintPreview, true);
                m_document->setRenderHint(Poppler::Document::ThinLineSolid, true);
                m_document->setRenderHint(Poppler::Document::ThinLineShape, true);

                if (!m_document || m_document->isLocked()) {
                    qWarning() << "Invalid Document";
                    delete m_document;
                    m_document = Q_NULLPTR;
                } else {
                    if (m_count != m_document->numPages()) {
                        m_count = m_document->numPages();

                        Q_EMIT countChanged();
                    }
                }

                Q_EMIT urlChanged();
            } else {
                qWarning() << "File is not a PDF:" << url;
            }
        } else {
            qWarning() << "Url is not a local file:" << url;
        }
    }
}

QUrl Document::url() const
{
    return m_url;
}
