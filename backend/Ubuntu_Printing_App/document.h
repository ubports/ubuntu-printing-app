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
#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QtCore/QObject>
#include <QtCore/QSizeF>
#include <QtCore/QUrl>

#include <QtGui/QPainter>

#include <poppler/qt5/poppler-qt5.h>

class Document : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(Orientation orientation READ orientation NOTIFY orientationChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
public:
    enum class Errors {
        ErrorDocumentInvalid,
        ErrorNotFound,
        ErrorNotPdf,
    };

    Q_ENUM(Errors)

    enum Orientation {
        Portrait,
        Landscape,
    };
    Q_ENUM(Orientation)

    explicit Document(QObject *parent = 0);
    ~Document();
    int count() const;
    QSizeF getPageSize(int pageNumber);
    Orientation orientation() const;
    QString title() const;
    QUrl url() const;

    QImage makeImage(QSizeF size, int pageNumber);
    QImage makeImageToFit(QSizeF size, int pageNumber, bool color);
signals:
    void countChanged();
    void error(Document::Errors errorType);
    void orientationChanged();
    void titleChanged();
    void urlChanged();
public slots:
    void setUrl(QUrl url);
private:
    float getDpi(QSizeF sourceSizeF, QSizeF targetSizeF);

    int m_count;
    Poppler::Document *m_document;
    QUrl m_url;
};

#endif // DOCUMENT_H
