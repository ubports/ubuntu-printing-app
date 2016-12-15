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
        qWarning() << "Invalid id given to poppler image provider:" << id;
        return QImage();
    }

    // Check request size is valid
    if (requestedSize.height() <= 0 || requestedSize.width() <= 0) {
        return QImage(QSize(1, 1), QImage::Format_ARGB32_Premultiplied);
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

    QImage image = doc->makeImageToFit(requestedSize, pageNumber, colorMode == Printer::Color);

    doc->deleteLater();

    return image;
}
