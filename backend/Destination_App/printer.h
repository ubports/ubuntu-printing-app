#ifndef PRINTER_H
#define PRINTER_H

#include <QtCore/QObject>

#include <QtPrintSupport/QPrinter>

#include "document.h"

class Printer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QString paperSize READ paperSize WRITE setPaperSize NOTIFY paperSizeChanged)
public:
    explicit Printer(QObject *parent = 0);
    QString name() const;
    Qt::Orientation orientation() const;
    QString paperSize() const;

    Q_INVOKABLE QSize size() const;
    Q_INVOKABLE bool print(Document *doc);
signals:
    void nameChanged();
    void paperSizeChanged();
    void orientationChanged();
public slots:
    void setName(QString name);
    void setOrientation(Qt::Orientation orientation);
    void setPaperSize(QString paperSize);
private:
    QString m_name;
    QString m_paper_size;
    Qt::Orientation m_orientation = Qt::Orientation::Vertical;
    QPrinter *m_printer;
};

#endif // PRINTER_H
