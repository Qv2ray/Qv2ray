#pragma once

class QImage;
class QSize;
class QString;

namespace Qv2ray::ui
{
    QString DecodeQRCode(const QImage &img);
    QImage EncodeQRCode(const QString &content, const QSize &size);
} // namespace Qv2ray::ui
using namespace Qv2ray::ui;
