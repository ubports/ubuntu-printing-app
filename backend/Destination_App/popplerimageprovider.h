#ifndef POPPLERIMAGEPROVIDER_H
#define POPPLERIMAGEPROVIDER_H

#include <QtQuick/QQuickImageProvider>

class PopplerImageProvider : public QQuickImageProvider
{
public:
    PopplerImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // POPPLERIMAGEPROVIDER_H
