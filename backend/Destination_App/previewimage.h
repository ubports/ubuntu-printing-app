#ifndef PREVIEWIMAGE_H
#define PREVIEWIMAGE_H

#include <QtQuick/QQuickPaintedItem>

#include "document.h"

class PreviewImage : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(Document *document READ document WRITE setDocument NOTIFY documentChanged)
public:
    PreviewImage(QQuickItem *parent = 0);
    Document *document() const;
    void paint(QPainter *painter);
signals:
    void documentChanged();
public slots:
    void setDocument(Document *document);
private:
    Document *m_document;
};

#endif // PREVIEWIMAGE_H
