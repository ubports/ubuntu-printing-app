#include <QtQml>
#include <QtQml/QQmlContext>
#include "backend.h"

#include "document.h"
#include "pagehelper.h"
#include "popplerimageprovider.h"
#include "printer.h"
#include "printerinfo.h"


#define MAKE_SINGLETON_FACTORY(type) \
    static QObject* type##_singleton_factory(QQmlEngine* engine, QJSEngine* scriptEngine) { \
        Q_UNUSED(engine); \
        Q_UNUSED(scriptEngine); \
        return new type(); \
    }

MAKE_SINGLETON_FACTORY(PrinterInfo)


void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu_Printing_App"));

    qmlRegisterType<Document>(uri, 1, 0, "Document");
    qmlRegisterType<PageHelper>(uri, 1, 0, "PageHelper");
    qmlRegisterType<Printer>(uri, 1, 0, "Printer");
    qmlRegisterSingletonType<PrinterInfo>(uri, 1, 0, "PrinterInfo", PrinterInfo_singleton_factory);
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);

    engine->addImageProvider(QLatin1String("poppler"), new PopplerImageProvider);
}
