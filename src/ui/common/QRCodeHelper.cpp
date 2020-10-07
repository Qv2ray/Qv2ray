#include "QRCodeHelper.hpp"

#include "BinaryBitmap.h"
#include "BitMatrix.h"
#include "ReadBarcode.h"
#include "qrcode/QRReader.h"
#include "qrcode/QRWriter.h"

#include <QImage>

namespace Qv2ray::ui
{
    using namespace ZXing;
    ImageFormat GetImageFormat(const QImage &img)
    {
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
    }

    class GreyscaleBitmapProcessor : public BinaryBitmap
    {
      public:
        GreyscaleBitmapProcessor(const QImage &image)
        {
            if (GetImageFormat(image) == ImageFormat::None)
                buffer = image.convertToFormat(QImage::Format_RGBX8888);
            else
                buffer = image;

            format = GetImageFormat(image);
            pixStride = PixStride(format);
            rowStride = image.width() * pixStride;
        }

        int width() const override
        {
            return buffer.width();
        }
        int height() const override
        {
            return buffer.height();
        }
        bool getBlackRow(int, BitArray &) const override
        {
            return true;
        }

        std::shared_ptr<const BitMatrix> getBlackMatrix() const override
        {
            if (!_cache)
            {
                BitMatrix res(width(), height());
                auto src = data(0, 0) + GreenIndex(format);
                for (auto y = 0; y < res.height(); ++y)
                    for (auto &dst : res.row(y))
                        dst = *(src += pixStride) <= THRESHOLD;
                _cache = std::make_shared<const BitMatrix>(std::move(res));
            }
            return _cache;
        }

      private:
        static constexpr uint8_t THRESHOLD = 127;
        QImage buffer;
        ImageFormat format;
        mutable std::shared_ptr<const BitMatrix> _cache;
        int pixStride = 0, rowStride = 0;
        const uint8_t *data(int x, int y) const
        {
            return buffer.bits() + y * rowStride + x * pixStride;
        }
    };

    QString DecodeQRCode(const QImage &img)
    {
        DecodeHints hints;
        hints.setTryHarder(true);
        hints.setTryRotate(true);
        return QString::fromStdWString(QRCode::Reader(hints).decode(GreyscaleBitmapProcessor(img)).text());
    }

    QImage EncodeQRCode(const QString &content, const QSize &size)
    {
        int eccLevel = 1;
        constexpr auto black = qRgba(0, 0, 0, 255);
        constexpr auto white = qRgba(255, 255, 255, 255);
        QImage image{ size, QImage::Format_ARGB32 };
        image.fill(white);
        try
        {
            QRCode::Writer writer;
            writer.setMargin(1);
            const auto bitmap = writer.encode(content.toStdWString(), size.width(), size.height());
            for (auto i = 0; i < bitmap.width(); i++)
            {
                for (auto j = 0; j < bitmap.height(); j++)
                {
                    // 0 means black?
                    if (bitmap.get(j, i))
                        image.setPixel(i, j, black);
                }
            }
        }
        catch (...)
        {
        }
        return image;
    }
} // namespace Qv2ray::ui
