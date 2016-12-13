#include "popplerimageprovider.h"

#include <QDebug>

#include "document.h"
#include "printer.h"

PopplerImageProvider::PopplerImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image, QQuickImageProvider::ForceAsynchronousImageLoading)
{

}


QImage PopplerImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << "ID" << id << requestedSize.height() << requestedSize.width();

    // Load id ( image://poppler/{pageNumber}/{colorMode}/{filePath} )
    QStringList parts = id.split("/");

    bool colorOk, numberOk;
    int pageNumber = parts.takeFirst().toInt(&numberOk, 10);
    int colorModeInt = parts.takeFirst().toInt(&colorOk, 10);

    Printer::ColorMode colorMode = static_cast<Printer::ColorMode>(colorModeInt);

    QString url = parts.join("/");

    if (url.isEmpty() || !numberOk || !colorOk) {
        return QImage();
    }

    // Fallback size if none is given for QImage
    int width = 100;
    int height = 50;

    if (size)
        *size = QSize(width, height);

    qDebug() << "Url:" << url;
    qDebug() << "Page:" << pageNumber;

    Document *doc = new Document();
    doc->setUrl(url);

    return doc->makeImageToFit(requestedSize, pageNumber, colorMode == Printer::Color);
}
