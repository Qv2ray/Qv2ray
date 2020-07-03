#pragma once

#include <QImage>
#include <QString>
namespace Qv2ray::common
{
    QString DecodeQRCode(const QImage &img);
    QImage EncodeQRCode(const QString &content, const QSize &size);
} // namespace Qv2ray::common
using namespace Qv2ray::common;
