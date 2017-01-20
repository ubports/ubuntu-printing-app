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
#include "document.h"

#include <QtCore/QDebug>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMimeDatabase>
#include <QtCore/QUrl>

#include <QtGui/QPainter>
#include <QtGui/QPaintDevice>

#include <cmath>

#include <poppler/qt5/poppler-qt5.h>

Document::Document(QObject *parent)
    : QObject(parent),
      m_count(0),
      m_document(Q_NULLPTR),
      m_url(QStringLiteral(""))
{

}

Document::~Document()
{
    if (m_document) {
        delete m_document;
    }
}

int Document::count() const
{
    return m_count;
}

float Document::getDpi(QSizeF sourceSizeF, QSizeF targetSizeF)
{
    // Convert pt size to inches
    QSizeF sourceSizeInch(sourceSizeF.width() / 72,
                          sourceSizeF.height() / 72);

    // Target size / source inch size = DPI to render at
    QSizeF destDPI(targetSizeF.width() / sourceSizeInch.width(),
                   targetSizeF.height() / sourceSizeInch.height());

    // Choose the shorter side
    return fmin(destDPI.width(), destDPI.height());
}

QSizeF Document::getPageSize(int pageNumber)
{
    QSizeF size;

    if (m_document) {
        Poppler::Page *page = m_document->page(pageNumber);

        if (page) {
            size = page->pageSizeF();
        }
    }

    return size;
}

QImage Document::makeImage(QSizeF size, int pageNumber)
{
    if (pageNumber < 0) {
        qWarning() << "Invalid page number";
        return QImage();
    }

    if (!m_document) {
        qWarning() << "No document loaded";
        return QImage();
    }

    if (size == QSize(0, 0)) {
        return QImage();
    }

    Poppler::Page *page = m_document->page(pageNumber);

    if (!page) {
        qWarning() << "Invalid page";
        return QImage();
    }

    // Calculate the DPI to render at
    float res = getDpi(page->pageSizeF(), size);

    // Make the image
    QImage image = page->renderToImage(res, res, 0, 0, size.width(), size.height());

    delete page;

    if (image.isNull()) {
        qWarning() << "Image is null";
        return QImage();
    }

    return image;
}

QImage Document::makeImageToFit(QSizeF size, int pageNumber, bool color)
{
    if (pageNumber < 0) {
        qWarning() << "Invalid page number";
        return QImage();
    }

    if (!m_document) {
        qWarning() << "No document loaded";
        return QImage();
    }

    Poppler::Page *page = m_document->page(pageNumber);

    if (!page) {
        qWarning() << "Invalid page";
        return QImage();
    }

    // Calculate scaler
    float scaleX = size.width() / page->pageSizeF().width();
    float scaleY = size.height() / page->pageSizeF().height();
    float scale = fmin(scaleX, scaleY);

    // Calculate the size of the content inside the container
    QSizeF scaledSize(scale * page->pageSizeF().width(), scale * page->pageSizeF().height());

    // Make the image
    QImage image = makeImage(scaledSize, pageNumber);

    if (!color) {
        image = image.convertToFormat(QImage::Format_Grayscale8);
    }

    // Make a container then put the content image inside
    QImage container(size.toSize(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter;

    container.fill(QColor(0, 0, 0, 0));

    painter.begin(&container);

    if (!image.isNull()) {
        painter.drawImage((size.width() - scaledSize.width()) / 2, (size.height() - scaledSize.height()) / 2, image);
    } else {
        qWarning() << "Image is null";
        return QImage();
    }

    painter.end();

    return container;
}

Document::Orientation Document::orientation() const
{
    if (!m_document) {
        return Portrait;
    }

    Poppler::Page *page = m_document->page(0);

    if (page) {
        QSizeF pageSize = page->pageSizeF();

        if (pageSize.width() > pageSize.height()) {
            return Landscape;
        } else {
            return Portrait;
        }

        // TODO: page orientation seems to be wrong?
//        qDebug() << "DOC" << page->orientation() << Portrait << Landscape;

//        if (page->orientation() == Poppler::Page::Landscape) {
//            return Landscape;
//        } else if (page->orientation() == Poppler::Page::Portrait) {
//            return Portrait;
//        } else {
//            qWarning() << "Other orientation:" << page->orientation() << "using Portrait";
//            return Portrait;
//        }
    } else {
        return Portrait;
    }
}

void Document::setUrl(QUrl url)
{
    if (m_url != url) {
        if (url.isLocalFile() && QFileInfo::exists(url.toLocalFile())) {
            if (QMimeDatabase().mimeTypeForUrl(url).name() == "application/pdf") {
                // Attempt to Load document
                m_document = Poppler::Document::load(url.toLocalFile());

                if (!m_document || m_document->isLocked()) {
                    qWarning() << "Invalid Document";
                    delete m_document;
                    m_document = Q_NULLPTR;

                    Q_EMIT error(Errors::ErrorDocumentInvalid);
                } else {
                    m_url = url;

                    m_document->setRenderBackend(Poppler::Document::SplashBackend);

                    m_document->setRenderHint(Poppler::Document::Antialiasing, true);
                    m_document->setRenderHint(Poppler::Document::TextAntialiasing, true);
                    m_document->setRenderHint(Poppler::Document::TextHinting, true);
                    m_document->setRenderHint(Poppler::Document::TextSlightHinting, true);
//                    m_document->setRenderHint(Poppler::Document::OverprintPreview, true);
//                    m_document->setRenderHint(Poppler::Document::ThinLineSolid, true);
//                    m_document->setRenderHint(Poppler::Document::ThinLineShape, true);

                    Q_EMIT urlChanged();

                    if (m_count != m_document->numPages()) {
                        m_count = m_document->numPages();

                        Q_EMIT countChanged();
                    }

                    Q_EMIT orientationChanged();
                    Q_EMIT titleChanged();
                }
            } else {
                qWarning() << "File is not a PDF:" << url;
                Q_EMIT error(Errors::ErrorNotPdf);
            }
        } else {
            qWarning() << "Url is not a local file:" << url;

            Q_EMIT error(Errors::ErrorNotFound);
        }
    }
}

QString Document::title() const
{
    if (m_document) {
        return m_document->info("Title");
    } else {
        return QStringLiteral("");
    }
}

QUrl Document::url() const
{
    return m_url;
}
