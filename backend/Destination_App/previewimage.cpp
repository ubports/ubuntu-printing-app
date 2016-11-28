#include "previewimage.h"

PreviewImage::PreviewImage(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_document(Q_NULLPTR),
      m_printer(Q_NULLPTR)
{

}

Document *PreviewImage::document() const
{
    return m_document;
}

Printer *PreviewImage::printer() const
{
    return m_printer;
}

void PreviewImage::paint(QPainter *painter)
{
    if (!m_document || !m_printer) {
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);

    // Size of page and paper in pixels
    QSizeF paperSize = m_printer->printerInstance()->paperRect(QPrinter::DevicePixel).size();
    QSizeF pageSize = m_printer->printerInstance()->pageRect(QPrinter::DevicePixel).size();

    qDebug() << "Paper" << paperSize << "Page" << pageSize;

    float scaleX = width() / paperSize.width();
    float scaleY = height() / paperSize.height();
    float scale = std::fmin(scaleX, scaleY);

//    qDebug() << "Scale" << scaleX << scaleY << scale;

    paperSize.setHeight(paperSize.height() * scale);
    paperSize.setWidth(paperSize.width() * scale);

    pageSize.setHeight(pageSize.height() * scale);
    pageSize.setWidth(pageSize.width() * scale);

    // Scale to fit inside PreviewImage
//    paperSize.scale(width(), height(), Qt::KeepAspectRatio);
//    pageSize.scale(width(), height(), Qt::KeepAspectRatio);

    // Draw background
//    painter->fillRect(boundingRect(), QColor(200, 200, 200));

    // Draw paper
    painter->fillRect(QRect(QPoint(0, 0), paperSize.toSize()), QColor(255, 255, 255));

//    qDebug() << paperSize.height() << pageSize.height();
//    qDebug() << "Draw" << (paperSize.width() - pageSize.width()) / 2 << (paperSize.height() - pageSize.height()) / 2;

    // Draw page
    QImage docImage = m_document->makeImage(pageSize, 0);

    if (!docImage.isNull()) {
        painter->drawImage((paperSize.width() - pageSize.width()) / 2, (paperSize.height() - pageSize.height()) / 2, docImage);
    }
}

void PreviewImage::setDocument(Document *document)
{
    if (m_document != document) {
        m_document = document;

        connect(m_document, SIGNAL(urlChanged()), this, SLOT(update()));

        Q_EMIT documentChanged();
    }
}

void PreviewImage::setPrinter(Printer *printer)
{
    if (m_printer != printer) {
        m_printer = printer;

        connect(m_printer, SIGNAL(settingsChanged()), this, SLOT(update()));

        Q_EMIT printerChanged();
    }
}
