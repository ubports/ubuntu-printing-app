#include "pagehelper.h"

#include <QtCore/QSizeF>

#include <poppler/qt5/poppler-qt5.h>

#define A4_ASPECT_RATIO 210.0 / 297.0

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

    Poppler::Page *page = m_document->getPage(m_page);

    if (page) {
        QSizeF pageSize = page->pageSizeF();

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

