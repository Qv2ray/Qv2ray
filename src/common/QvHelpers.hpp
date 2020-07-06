#pragma once

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QHostAddress>
#include <QJsonDocument>
#include <QMessageBox>
#include <QRegularExpression>

#define REGEX_IPV6_ADDR                                                                                                                         \
    R"(\[\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*\])"
#define REGEX_IPV4_ADDR                                                                                                                         \
    R"((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5]))"
#define REGEX_PORT_NUMBER R"(([0-9]|[1-9]\d{1,3}|[1-5]\d{4}|6[0-5]{2}[0-3][0-5])*)"

namespace Qv2ray::common
{
    QStringList GetFileList(const QDir &dir);

    QString SafeBase64Decode(QString string);
    QString SafeBase64Encode(const QString &string, bool trim);

    QString Base64Encode(const QString &string);
    QString Base64Decode(const QString &string);
    QStringList SplitLines(const QString &str);
    // list<string> SplitLines_std(const QString &_string);
    bool FileExistsIn(const QDir &dir, const QString &fileName);
    const QString GenerateRandomString(int len = 12);
    //
    QString StringFromFile(const QString &filePath);
    QString StringFromFile(QFile &source);
    // bool StringToFile(const QString &text, QFile &target);
    bool StringToFile(const QString &text, const QString &targetpath);
    //
    QJsonObject JsonFromString(const QString &string);
    QString JsonToString(const QJsonObject &json, QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented);
    QString JsonToString(const QJsonArray &array, QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented);
    QString VerifyJsonString(const QString &source);
    //
    QString FormatBytes(const int64_t bytes);
    void DeducePossibleFileName(const QString &baseDir, QString *fileName, const QString &extension);
    //
    // This function cannot be marked as inline.
    QString RemoveInvalidFileName(const QString &fileName);
    bool IsValidFileName(const QString &fileName);
    inline QString GenerateUuid()
    {
        return GenerateRandomString().toLower();
        // return QUuid::createUuid().toString(QUuid::WithoutBraces);
    }

    inline QString TruncateString(const QString &str, int limit = -1, const QString &suffix = "...")
    {
        QString t = str;
        t.truncate(limit);
        return (limit == -1 || str.length() < limit) ? str : (t + suffix);
    }

    namespace validation
    {
        const inline QRegularExpression __regex_ipv4_full(REGEX_IPV4_ADDR "$");

        inline bool IsIPv4Address(const QString &addr)
        {
            return __regex_ipv4_full.match(addr).hasMatch();
        }

        inline bool IsIPv6Address(const QString &addr)
        {
            QHostAddress address(addr);
            return QAbstractSocket::IPv6Protocol == address.protocol();
        }

        inline bool IsValidIPAddress(const QString &addr)
        {
            return !addr.isEmpty() && (IsIPv4Address(addr) || IsIPv6Address(addr));
        }

        inline bool IsValidDNSServer(const QString &addr)
        {
            return IsIPv4Address(addr)                  //
                   || IsIPv6Address(addr)               //
                   || addr.startsWith("https://")       //
                   || addr.startsWith("https+local://") //
                   || addr == "localhost";
        }
    } // namespace validation

    inline QString timeToString(const time_t &t)
    {
        QDateTime timestamp;
        timestamp.setSecsSinceEpoch(t);
        return timestamp.toString();
    }
    void QvMessageBoxWarn(QWidget *parent, const QString &title, const QString &text);
    void QvMessageBoxInfo(QWidget *parent, const QString &title, const QString &text);
    QMessageBox::StandardButton QvMessageBoxAsk(QWidget *parent, const QString &title, const QString &text,
                                                QMessageBox::StandardButton extraButtons = QMessageBox::StandardButton::NoButton);
} // namespace Qv2ray::common

using namespace Qv2ray::common;
