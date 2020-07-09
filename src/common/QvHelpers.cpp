#include "common/QvHelpers.hpp"

#include "base/Qv2rayBase.hpp"
#include "libs/puresource/src/PureJson.hpp"

namespace Qv2ray::common
{
    const QString GenerateRandomString(int len)
    {
        const QString possibleCharacters("abcdefghijklmnopqrstuvwxyz");
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
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::ConverterState state;
        const QString text = codec->toUnicode(byteArray.constData(), byteArray.size(), &state);
        if (state.invalidChars > 0)
        {
            LOG(MODULE_FILEIO, "Not a valid UTF-8 sequence: " + source.fileName())
        }
        return state.invalidChars > 0 ? byteArray : text;
    }

    bool StringToFile(const QString &text, const QString &targetpath)
    {
        bool override = false;
        {
            QFileInfo info(targetpath);
            override = info.exists();
            if (!override && !info.dir().exists())
                info.dir().mkpath(info.dir().path());
        }
        QSaveFile f{ targetpath };
        f.open(QIODevice::WriteOnly);
        f.write(text.toUtf8());
        f.commit();
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

    // backported from QvPlugin-SSR.
    QString SafeBase64Decode(QString string)
    {
        QByteArray ba = string.replace(QChar('-'), QChar('+')).replace(QChar('_'), QChar('/')).toUtf8();
        return QByteArray::fromBase64(ba, QByteArray::Base64Option::OmitTrailingEquals);
    }

    // backported from QvPlugin-SSR.
    QString SafeBase64Encode(const QString &string, bool trim)
    {
        QString base64 = string.toUtf8().toBase64();
        if (trim)
        {
            auto tmp = base64.replace(QChar('+'), QChar('-')).replace(QChar('/'), QChar('_'));
            auto crbedin = tmp.crbegin();
            auto idx = tmp.length();
            while (crbedin != tmp.crend() && (*crbedin) == '=') idx -= 1, crbedin++;
            return idx != tmp.length() ? tmp.remove(idx, tmp.length() - idx) : tmp;
        }
        else
        {
            return base64.replace(QChar('+'), QChar('-')).replace(QChar('/'), QChar('_'));
        }
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        return _string.split(QRegExp("[\r\n]"), Qt::SkipEmptyParts);
#else
        return _string.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
#endif
    }

    QStringList GetFileList(const QDir &dir)
    {
        return dir.entryList(QStringList{ "*", "*.*" }, QDir::Hidden | QDir::Files);
    }

    bool FileExistsIn(const QDir &dir, const QString &fileName)
    {
        return GetFileList(dir).contains(fileName);
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
            _name.begin(), _name.end(), [](char c) { return std::string::npos != std::string(R"("/\?%&^*;:|><)").find(c); }, '_');
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

} // namespace Qv2ray::common
