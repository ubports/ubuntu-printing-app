#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <QtGui/QPainter>

#include <poppler/qt5/poppler-qt5.h>

class Document : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
public:
    explicit Document(QObject *parent = 0);
    ~Document();
    int count() const;
    QUrl url() const;

    QImage makeImage(QSizeF size, int pageNumber);
    QPixmap makePixmap(QSize size, int pageNumber);

    Q_INVOKABLE QImage renderImage(QSize size, int pageNumber);
    Q_INVOKABLE bool renderPage(QPainter *painter, int pageNumber);

    bool printFromImage(QPainter *painter, int pageNumber);
signals:
    void countChanged();
    void urlChanged();
public slots:
    void setUrl(QUrl url);
private:
    float getDpi(QSizeF sourceSizeF, QSizeF targetSizeF);

    int m_count;
    Poppler::Document *m_document;
    QUrl m_url;
};

#endif // DOCUMENT_H
