#include "popplerasyncimageresponse.h"

#include <QtCore/QDebug>

#include "document.h"

PopplerAsyncImageResponse::PopplerAsyncImageResponse(const QString &id, const QSize &requestedSize)
    : m_id(id),
      m_requestedSize(requestedSize),
      m_texture(0)
{
    setAutoDelete(false);
}

QQuickTextureFactory *PopplerAsyncImageResponse::textureFactory() const
{
    return m_texture;
}

void PopplerAsyncImageResponse::run()
{
    qDebug() << "ID" << m_id << m_requestedSize.height() << m_requestedSize.width();

    QStringList parts = m_id.split("/");

    bool numberOk;
    int pageNumber = parts.takeFirst().toInt(&numberOk, 10);
    QString url = parts.join("/");

    if (url.isEmpty() || !numberOk) {
        m_texture = QQuickTextureFactory::textureFactoryForImage(QImage());

        Q_EMIT finished();
    } else {
        qDebug() << "Url:" << url;
        qDebug() << "Page:" << pageNumber;

        Document *doc = new Document();
        doc->setUrl(url);

        m_texture = QQuickTextureFactory::textureFactoryForImage(doc->makeImage(m_requestedSize, pageNumber));

        Q_EMIT finished();
    }

}
