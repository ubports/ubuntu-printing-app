/*
 * Copyright 2017 Canonical Ltd.
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

#include <QDebug>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "UbuntuPrintingApp/popplerimageprovider.h"

class TestPopplerImageProvider : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init()
    {
        m_provider = new PopplerImageProvider();
    }
    void cleanup()
    {
        delete m_provider;
    }
    QUrl getResourceUrl(const QString &resource) const
    {
        return QUrl("file://" + QDir::currentPath() + "/../../resources/pdf/" + resource);
    }

    void testRequestImage()
    {
        QString id = "0/true/" + getResourceUrl("color.pdf").toString();
        QSize *size = new QSize();
        QSize requestedSize(250, 250);

        QImage result = m_provider->requestImage(id, size, requestedSize);

        QCOMPARE(result.isNull(), false);
        QCOMPARE(result.size(), requestedSize);
        QCOMPARE(*size, requestedSize);
        QCOMPARE(result.isGrayscale(), false);
    }
    void testRequestImageGrayscale()
    {
        QString id = "0/false/" + getResourceUrl("color.pdf").toString();
        QSize *size = new QSize();
        QSize requestedSize(250, 250);

        QImage result = m_provider->requestImage(id, size, requestedSize);

        QCOMPARE(result.isNull(), false);
        QCOMPARE(result.size(), requestedSize);
        QCOMPARE(*size, requestedSize);
        QCOMPARE(result.isGrayscale(), true);
    }
    void testRequestImageMultiPage()
    {
        // TODO: is there a way we can confirm that the image is showing the 2nd page?
        QString id = "1/true/" + getResourceUrl("mixed_portrait.pdf").toString();
        QSize *size = new QSize();
        QSize requestedSize(250, 250);

        QImage result = m_provider->requestImage(id, size, requestedSize);

        QCOMPARE(result.isNull(), false);
        QCOMPARE(result.size(), requestedSize);
        QCOMPARE(*size, requestedSize);
        // Use allGray() as the image container is color,
        // but the content is only gray
        // otherwise this fails on power-based cpu's
        QCOMPARE(result.allGray(), true);
    }

    void testRequestImageInvalidColor()
    {
        QString id = "0/abc/" + getResourceUrl("a4_portrait.pdf").toString();
        QSize *size = new QSize();
        QSize requestedSize(250, 250);

        QImage result = m_provider->requestImage(id, size, requestedSize);

        QCOMPARE(result.isNull(), true);
        QCOMPARE((*size), requestedSize);
    }
    void testRequestImageInvalidPage()
    {
        QString id = "1000/true/" + getResourceUrl("a4_portrait.pdf").toString();
        QSize *size = new QSize();
        QSize requestedSize(250, 250);

        QImage result = m_provider->requestImage(id, size, requestedSize);

        QCOMPARE(result.isNull(), true);
        QCOMPARE((*size), requestedSize);
    }
    void testRequestImageInvalidRequestedSize()
    {
        QString id = "0/true/" + getResourceUrl("a4_portrait.pdf").toString();
        QSize *size = new QSize();
        QSize requestedSize(-100, -50);

        QImage result = m_provider->requestImage(id, size, requestedSize);

        // An invalid request size returns a size of 1,1 otherwise QML errors occur
        QCOMPARE(result.isNull(), false);
        QCOMPARE(result.size(), QSize(1, 1));
        QCOMPARE((*size), QSize(1, 1));
    }

private:
    PopplerImageProvider *m_provider;
};

QTEST_GUILESS_MAIN(TestPopplerImageProvider)
#include "tst_popplerimageprovider.moc"
