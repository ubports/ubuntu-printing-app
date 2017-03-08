#ifndef PRINTER_H
#define PRINTER_H

#include <QtCore/QObject>

#include <cups/cups.h>
#include <cups/ppd.h>

#include "document.h"

class Printer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(ColorMode colorMode READ colorMode WRITE setColorMode NOTIFY colorModeChanged)
    Q_PROPERTY(int copies READ copies WRITE setCopies NOTIFY copiesChanged)
    Q_PROPERTY(bool duplex READ duplex WRITE setDuplex NOTIFY duplexChanged)
    Q_PROPERTY(bool duplexSupported READ duplexSupported NOTIFY duplexSupportedChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool pdfMode READ pdfMode WRITE setPdfMode NOTIFY pdfModeChanged)
    Q_PROPERTY(Quality quality READ quality WRITE setQuality NOTIFY qualityChanged)
public:
    enum ColorMode
    {
        Color,
        GrayScale,
    };
    Q_ENUMS(ColorMode)

    enum Quality
    {
        Draft,
        Normal,
        Best,
        Photo,
    };
    Q_ENUMS(Quality)

    explicit Printer(QObject *parent = 0);
    ~Printer();
    ColorMode colorMode() const;
    int copies() const;
    bool duplex() const;
    bool duplexSupported() const;
    QString name() const;
    bool pdfMode() const;
    Quality quality() const;

    Q_INVOKABLE bool print(Document *doc);
signals:
    void colorModeChanged();
    void copiesChanged();
    void duplexChanged();
    void duplexSupportedChanged();
    void nameChanged();
    void settingsChanged();
    void pdfModeChanged();
    void qualityChanged();

    void exportRequest(const QString &filepath);
public slots:
    void setColorMode(ColorMode colorMode);
    void setCopies(int copies);
    void setDuplex(bool duplex);
    void setDuplexSupported(bool duplexSupported);
    void setName(QString name);
    void setPdfMode(bool pdfMode);
    void setQuality(Quality quality);
private:
    void loadDuplexModes(const QString name);
    bool loadOptions(cups_dest_t *cups_dest, Document *doc);
    QString makeOutputFilepath() const;

    ColorMode m_color_mode;
    int m_copies;

    cups_dest_t *m_cups_dest;
    QString m_cups_instance;
    QString m_cups_name;

    bool m_duplex;
    QList<QString> m_duplex_modes;
    bool m_duplex_supported;
    int m_job_id;
    QString m_name;

    ppd_file_t *m_ppd;

    bool m_pdf_mode;
    Quality m_quality;

};

#endif // PRINTER_H
