#ifndef POPPLERIMAGEPROVIDER_H
#define POPPLERIMAGEPROVIDER_H

#include <QtQuick/QQuickImageProvider>

#include <QDebug>
#include "document.h"

class PopplerImageProvider : public QQuickImageProvider
{
public:
    PopplerImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Image)
    {

    }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
        qDebug() << "ID" << id << requestedSize.height() << requestedSize.width();

        if (id.isEmpty()) {
            return QImage();
        }

        int width = 100;
        int height = 50;

        if (size)
            *size = QSize(width, height);

        Document *doc = new Document();
        doc->setUrl(id);

        return doc->renderImage(requestedSize, 0);
    }
};

#endif // POPPLERIMAGEPROVIDER_H
