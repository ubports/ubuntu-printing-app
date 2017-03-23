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

#include "UbuntuPrintingApp/document.h"
#include "UbuntuPrintingApp/pagehelper.h"

#define A4_PORTRAIT_ASPECT_RATIO 595.0 / 842.0
#define A4_LANDSCAPE_ASPECT_RATIO 842.0 / 595.0
#define A5_LANDSCAPE_ASPECT_RATIO 595.0 / 420.0

class TestPageHelper : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init()
    {
        m_document = new Document();
        m_page_helper = new PageHelper();
        m_page_helper->setDocument(m_document);
    }
    void cleanup()
    {
        QSignalSpy destroyedPageHelperSpy(m_page_helper, SIGNAL(destroyed(QObject*)));
        m_page_helper->deleteLater();
        QTRY_COMPARE(destroyedPageHelperSpy.count(), 1);

        QSignalSpy destroyedDocumentSpy(m_document, SIGNAL(destroyed(QObject*)));
        m_document->deleteLater();
        QTRY_COMPARE(destroyedDocumentSpy.count(), 1);
    }
    QUrl getResourceUrl(const QString &resource) const
    {
        return QUrl("file://" + QDir::currentPath() + "/../../resources/pdf/" + resource);
    }

    void testInit()
    {
        QCOMPARE(m_page_helper->aspect(), A4_PORTRAIT_ASPECT_RATIO);
        QCOMPARE(m_page_helper->document(), m_document);
        QCOMPARE(m_page_helper->page(), 0);
    }

    void testAspect()
    {
        QSignalSpy aspectSpy(m_page_helper, SIGNAL(aspectChanged()));
        m_document->setUrl(getResourceUrl("a5_landscape.pdf"));
        QTRY_COMPARE(aspectSpy.count(), 1);
        QCOMPARE(m_page_helper->aspect(), A5_LANDSCAPE_ASPECT_RATIO);
    }
    void testAspectA4Document()
    {
        QSignalSpy aspectSpy(m_page_helper, SIGNAL(aspectChanged()));
        m_document->setUrl(getResourceUrl("a5_landscape.pdf"));
        QTRY_COMPARE(aspectSpy.count(), 1);
        QCOMPARE(m_page_helper->aspect(), A5_LANDSCAPE_ASPECT_RATIO);

        m_document->setUrl(getResourceUrl("a4_portrait.pdf"));
        QTRY_COMPARE(aspectSpy.count(), 2);
        QCOMPARE(m_page_helper->aspect(), A4_PORTRAIT_ASPECT_RATIO);
    }
    void testAspectFallsBack()
    {
        QSignalSpy aspectSpy(m_page_helper, SIGNAL(aspectChanged()));
        m_document->setUrl(getResourceUrl("a5_landscape.pdf"));
        QTRY_COMPARE(aspectSpy.count(), 1);
        QCOMPARE(m_page_helper->aspect(), A5_LANDSCAPE_ASPECT_RATIO);

        // Test when set to a new document that it goes back to A4
        Document *newDoc = new Document();
        m_page_helper->setDocument(newDoc);
        QTRY_COMPARE(aspectSpy.count(), 2);
        QCOMPARE(m_page_helper->aspect(), A4_PORTRAIT_ASPECT_RATIO);

        newDoc->deleteLater();
    }

    void testDocument()
    {
        QSignalSpy documentSpy(m_page_helper, SIGNAL(documentChanged()));
        Document *newDoc = new Document();
        m_page_helper->setDocument(newDoc);
        QTRY_COMPARE(documentSpy.count(), 1);
        QCOMPARE(m_page_helper->document(), newDoc);
    }
    void testDocumentDisconnect()
    {
        QSignalSpy documentSpy(m_page_helper, SIGNAL(documentChanged()));
        Document *newDoc = new Document();
        m_page_helper->setDocument(newDoc);
        QTRY_COMPARE(documentSpy.count(), 1);
        QCOMPARE(m_page_helper->document(), newDoc);

        // Check that changing the old document doesn't cause signals that were
        // linked to be fired - eg loading of aspect
        QSignalSpy aspectSpy(m_page_helper, SIGNAL(aspectChanged()));
        QSignalSpy urlSpy(m_document, SIGNAL(urlChanged()));
        m_document->setUrl(getResourceUrl("a5_landscape.pdf"));
        QTRY_COMPARE(urlSpy.count(), 1);
        QCOMPARE(aspectSpy.count(), 0);

        newDoc->deleteLater();
    }

    void testDocumentUrlChange()
    {
        QSignalSpy aspectSpy(m_page_helper, SIGNAL(aspectChanged()));
        m_document->setUrl(getResourceUrl("a5_landscape.pdf"));
        QTRY_COMPARE(aspectSpy.count(), 1);

        QCOMPARE(m_page_helper->aspect(), A5_LANDSCAPE_ASPECT_RATIO);
    }

    void testPageMixedLandscape()
    {
        QSignalSpy aspectSpy(m_page_helper, SIGNAL(aspectChanged()));
        QSignalSpy pageSpy(m_page_helper, SIGNAL(pageChanged()));
        m_document->setUrl(getResourceUrl("mixed_landscape.pdf"));
        QTRY_COMPARE(aspectSpy.count(), 1);
        QCOMPARE(m_page_helper->page(), 0);
        QCOMPARE(m_page_helper->aspect(), A4_LANDSCAPE_ASPECT_RATIO);

        m_page_helper->setPage(1);
        QTRY_COMPARE(pageSpy.count(), 1);
        QCOMPARE(m_page_helper->page(), 1);
        QTRY_COMPARE(aspectSpy.count(), 2);
        QCOMPARE(m_page_helper->aspect(), A4_PORTRAIT_ASPECT_RATIO);
    }
    void testPageMixedPortrait()
    {
        QSignalSpy aspectSpy(m_page_helper, SIGNAL(aspectChanged()));
        QSignalSpy pageSpy(m_page_helper, SIGNAL(pageChanged()));
        m_document->setUrl(getResourceUrl("mixed_portrait.pdf"));
        QCOMPARE(aspectSpy.count(), 0);
        QCOMPARE(m_page_helper->aspect(), A4_PORTRAIT_ASPECT_RATIO);

        m_page_helper->setPage(1);
        QTRY_COMPARE(pageSpy.count(), 1);
        QCOMPARE(m_page_helper->page(), 1);
        QTRY_COMPARE(aspectSpy.count(), 1);
        QCOMPARE(m_page_helper->aspect(), A4_LANDSCAPE_ASPECT_RATIO);
    }

private:
    Document *m_document;
    PageHelper *m_page_helper;
};

QTEST_GUILESS_MAIN(TestPageHelper)
#include "tst_pagehelper.moc"
