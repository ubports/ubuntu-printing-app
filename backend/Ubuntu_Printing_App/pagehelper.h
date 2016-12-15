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
