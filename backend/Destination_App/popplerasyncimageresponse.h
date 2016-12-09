#ifndef POPPLERASYNCIMAGERESPONSE_H
#define POPPLERASYNCIMAGERESPONSE_H

#include <QtCore/QRunnable>
#include <QtCore/QString>
#include <QtQuick/QQuickImageResponse>

class PopplerAsyncImageResponse : public QQuickImageResponse, public QRunnable
{
public:
    PopplerAsyncImageResponse(const QString &id, const QSize &requestedSize);

    QQuickTextureFactory *textureFactory() const;

    void run();
private:
    QString m_id;
    QSize m_requestedSize;
    QQuickTextureFactory *m_texture;
};

#endif // POPPLERASYNCIMAGERESPONSE_H
