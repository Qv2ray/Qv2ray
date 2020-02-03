#pragma once

#include "base/Qv2rayBase.hpp"
#include <QMessageBox>

#define REGEX_IPV6_ADDR "\\[\\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:)))(%.+)?\\s*\\]"

namespace Qv2ray::common
{
    QTranslator *getTranslator(const QString &lang);
    QStringList GetFileList(QDir dir);
    QString Base64Encode(QString string);
    QString Base64Decode(QString string);
    QStringList SplitLines(const QString &str);
    list<string> SplitLines_std(const QString &_string);
    bool FileExistsIn(QDir dir, QString fileName);
    const QString GenerateRandomString(int len = 12);
    //
    void QvMessageBoxWarn(QWidget *parent, QString title, QString text);
    void QvMessageBoxInfo(QWidget *parent, QString title, QString text);
    QMessageBox::StandardButton QvMessageBoxAsk(QWidget *parent, QString title, QString text, QMessageBox::StandardButton extraButtons = QMessageBox::NoButton);
    //
    QString StringFromFile(QFile *source);
    bool StringToFile(const QString *text, QFile *target);
    //
    QJsonObject JsonFromString(QString string);
    QString JsonToString(QJsonObject json, QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented);
    QString JsonToString(QJsonArray array, QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented);
    QString VerifyJsonString(const QString &source);
    //
    QString FormatBytes(long long bytes);
    void DeducePossibleFileName(const QString &baseDir, QString *fileName, const QString &extension);
    QStringList ConvertQStringList(const QList<string> &stdListString);
    std::list<string> ConvertStdStringList(const QStringList &qStringList);
    // This function cannot be marked as inline.
    QString RemoveInvalidFileName(const QString &fileName);
    bool IsValidFileName(const QString &fileName);
    //
    template <typename TYPE>
    QString StructToJsonString(const TYPE t)
    {
        return QString::fromStdString(x2struct::X::tojson(t, "", 4, ' '));
    }
    //
    template <typename TYPE>
    TYPE StructFromJsonString(const QString &str)
    {
        TYPE v;
        x2struct::X::loadjson(str.toStdString(), v, false);
        return v;
    }
    // Misc
    template<typename T>
    QJsonObject GetRootObject(const T &t)
    {
        auto json = StructToJsonString(t);
        return JsonFromString(json);
    }

    inline bool IsIPv6Address(const QString &addr)
    {
        return QRegularExpression(REGEX_IPV6_ADDR).match(addr).hasMatch();
    }

    /*
     * Generic function to find if an element of any type exists in list
     */
    template<typename T>
    bool contains(std::list<T> &listOfElements, const T &element)
    {
        // Find the iterator if element in list
        auto it = std::find(listOfElements.begin(), listOfElements.end(), element);
        //return if iterator points to end or not. It points to end then it means element
        // does not exists in list
        return it != listOfElements.end();
    }

    inline QString timeToString(const time_t &t)
    {
        auto _tm = std::localtime(&t);
        char MY_TIME[128];
        // using strftime to display time
        strftime(MY_TIME, sizeof(MY_TIME), "%x - %I:%M%p", _tm);
        return QString(MY_TIME);
    }

    template<typename myMap>
    std::vector<typename myMap::key_type> Keys(const myMap &m)
    {
        std::vector<typename myMap::key_type> r;
        r.reserve(m.size());

        for (const auto &kvp : m) {
            r.push_back(kvp.first);
        }

        return r;
    }

    template<typename myMap>
    std::vector<typename myMap::mapped_type> Values(const myMap &m)
    {
        std::vector<typename myMap::mapped_type> r;
        r.reserve(m.size());

        for (const auto &kvp : m) {
            r.push_back(kvp.second);
        }

        return r;
    }
}

using namespace Qv2ray::common;
