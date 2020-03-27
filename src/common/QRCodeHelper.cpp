#include "QRCodeHelper.hpp"

#include "BitMatrix.h"
#include "ByteMatrix.h"
#include "MultiFormatWriter.h"
#include "ReadBarcode.h"
#include "TextUtfEncoding.h"
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::components
{
    using namespace ZXing;
    QString DecodeQRCode(const QImage &source)
    {
        QImage img = source.copy();
        auto result =
            ReadBarcode(img.width(), img.height(), img.bits(), img.width() * 4, 4, 0, 1, 2, { BarcodeFormatFromString("") }, true, true);

        if (result.isValid())
        {
            auto errLevel = result.metadata().getString(ResultMetadata::Key::ERROR_CORRECTION_LEVEL);
            if (!errLevel.empty())
            {
                LOG(MODULE_UI, "EC Level: " + QString::fromStdWString(errLevel))
            }
            return QString::fromStdWString(result.text());
        }

        return "";
    }

    QImage EncodeQRCode(const QString &content, const QSize &size)
    {
        int eccLevel = 1;
        try
        {
            auto barcodeFormat = BarcodeFormatFromString("QR_CODE");

            MultiFormatWriter writer(barcodeFormat);
            writer.setMargin(1);
            writer.setEccLevel(eccLevel);

            auto bitmap = writer.encode(content.toStdWString(), size.width(), size.height());
            auto BM = bitmap.toByteMatrix();
            //
            const QRgb black = qRgba(0, 0, 0, 255);
            const QRgb white = qRgba(255, 255, 255, 255);
            //
            auto image = QImage(bitmap.width(), bitmap.width(), QImage::Format_ARGB32);
            image.fill(white);
            //
            for (auto i = 0; i < bitmap.width(); ++i)
            {
                for (auto j = 0; j < bitmap.height(); ++j)
                {
                    // 0 means black?
                    if (bitmap.get(j, i))
                    {
                        image.setPixel(i, j, black);
                    }
                }
            }
            return image;
        }
        catch (const std::exception &e)
        {
            return {};
        }
    }
} // namespace Qv2ray::components
