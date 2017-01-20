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
#include <QtCore/QSizeF>

#include <poppler/qt5/poppler-qt5.h>

#include "pagehelper.h"

#define A4_ASPECT_RATIO 595.0 / 842.0

PageHelper::PageHelper(QObject *parent)
    : QObject(parent)
    , m_aspect(A4_ASPECT_RATIO)
    , m_document(Q_NULLPTR)
    , m_page(0)
{

}

double PageHelper::aspect() const
{
    return m_aspect;
}

Document *PageHelper::document() const
{
    return m_document;
}

void PageHelper::loadAspect()
{
    if (!m_document) {
        setAspect(A4_ASPECT_RATIO);
    }

    QSizeF pageSize = m_document->getPageSize(m_page);

    if (pageSize.isValid()) {
        setAspect(pageSize.width() / pageSize.height());
    } else {
        setAspect(A4_ASPECT_RATIO);
    }
}

int PageHelper::page() const
{
    return m_page;
}

void PageHelper::setAspect(double aspect)
{
    if (m_aspect != aspect) {
        m_aspect = aspect;

        Q_EMIT aspectChanged();
    }
}

void PageHelper::setDocument(Document *document)
{
    if (m_document != document) {
        if (m_document) {  // disconnect old signals
            m_document->disconnect();
        }

        m_document = document;

        // If the document URL changes reload the aspect
        PageHelper::connect(m_document, SIGNAL(urlChanged()), this, SLOT(loadAspect()));

        Q_EMIT documentChanged();

        loadAspect();
    }
}

void PageHelper::setPage(int page)
{
    if (m_page != page) {
        m_page = page;

        Q_EMIT pageChanged();

        loadAspect();
    }
}

