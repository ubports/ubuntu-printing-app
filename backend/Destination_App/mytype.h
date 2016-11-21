#ifndef MYTYPE_H
#define MYTYPE_H

#include <QObject>

class MyType : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString helloWorld READ helloWorld WRITE setHelloWorld NOTIFY helloWorldChanged )
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)

public:
    explicit MyType(QObject *parent = 0);
    ~MyType();
    QString url() { return m_url; }

    Q_INVOKABLE void printType();

Q_SIGNALS:
    void helloWorldChanged();
    void urlChanged();

protected:
    QString helloWorld() { return m_message; }
    void setHelloWorld(QString msg) { m_message = msg; Q_EMIT helloWorldChanged(); }
    void setUrl(QString url);

    QString m_message;
    QString m_url;
};

#endif // MYTYPE_H
