#include "QRCodeHelper.hpp"

#include "BitMatrix.h"
#include "ByteMatrix.h"
#include "MultiFormatWriter.h"
#include "ReadBarcode.h"
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::ui
{
    using namespace ZXing;

    QString DecodeQRCode(const QImage &img)
    {
        DecodeHints hints;
        hints.setTryHarder(true);
        hints.setTryRotate(true);
        //
        auto ImgFmtFromQImg = [](const QImage &img) {
            switch (img.format())
            {
                case QImage::Format_ARGB32:
                case QImage::Format_RGB32:
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
                    return ImageFormat::BGRX;
#else
                    return ImageFormat::XRGB;
#endif
                case QImage::Format_RGB888: return ImageFormat::RGB;
                case QImage::Format_RGBX8888:
                case QImage::Format_RGBA8888: return ImageFormat::RGBX;
                case QImage::Format_Grayscale8: return ImageFormat::Lum;
                default: return ImageFormat::None;
            }
        };

        auto exec = [&](const QImage &img) {
            return QString::fromStdWString(ZXing::ReadBarcode({ img.bits(), img.width(), img.height(), ImgFmtFromQImg(img) }, hints).text());
        };

        return ImgFmtFromQImg(img) == ImageFormat::None ? exec(img.convertToFormat(QImage::Format_RGBX8888)) : exec(img);
    }

    QImage EncodeQRCode(const QString &content, const QSize &size)
    {
        int eccLevel = 1;
        try
        {
            MultiFormatWriter writer(ZXing::BarcodeFormat::QR_CODE);
            writer.setMargin(1);
            writer.setEccLevel(eccLevel);

            const auto bitmap = writer.encode(content.toStdWString(), size.width(), size.height());
            const auto BM = bitmap.toByteMatrix();
            //
            const auto black = qRgba(0, 0, 0, 255);
            const auto white = qRgba(255, 255, 255, 255);
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
} // namespace Qv2ray::ui
