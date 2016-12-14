#include "document.h"

#include <QtCore/QDebug>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMimeDatabase>
#include <QtCore/QUrl>

#include <QtGui/QPainter>
#include <QtGui/QPaintDevice>

#include <cmath>

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

float Document::getDpi(QSizeF sourceSizeF, QSizeF targetSizeF)
{
    // Convert pt size to inches
    QSizeF sourceSizeInch(sourceSizeF.width() / 72,
                          sourceSizeF.height() / 72);

    // Target size / source inch size = DPI to render at
    QSizeF destDPI(targetSizeF.width() / sourceSizeInch.width(),
                   targetSizeF.height() / sourceSizeInch.height());

    // Choose the shorter side
    return fmin(destDPI.width(), destDPI.height());
}

QImage Document::makeImage(QSizeF size, int pageNumber)
{
    if (pageNumber < 0) {
        qWarning() << "Invalid page number";
        return QImage();
    }

    if (!m_document) {
        qWarning() << "No document loaded";
        return QImage();
    }

    // Splash backend has much better rendering
    m_document->setRenderBackend(Poppler::Document::SplashBackend);

    Poppler::Page *page = m_document->page(pageNumber);

    if (!page) {
        qWarning() << "Invalid page";
        return QImage();
    }

    // Calculate the DPI to render at
    float res = getDpi(page->pageSizeF(), size);

    qDebug() << "Making image with res of" << res;

    // Make the image
    QImage image = page->renderToImage(res, res, 0, 0, size.width(), size.height());

    if (image.isNull()) {
        qWarning() << "Image is null";
    }

    delete page;

    return image;
}

QImage Document::makeImageToFit(QSizeF size, int pageNumber, bool color)
{
    if (pageNumber < 0) {
        qWarning() << "Invalid page number";
        return QImage();
    }

    if (!m_document) {
        qWarning() << "No document loaded";
        return QImage();
    }

    // Splash backend has much better rendering
    m_document->setRenderBackend(Poppler::Document::SplashBackend);

    Poppler::Page *page = m_document->page(pageNumber);

    if (!page) {
        qWarning() << "Invalid page";
        return QImage();
    }

    // Calculate scaler
    float scaleX = size.width() / page->pageSizeF().width();
    float scaleY = size.height() / page->pageSizeF().height();
    float scale = fmin(scaleX, scaleY);

    // Calculate the size of the content inside the container
    QSizeF scaledSize(scale * page->pageSizeF().width(), scale * page->pageSizeF().height());

    // Make the image
    QImage image = makeImage(scaledSize, pageNumber);
//    image.setDotsPerMeterX(6000);
//    image.setDotsPerMeterY(6000);

    if (!color) {
        image = image.convertToFormat(QImage::Format_Grayscale8);
    }

    qDebug() << "i" << image.width() << scaledSize.width() << image.dotsPerMeterX();

    // Make a container then put the content image inside
    QImage container(size.toSize(), QImage::Format_ARGB32_Premultiplied); //QImage::Format_Grayscale8); //QImage::Format_ARGB32);
    QPainter painter;

    container.fill(QColor(0, 0, 0, 0));

    painter.begin(&container);

    if (!image.isNull()) {
        painter.drawImage((size.width() - scaledSize.width()) / 2, (size.height() - scaledSize.height()) / 2, image);
    } else {
        qWarning() << "Image is null";
    }

    painter.end();

    return container;
}

