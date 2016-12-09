#ifndef POPPLERASYNCIMAGEPROVIDER_H
#define POPPLERASYNCIMAGEPROVIDER_H

#include <QtCore/QThreadPool>
#include <QtQuick/QQuickAsyncImageProvider>

class PopplerAsyncImageProvider : public QQuickAsyncImageProvider
{
public:
    PopplerAsyncImageProvider();
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize);
private:
    QThreadPool pool;
};

#endif // POPPLERASYNCIMAGEPROVIDER_H
