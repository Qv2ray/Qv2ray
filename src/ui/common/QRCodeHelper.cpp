#include "QRCodeHelper.hpp"

#include "QtQrCode"
#include "QtQrCodePainter"

#include <QImage>

namespace Qv2ray::ui
{
    QString DecodeQRCode(const QImage &)
    {
        return "";
    }

    QImage EncodeQRCode(const QString content, int size)
    {
        QtQrCode c;
        c.setData(content.toUtf8());
        return QtQrCodePainter(2.0).toImage(c, size);
    }
} // namespace Qv2ray::ui
