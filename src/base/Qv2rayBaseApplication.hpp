#pragma once

#include "base/models/QvSettingsObject.hpp"

#include <QCoreApplication>
#include <QObject>

namespace Qv2ray
{
    enum MessageOpt
    {
        OK,
        Cancel,
        Yes,
        No,
        Ignore
    };

    enum Qv2rayExitReason
    {
        EXIT_NORMAL = 0,
        EXIT_NEW_VERSION_TRIGGER = EXIT_NORMAL,
        EXIT_SECONDARY_INSTANCE = EXIT_NORMAL,
        EXIT_INITIALIZATION_FAILED = -1,
        EXIT_PRECONDITION_FAILED = -2,
    };

    struct Qv2rayStartupArguments
    {
        enum Argument
        {
            NORMAL = 0,
            QV2RAY_LINK = 1,
            EXIT = 2,
            RECONNECT = 3,
            DISCONNECT = 4
        };
        QList<Argument> arguments;
        QString version;
        int buildVersion;
        QString data;
        QList<QString> links;
        QList<QString> fullArgs;
        //
        bool noAPI;
        bool noAutoConnection;
        bool debugLog;
        bool noPlugins;
        bool exitQv2ray;
        //
        QString _qvNewVersionPath;
        JSONSTRUCT_REGISTER(Qv2rayStartupArguments, F(arguments, data, version, links, fullArgs, buildVersion))
    };

    class Qv2rayApplicationInterface
    {
      public:
        Qv2ray::base::config::Qv2rayConfigObject *ConfigObject;
        QString ConfigPath;
        Qv2rayStartupArguments StartupArguments;

      public:
        explicit Qv2rayApplicationInterface();
        ~Qv2rayApplicationInterface();

      public:
        virtual QStringList GetAssetsPaths(const QString &dirName) const final;
        //
        virtual void MessageBoxWarn(QWidget *parent, const QString &title, const QString &text) = 0;
        virtual void MessageBoxInfo(QWidget *parent, const QString &title, const QString &text) = 0;
        virtual MessageOpt MessageBoxAsk(QWidget *parent, const QString &title, const QString &text, const QList<MessageOpt> &buttons) = 0;
        virtual void OpenURL(const QString &url) = 0;
    };
    inline Qv2rayApplicationInterface *QvCoreApplication = nullptr;
} // namespace Qv2ray

#define GlobalConfig (*Qv2ray::QvCoreApplication->ConfigObject)