QPixmap Document::makePixmap(QSize size, int pageNumber)
{
    if (pageNumber < 0) {
        qWarning() << "Invalid page number";
    }

    if (!m_document) {
        qWarning() << "No document loaded";
    }

    // Splash backend has much better rendering
    m_document->setRenderBackend(Poppler::Document::SplashBackend);

    Poppler::Page *page = m_document->page(pageNumber);

    if (!page) {
        qWarning() << "Invalid page";
    }

    float res = getDpi(page->pageSizeF(), size);

    qDebug() << "Making image with res of" << res;

    // Make QPixmap
    QPixmap pixmap(size);

    // If using ArthurBackend can do
    // QPainter painter(&pixmap);
    // if (!page->renderToPainter(painter, res, res)) {
    //     qWarning() << "Failed to render to painter";
    // }

    QImage image = page->renderToImage(res, res);

    if (image.isNull()) {
        qWarning() << "Image is null";
    }

    pixmap = QPixmap::fromImage(image);

    delete page;

    return pixmap;
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

    qDebug() << "Making image with res of" << res;

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

Document::Orientation Document::orientation() const
{
    Poppler::Page *page = m_document->page(0);

    if (page) {
        QSizeF pageSize = page->pageSizeF();

        if (pageSize.width() > pageSize.height()) {
            return Landscape;
        } else {
            return Portrait;
        }

        qDebug() << "DOC" << page->orientation() << Portrait << Landscape;

        if (page->orientation() == Poppler::Page::Landscape) {
            return Landscape;
        } else if (page->orientation() == Poppler::Page::Portrait) {
            return Portrait;
        } else {
            qWarning() << "Other orientation:" << page->orientation() << "using Portrait";
            return Portrait;
        }
    } else {
        return Portrait;
    }
}

bool Document::printFromImage(QPainter *painter, int pageNumber, QRect pageRect, double resolution)
{
    // Ensure we are using the right render hints
    m_document->setRenderBackend(Poppler::Document::SplashBackend);

    m_document->setRenderHint(Poppler::Document::Antialiasing, true);
    m_document->setRenderHint(Poppler::Document::TextAntialiasing, true);
    m_document->setRenderHint(Poppler::Document::TextHinting, true);

    Poppler::Page *page = m_document->page(pageNumber);

    if (!page) {
        qWarning() << "Invalid page!";
        return false;
    }

    // Render at given resolution
    QImage image = page->renderToImage(resolution, resolution);

    // Get the scalar for the image to fit within the pageRect
    double ratioX = (double) image.width() / (double) pageRect.width();
    double ratioY = (double) image.height() / (double) pageRect.height();

    image.setDevicePixelRatio(fmax(ratioX, ratioY));

    // Check that image isn't null
    if (image.isNull()) {
        return false;
    }

    // Draw the image to the painter, should we scale? the image should be correct aspect
    // Here we should check if the image aspect is different and position in the centre?
    painter->drawImage(0, 0, image);
    //painter->drawImage(QRect(QPoint(0, 0), QSize(pageRect.width(), pageRect.height())), image, image.rect());

    qDebug() << "Device Width:" << pageRect.width() << "LogicalDPI:" << painter->device()->logicalDpiX(); // << "RenderDPI:" << renderDPI.width();
    qDebug() << "RatioX" << ratioX << "RatioY" << ratioY;

    qDebug() << "PageRect:" << pageRect.width() << pageRect.height();
    qDebug() << "Image:" << image.width() << image.height() << image.dotsPerMeterX();

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

                if (!m_document || m_document->isLocked()) {
                    qWarning() << "Invalid Document";
                    delete m_document;
                    m_document = Q_NULLPTR;

                    Q_EMIT error(ErrorDocumentInvalid);
                } else {
                    m_document->setRenderBackend(Poppler::Document::ArthurBackend);

                    m_document->setRenderHint(Poppler::Document::Antialiasing, true);
                    m_document->setRenderHint(Poppler::Document::TextAntialiasing, true);
                    m_document->setRenderHint(Poppler::Document::TextHinting, true);
    //                m_document->setRenderHint(Poppler::Document::TextSlightHinting, true);
    //                m_document->setRenderHint(Poppler::Document::OverprintPreview, true);
    //                m_document->setRenderHint(Poppler::Document::ThinLineSolid, true);
    //                m_document->setRenderHint(Poppler::Document::ThinLineShape, true);

                    if (m_count != m_document->numPages()) {
                        m_count = m_document->numPages();

                        Q_EMIT countChanged();
                    }
                }

                Q_EMIT urlChanged();
            } else {
                qWarning() << "File is not a PDF:" << url;
                Q_EMIT error(ErrorNotPdf);
            }
        } else {
            qWarning() << "Url is not a local file:" << url;

            Q_EMIT error(ErrorNotFound);
        }
    }
}

QString Document::title() const
{
    if (m_document) {
        return m_document->info("Title");
    } else {
        return "";
    }
}

QUrl Document::url() const
{
    return m_url;
}
