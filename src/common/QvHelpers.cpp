#include "common/QvHelpers.hpp"

#include "libs/puresource/src/PureJson.hpp"

#include <QGraphicsEffect>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QQueue>

namespace Qv2ray::common
{
    const QString GenerateRandomString(int len)
    {
        const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
        QString randomString;

        for (int i = 0; i < len; ++i)
        {
            uint rand = QRandomGenerator::system()->generate();
            uint max = static_cast<uint>(possibleCharacters.length());
            QChar nextChar = possibleCharacters[rand % max];
            randomString.append(nextChar);
        }

        return randomString;
    }

    QString StringFromFile(const QString &filePath)
    {
        QFile f(filePath);
        return StringFromFile(f);
    }

    QString StringFromFile(QFile &source)
    {
        bool wasOpened = source.isOpen();
        if (!wasOpened)
            source.open(QFile::ReadOnly);
        auto byteArray = source.readAll();
        if (!wasOpened)
            source.close();
        //
        QTextCodec::ConverterState state;
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        const QString text = codec->toUnicode(byteArray.constData(), byteArray.size(), &state);
        if (state.invalidChars > 0)
        {
            LOG(MODULE_FILEIO, "Not a valid UTF-8 sequence: " + source.fileName())
            return byteArray;
        }
        else
        {
            return text;
        }
    }

    bool StringToFile(const QString &text, const QString &targetpath)
    {
        auto file = QFile(targetpath);
        return StringToFile(text, file);
    }
    bool StringToFile(const QString &text, QFile &targetFile)
    {
        QFileInfo info(targetFile);
        if (!info.dir().exists())
        {
            info.dir().mkpath(info.dir().path());
        }
        bool override = targetFile.exists();
        targetFile.open(QFile::WriteOnly);
        targetFile.write(text.toUtf8());
        targetFile.close();
        return override;
    }

    QString JsonToString(const QJsonObject &json, QJsonDocument::JsonFormat format)
    {
        QJsonDocument doc;
        doc.setObject(json);
        return doc.toJson(format);
    }

    QString JsonToString(const QJsonArray &array, QJsonDocument::JsonFormat format)
    {
        QJsonDocument doc;
        doc.setArray(array);
        return doc.toJson(format);
    }

    QString VerifyJsonString(const QString &source)
    {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(source.toUtf8(), &error);
        Q_UNUSED(doc)

        if (error.error == QJsonParseError::NoError)
        {
            return "";
        }
        else
        {
            LOG(MODULE_UI, "WARNING: Json parse returns: " + error.errorString())
            return error.errorString();
        }
    }

    QJsonObject JsonFromString(const QString &string)
    {
        auto removeComment = RemoveComment(string.trimmed()).trimmed();
        if (removeComment != string.trimmed())
        {
            LOG(MODULE_FILEIO, "Some comments have been removed from the json.")
        }
        QJsonDocument doc = QJsonDocument::fromJson(removeComment.toUtf8());
        return doc.object();
    }

    QString Base64Encode(const QString &string)
    {
        QByteArray ba = string.toUtf8();
        return ba.toBase64();
    }

    QString Base64Decode(const QString &string)
    {
        QByteArray ba = string.toUtf8();
        return QString(QByteArray::fromBase64(ba));
    }

    QStringList SplitLines(const QString &_string)
    {
        return _string.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    }

    list<string> SplitLines_std(const QString &_string)
    {
        list<string> list;

        for (auto line : _string.split(QRegExp("[\r\n]"), QString::SkipEmptyParts))
        {
            list.push_back(line.toStdString());
        }

        return list;
    }

    QStringList GetFileList(const QDir &dir)
    {
        return dir.entryList(QStringList{ "*", "*.*" }, QDir::Hidden | QDir::Files);
    }

    bool FileExistsIn(const QDir &dir, const QString &fileName)
    {
        return GetFileList(dir).contains(fileName);
    }

    void QvMessageBoxWarn(QWidget *parent, const QString &title, const QString &text)
    {
        QMessageBox::warning(parent, title, text, QMessageBox::Ok | QMessageBox::Default, 0);
    }

    void QvMessageBoxInfo(QWidget *parent, const QString &title, const QString &text)
    {
        QMessageBox::information(parent, title, text, QMessageBox::Ok | QMessageBox::Default, 0);
    }

    QMessageBox::StandardButton QvMessageBoxAsk(QWidget *parent, const QString &title, const QString &text,
                                                QMessageBox::StandardButton extraButtons)
    {
        return QMessageBox::question(parent, title, text, QMessageBox::Yes | QMessageBox::No | extraButtons);
    }

    QString FormatBytes(const int64_t b)
    {
        auto _bytes = b;
        char str[64];
        const char *sizes[5] = { "B", "KB", "MB", "GB", "TB" };
        int i;
        double dblByte = _bytes;

        for (i = 0; i < 5 && _bytes >= 1000; i++, _bytes /= 1000) dblByte = _bytes / 1000.0;

        sprintf(str, "%.2f", dblByte);
        return QString(str) + " " + QString(sizes[i]);
    }

    bool IsValidFileName(const QString &fileName)
    {
        QString name = fileName;
        return name == RemoveInvalidFileName(fileName);
    }

    QString RemoveInvalidFileName(const QString &fileName)
    {
        std::string _name = fileName.toStdString();
        std::replace_if(
            _name.begin(), _name.end(), [](char c) { return std::string::npos != string(R"("/\?%&^*;:|><)").find(c); }, '_');
        return QString::fromStdString(_name);
    }

    /// This returns a file name without extensions.
    void DeducePossibleFileName(const QString &baseDir, QString *fileName, const QString &extension)
    {
        int i = 1;

        if (!QDir(baseDir).exists())
        {
            QDir(baseDir).mkpath(baseDir);
            LOG(MODULE_FILEIO, "Making path: " + baseDir)
        }

        while (true)
        {
            if (!QFile(baseDir + "/" + fileName + "_" + QSTRN(i) + extension).exists())
            {
                *fileName = *fileName + "_" + QSTRN(i);
                return;
            }
            else
            {
                DEBUG(MODULE_FILEIO, "File with name: " + *fileName + "_" + QSTRN(i) + extension + " already exists")
            }

            i++;
        }
    }
    QPixmap ApplyEffectToImage(QPixmap src, QGraphicsEffect *effect, int extent)
    {
        constexpr int extent2 = 0;
        if (src.isNull())
            return QPixmap(); // No need to do anything else!
        if (!effect)
            return src; // No need to do anything else!
        QGraphicsScene scene;
        auto p = scene.addPixmap(src);
        p->setGraphicsEffect(effect);
        //
        QImage res(src.size() + QSize(extent2, extent2), QImage::Format_ARGB32);
        res.fill(Qt::transparent);
        QPainter ptr(&res);
        //
        scene.render(&ptr, QRectF(), QRectF(-extent, -extent, src.width() + extent2, src.height() + extent * 2));
        //
        scene.removeItem(p);
        return QPixmap::fromImage(res);
    }
    QPixmap BlurImage(const QPixmap &pixmap, const double rad)
    {
        QGraphicsBlurEffect pBlur;
        pBlur.setBlurRadius(rad);
        return ApplyEffectToImage(pixmap, &pBlur, 0);
    }

    QPixmap ColorizeImage(const QPixmap &pixmap, const QColor &color, const qreal factor)
    {
        QGraphicsColorizeEffect pColor;
        pColor.setColor(color);
        pColor.setStrength(factor);
        return ApplyEffectToImage(pixmap, &pColor, 0);
    }
} // namespace Qv2ray::common
