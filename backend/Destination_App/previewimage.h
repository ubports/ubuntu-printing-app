#ifndef PREVIEWIMAGE_H
#define PREVIEWIMAGE_H

#include <QtQuick/QQuickPaintedItem>

#include "document.h"
#include "printer.h"

class PreviewImage : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(Document * document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(Printer * printer READ printer WRITE setPrinter NOTIFY printerChanged)
public:
    PreviewImage(QQuickItem *parent = 0);
    Document *document() const;
    void paint(QPainter *painter);
    Printer *printer() const;
signals:
    void documentChanged();
    void printerChanged();
public slots:
    void setDocument(Document *document);
    void setPrinter(Printer *printer);
private:
    Document *m_document;
    Printer *m_printer;
};

#endif // PREVIEWIMAGE_H
