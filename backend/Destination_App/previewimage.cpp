#include "previewimage.h"

PreviewImage::PreviewImage(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{

}

Document *PreviewImage::document() const
{
    return m_document;
}

void PreviewImage::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);

    painter->fillRect(boundingRect(), QColor(255, 0, 0));

    QPixmap doc(QSize(100, 100));
    QPainter docPainter(&doc);

    m_document->renderPage(&docPainter, 0);

    painter->drawPixmap(20, 20, doc);
}

void PreviewImage::setDocument(Document *document)
{
    if (m_document != document) {
        m_document = document;

        Q_EMIT documentChanged();
    }
}
