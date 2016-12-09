#ifndef POPPLERIMAGEPROVIDER_H
#define POPPLERIMAGEPROVIDER_H

#include <QtQuick/QQuickImageProvider>

#include <QDebug>
#include "document.h"
#include "printer.h"


#include <QThread>

class PopplerImageProvider : public QQuickImageProvider
{
public:
    PopplerImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Image, QQuickImageProvider::ForceAsynchronousImageLoading)
    {

    }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
        qDebug() << "ID" << id << requestedSize.height() << requestedSize.width();

        QStringList parts = id.split("/");

        bool colorOk, numberOk;
        int pageNumber = parts.takeFirst().toInt(&numberOk, 10);
        int colorModeInt = parts.takeFirst().toInt(&colorOk, 10);

        Printer::ColorMode colorMode = static_cast<Printer::ColorMode>(colorModeInt);

        QString url = parts.join("/");

        if (url.isEmpty() || !numberOk || !colorOk) {
            return QImage();
        }

        int width = 100;
        int height = 50;

        if (size)
            *size = QSize(width, height);

        qDebug() << "Url:" << url;
        qDebug() << "Page:" << pageNumber;

        Document *doc = new Document();
        doc->setUrl(url);

        return doc->makeImageToFit(requestedSize, pageNumber, colorMode == Printer::ColorMode::Color);
    }
};

#endif // POPPLERIMAGEPROVIDER_H
