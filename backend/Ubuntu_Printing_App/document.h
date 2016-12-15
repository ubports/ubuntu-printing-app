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
    enum Errors {
        ErrorDocumentInvalid,
        ErrorNotFound,
        ErrorNotPdf,
    };

    Q_ENUMS(Errors)

    enum Orientation {
        Portrait,
        Landscape,
    };

    explicit Document(QObject *parent = 0);
    ~Document();
    int count() const;
    Poppler::Page *getPage(int pageNumber);
    Orientation orientation() const;
    QString title() const;
    QUrl url() const;

    QImage makeImage(QSizeF size, int pageNumber);
    QImage makeImageToFit(QSizeF size, int pageNumber, bool color);
signals:
    void countChanged();
    void error(Errors errorType);
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
