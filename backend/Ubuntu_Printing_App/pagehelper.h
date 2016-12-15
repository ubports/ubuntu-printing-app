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
#ifndef PAGEHELPER_H
#define PAGEHELPER_H

#include <QtCore/QObject>

#include "document.h"

class PageHelper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double aspect READ aspect NOTIFY aspectChanged)
    Q_PROPERTY(Document *document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(int page READ page WRITE setPage NOTIFY pageChanged)
public:
    explicit PageHelper(QObject *parent = 0);
    double aspect() const;
    Document *document() const;
    int page() const;
signals:
    void aspectChanged();
    void documentChanged();
    void pageChanged();
private slots:
    void loadAspect();
    void setAspect(double aspect);
    void setDocument(Document *document);
    void setPage(int page);
private:
    double m_aspect;
    Document *m_document;
    int m_page;
};

#endif // PAGEGELPER_H
