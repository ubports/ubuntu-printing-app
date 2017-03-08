/*
 * Copyright 2016 Canonical Ltd.
 *
 * This file is part of ubuntu-printing-app.
 *
 * ubuntu-printing-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * ubuntu-printing-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored-by: Andrew Hayzen <andrew.hayzen@canonical.com>
 */
#include "popplerimageprovider.h"

#include <QDebug>

#include "document.h"

PopplerImageProvider::PopplerImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image, QQuickImageProvider::ForceAsynchronousImageLoading)
{

}


QImage PopplerImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    // Load id ( image://poppler/{pageNumber}/{color}/{filePath} )
    QStringList parts = id.split("/");

    bool colorOk, numberOk;
    int pageNumber = parts.takeFirst().toInt(&numberOk, 10);
    QString colorMode = QString::fromStdString(parts.takeFirst().toStdString());

    bool color;

    if (colorMode == "true") {
        color = true;
        colorOk = true;
    } else if (colorMode == "false") {
        color = false;
        colorOk = true;
    } else {
        colorOk = false;
    }

    QString url = parts.join("/");

    // Check request size is valid
    if (requestedSize.height() <= 0 || requestedSize.width() <= 0) {
        qWarning() << "Invalid requestedSize given" << requestedSize;

        // Requested size is invalid, return a 1x1 image to prevent QML errors
        // when resizing the window really small
        if (size) {
            *size = QSize(1, 1);
        }

        return QImage(QSize(1, 1), QImage::Format_ARGB32_Premultiplied);
    } else if (size) {
        // Set the size of the 'orignal' image to the requestedSize
        // as we generate an image dynamically
        *size = requestedSize;
    }

    // Check all parts given were OK
    if (url.isEmpty() || !numberOk || !colorOk) {
        qWarning() << "Invalid id given to poppler image provider:" << id;
        return QImage();
    }

    // Build a Document and generate an image to fit the given requestedSize
    Document *doc = new Document();
    doc->setUrl(url);

    QImage image = doc->makeImageToFit(requestedSize, pageNumber, color);

    doc->deleteLater();

    return image;
}
