#include "popplerasyncimageprovider.h"

#include "popplerasyncimageresponse.h"

PopplerAsyncImageProvider::PopplerAsyncImageProvider()
    : QQuickAsyncImageProvider()
{

}

QQuickImageResponse *PopplerAsyncImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    PopplerAsyncImageResponse *response = new PopplerAsyncImageResponse(id, requestedSize);
    pool.start(response);
    return response;
}
