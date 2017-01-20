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

#include "Ubuntu_Printing_App/document.h"

#define A4_PORTRAIT_SIZE QSize(595, 842)
#define A4_LANDSCAPE_SIZE QSize(842, 595)
#define A5_LANDSCAPE_SIZE QSize(595, 420)

#define INVALID_IMAGE_SIZE QSize(0, 0)
#define VALID_IMAGE_SIZE QSize(100, 100)

Q_DECLARE_METATYPE(Document::Errors)

class TestDocument : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init()
    {
        qRegisterMetaType<Document::Errors>("Errors");

        m_document = new Document();
    }
    void cleanup()
    {
        QSignalSpy destroyedSpy(m_document, SIGNAL(destroyed(QObject*)));
        m_document->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
    }
    QUrl getResourceUrl(const QString &resource) const
    {
        return QUrl("file://" + QDir::currentPath() + "/../../resources/pdf/" + resource);
    }

    void testInit()
    {
        QCOMPARE(m_document->count(), 0);
        QCOMPARE(m_document->getPageSize(0), QSizeF());
        QCOMPARE(m_document->getPageSize(0).isValid(), false);
        QCOMPARE(m_document->orientation(), Document::Portrait);
        QCOMPARE(m_document->title(), QStringLiteral(""));
        QCOMPARE(m_document->url(), QUrl(QStringLiteral("")));
    }

    void testCountSinglePage()
    {
        QSignalSpy countSpy(m_document, SIGNAL(countChanged()));
        m_document->setUrl(getResourceUrl("a4_portrait.pdf"));
        QTRY_COMPARE(countSpy.count(), 1);
        QCOMPARE(m_document->count(), 1);
    }
    void testCountMultiPage()
    {
        QSignalSpy countSpy(m_document, SIGNAL(countChanged()));
        m_document->setUrl(getResourceUrl("mixed_portrait.pdf"));
        QTRY_COMPARE(countSpy.count(), 1);
        QCOMPARE(m_document->count(), 3);
    }

    void testGetPageSizeA4()
    {
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("a4_portrait.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);
        // Use QSize to compare otherwise double's will be slightly different
        QCOMPARE(m_document->getPageSize(0).toSize(), A4_PORTRAIT_SIZE);
    }
    void testGetPageSizeMixed()
    {
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("mixed_portrait.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);
        // Use QSize to compare otherwise double's will be slightly different
        QCOMPARE(m_document->getPageSize(0).toSize(), A4_PORTRAIT_SIZE);

        // Check that second page size is landscape as this is a mixed document
        QCOMPARE(m_document->getPageSize(1).toSize(), A4_LANDSCAPE_SIZE);
    }
    void testGetPageSizeA5Landscape()
    {
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("a5_landscape.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);
        // Use QSize to compare otherwise double's will be slightly different
        QCOMPARE(m_document->getPageSize(0).toSize(), A5_LANDSCAPE_SIZE);
    }

    void testMakeImage()
    {
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("a4_portrait.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);

        QImage image = m_document->makeImage(VALID_IMAGE_SIZE, 0);
        QCOMPARE(image.isNull(), false);
        QCOMPARE(image.size(), VALID_IMAGE_SIZE);
    }
    void testMakeImageInvalidDocument()
    {
        QImage image = m_document->makeImage(VALID_IMAGE_SIZE, 0);
        QCOMPARE(image.isNull(), true);
    }
    void testMakeImageInvalidPage()
    {
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("a4_portrait.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);

        QImage image = m_document->makeImage(VALID_IMAGE_SIZE, 1000);
        QCOMPARE(image.isNull(), true);
    }
    void testMakeImageInvalidSize()
    {
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("a4_portrait.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);

        QImage image = m_document->makeImage(INVALID_IMAGE_SIZE, 0);
        QCOMPARE(image.isNull(), true);
    }
    void testMakeImageNegativePage()
    {
        QImage image = m_document->makeImage(VALID_IMAGE_SIZE, -1);
        QCOMPARE(image.isNull(), true);
    }
    void testMakeImageInvalidRenderToImage()
    {
        QSKIP("TODO: need to have a test to hit the case where renderToImage returns a null image");
    }


    void testMakeImageToFit()
    {
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("a4_portrait.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);

        QImage image = m_document->makeImageToFit(VALID_IMAGE_SIZE, 0, true);
        QCOMPARE(image.isNull(), false);
        QCOMPARE(image.size(), VALID_IMAGE_SIZE);
    }
    void testMakeImageToFitColor()
    {
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("color.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);

        QImage image = m_document->makeImageToFit(VALID_IMAGE_SIZE, 0, true);
        QCOMPARE(image.isNull(), false);
        QCOMPARE(image.size(), VALID_IMAGE_SIZE);
        QCOMPARE(image.isGrayscale(), false);
    }
    void testMakeImageToFitGrayscale()
    {
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("color.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);

        QImage image = m_document->makeImageToFit(VALID_IMAGE_SIZE, 0, false);
        QCOMPARE(image.isNull(), false);
        QCOMPARE(image.size(), VALID_IMAGE_SIZE);
        QCOMPARE(image.isGrayscale(), true);
    }
    void testMakeImageToFitInvalidDocument()
    {
        QImage image = m_document->makeImageToFit(VALID_IMAGE_SIZE, 0, true);
        QCOMPARE(image.isNull(), true);
    }
    void testMakeImageToFitInvalidPage()
    {
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("a4_portrait.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);

        QImage image = m_document->makeImageToFit(VALID_IMAGE_SIZE, 1000, true);
        QCOMPARE(image.isNull(), true);
    }
    void testMakeImageToFitInvalidSize()
    {
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("a4_portrait.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);

        QImage image = m_document->makeImageToFit(INVALID_IMAGE_SIZE, 0, true);
        QCOMPARE(image.isNull(), true);
    }
    void testMakeImageToFitNegativePage()
    {
        QImage image = m_document->makeImageToFit(VALID_IMAGE_SIZE, -1, true);
        QCOMPARE(image.isNull(), true);
    }


    void testOrientationPortrait()
    {
        QSignalSpy orientationSpy(m_document, SIGNAL(orientationChanged()));
        m_document->setUrl(getResourceUrl("a4_portrait.pdf"));
        QTRY_COMPARE(orientationSpy.count(), 1);
        QCOMPARE(m_document->orientation(), Document::Orientation::Portrait);
    }
    void testOrientationLandscape()
    {
        QSignalSpy orientationSpy(m_document, SIGNAL(orientationChanged()));
        m_document->setUrl(getResourceUrl("a5_landscape.pdf"));
        QTRY_COMPARE(orientationSpy.count(), 1);
        QCOMPARE(m_document->orientation(), Document::Orientation::Landscape);
    }
    void testOrientationMixedPortrait()
    {
        QSignalSpy orientationSpy(m_document, SIGNAL(orientationChanged()));
        m_document->setUrl(getResourceUrl("mixed_portrait.pdf"));
        QTRY_COMPARE(orientationSpy.count(), 1);
        QCOMPARE(m_document->orientation(), Document::Orientation::Portrait);
    }
    void testOrientationMixedLandscape()
    {
        QSignalSpy orientationSpy(m_document, SIGNAL(orientationChanged()));
        m_document->setUrl(getResourceUrl("mixed_landscape.pdf"));
        QTRY_COMPARE(orientationSpy.count(), 1);
        QCOMPARE(m_document->orientation(), Document::Orientation::Landscape);
    }

    void testTitle()
    {
        QSignalSpy titleSpy(m_document, SIGNAL(titleChanged()));
        m_document->setUrl(getResourceUrl("test_title.pdf"));
        QTRY_COMPARE(titleSpy.count(), 1);
        QCOMPARE(m_document->title(), QStringLiteral("Test Title Document"));
    }

    void testUrl()
    {
        QSignalSpy errorSpy(m_document, SIGNAL(error(Document::Errors)));
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));

        QUrl resource = getResourceUrl("a4_portrait.pdf");

        m_document->setUrl(resource);
        QTRY_COMPARE(urlSpy.count(), 1);
        QCOMPARE(errorSpy.count(), 0);
        QCOMPARE(m_document->url(), resource);
    }
    void testUrlNotFound()
    {
        QSignalSpy errorSpy(m_document, SIGNAL(error(Document::Errors)));
        m_document->setUrl(getResourceUrl("notfound.pdf"));
        QTRY_COMPARE(errorSpy.count(), 1);
        QCOMPARE(m_document->url(), QUrl(QStringLiteral("")));

        QList<QVariant> args = errorSpy.takeFirst();
        QCOMPARE(args.at(0).toInt(), (int) Document::Errors::ErrorNotFound);
    }
    void testUrlNotPdf()
    {
        QSignalSpy errorSpy(m_document, SIGNAL(error(Document::Errors)));
        m_document->setUrl(getResourceUrl("text.txt"));
        QTRY_COMPARE(errorSpy.count(), 1);
        QCOMPARE(m_document->url(), QUrl(QStringLiteral("")));

        QList<QVariant> args = errorSpy.takeFirst();
        QCOMPARE(args.at(0).toInt(), (int) Document::Errors::ErrorNotPdf);
    }
    void testUrlInvalidDocument()
    {
        QSignalSpy errorSpy(m_document, SIGNAL(error(Document::Errors)));
        m_document->setUrl(getResourceUrl("corrupt.pdf"));
        QTRY_COMPARE(errorSpy.count(), 1);
        QCOMPARE(m_document->url(), QUrl(QStringLiteral("")));

        QList<QVariant> args = errorSpy.takeFirst();
        QCOMPARE(args.at(0).toInt(), (int) Document::Errors::ErrorDocumentInvalid);
    }
private:
    Document *m_document;
};

QTEST_GUILESS_MAIN(TestDocument)
#include "tst_document.moc"
