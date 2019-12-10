#ifndef UTILS_H
#define UTILS_H

#include "Qv2rayBase.hpp"
#include <QMessageBox>
#include <QUuid>

namespace Qv2ray
{
    namespace Utils
    {
        QTranslator *getTranslator(const QString &lang);

        QStringList GetFileList(QDir dir);

        QString Base64Encode(QString string);
        QString Base64Decode(QString string);
        QStringList SplitLines(const QString &str);
        QList<string> SplitLinesStdString(const QString &_string);

        bool CheckFile(QDir dir, QString fileName);

        const QString GenerateRandomString(int len = 12);

        void SetConfigDirPath(const QString *path);
        QString GetConfigDirPath();

        void SetGlobalConfig(Qv2rayConfig conf);
        Qv2rayConfig GetGlobalConfig();

        void LoadGlobalConfig();

        void QvMessageBox(QWidget *parent, QString title, QString text);
        int QvMessageBoxAsk(QWidget *parent, QString title, QString text, QMessageBox::StandardButton extraButtons = QMessageBox::NoButton);
        //
        QString StringFromFile(QFile *source);
        bool StringToFile(const QString *text, QFile *target);
        //
        QJsonObject JsonFromString(QString string);
        QString JsonToString(QJsonObject json, QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented);
        QString JsonToString(QJsonArray array, QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented);
        //
        QString VerifyJsonString(const QString &source);
        //
        QString Stringify(list<string> list, QString saperator = ";");
        QString Stringify(QList<QString> list, QString saperator = ";");
        //
        //
        template <typename TYPE>
        QString StructToJsonString(const TYPE t)
        {
            return QString::fromStdString(X::tojson(t, "", 4, ' '));
        }
        //
        //
        template <typename TYPE>
        TYPE StructFromJsonString(const QString &str)
        {
            TYPE v;
            X::loadjson(str.toStdString(), v, false);
            return v;
        }
        //
        // Misc
        template<typename T>
        QJsonObject GetRootObject(const T &t)
        {
            auto json = StructToJsonString(t);
            QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json.toStdString()));
            return doc.object();
        }
        template QJsonObject GetRootObject<RuleObject>(const RuleObject &t);
        template QJsonObject GetRootObject<StreamSettingsObject>(const StreamSettingsObject &t);
        template QJsonObject GetRootObject<VMessServerObject>(const VMessServerObject &t);
        //
        //
        template <typename T>
        void RemoveItem(std::vector<T> &vec, size_t pos)
        {
            auto it = vec.begin();
            std::advance(it, pos);
            vec.erase(it);
        }

        QString FormatBytes(long long bytes);
        void DeducePossibleFileName(const QString &baseDir, QString *fileName, const QString &extension);
        //
        //
        QString ConvertGFWToPAC(const QString &rawContent, const QString &customProxyString);
        void QFastAppendTextDocument(const QString &message, QTextDocument *doc);
        QStringList ConvertQStringList(const QList<string> &stdListString);
        inline bool IsValidFileName(const QString &str)
        {
            // If no match, we are good.
            return QRegExp(R"([\/\\\"?%*:|><]|(^\.{1,2}$))").indexIn(str) == -1;
        }
    }
}

using namespace Qv2ray::Utils;
#endif // UTILS_H
