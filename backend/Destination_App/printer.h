#ifndef PRINTER_H
#define PRINTER_H

#include <QtCore/QObject>

#include <QtPrintSupport/QPrinter>

#include "document.h"

class Printer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(ColorMode colorMode READ colorMode WRITE setColorMode NOTIFY colorModeChanged)
    Q_PROPERTY(int copies READ copies WRITE setCopies NOTIFY copiesChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
public:
    enum ColorMode
    {
        Color,
        GrayScale,
    };
    Q_ENUMS(ColorMode)

    explicit Printer(QObject *parent = 0);
    ColorMode colorMode() const;
    int copies() const;
    QString name() const;
    int resolution() const;

    QPrinter *printerInstance();

    Q_INVOKABLE bool print(Document *doc);
signals:
    void colorModeChanged();
    void copiesChanged();
    void nameChanged();
    void settingsChanged();
    void resolutionChanged();
public slots:
    void setColorMode(ColorMode colorMode);
    void setCopies(int copies);
    void setName(QString name);
    void setResolution(int resolution);
private:
    ColorMode m_color_mode;
    int m_copies;
    QString m_name;
    int m_resolution;
};

#endif // PRINTER_H
