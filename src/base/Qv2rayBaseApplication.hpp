#pragma once
#include "core/handler/ConfigHandler.hpp"
#include "core/handler/RouteHandler.hpp"
#include "libs/QJsonStruct/QJsonStruct.hpp"

#include <QCoreApplication>
#include <QObject>

namespace Qv2ray
{
    enum Qv2rayExitCode
    {
        QVEXIT_NORMAL = 0,
        QVEXIT_SECONDARY_INSTANCE = 0,
        QVEXIT_PRE_INITIALIZE_FAIL = -1,
        QVEXIT_EARLY_SETUP_FAIL = -2,
        QVEXIT_CONFIG_FILE_FAIL = -3,
        QVEXIT_SSL_FAIL = -4,
        QVEXIT_NEW_VERSION = -5
    };

    enum MessageOpt
    {
        OK,
        Cancel,
        Yes,
        No,
        Ignore
    };

    enum Qv2rayPreInitResult
    {
        PRE_INIT_RESULT_ERROR,
        PRE_INIT_RESULT_QUIT,
        PRE_INIT_RESULT_CONTINUE
    };

    enum Qv2raySetupStatus
    {
        NORMAL,
        SINGLE_APPLICATION,
        FAILED
    };

    struct Qv2rayProcessArguments
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
        QString data;
        QList<QString> links;
        QList<QString> fullArgs;
        //
        QString _qvNewVersionPath;
        JSONSTRUCT_REGISTER(Qv2rayProcessArguments, F(arguments, version, data, links, fullArgs))
    };

    inline Qv2rayProcessArguments Qv2rayProcessArgument;
    class Qv2rayApplicationManager
    {
      public:
        static Qv2rayPreInitResult PreInitialize(int argc, char **argv);
        explicit Qv2rayApplicationManager();
        ~Qv2rayApplicationManager();
        virtual bool FindAndCreateInitialConfiguration() final;
        //
        virtual Qv2raySetupStatus Initialize() = 0;
        virtual Qv2rayExitCode RunQv2ray() = 0;
        //
        virtual void MessageBoxWarn(QWidget *parent, const QString &title, const QString &text, MessageOpt button) = 0;
        virtual void MessageBoxInfo(QWidget *parent, const QString &title, const QString &text, MessageOpt button) = 0;
        virtual MessageOpt MessageBoxAsk(QWidget *parent, const QString &title, const QString &text, const QList<MessageOpt> &buttons) = 0;
        virtual void OpenURL(const QString &url) = 0;

      private:
        static Qv2rayPreInitResult ParseCommandLine(QString *errorMessage, const QStringList &_argx_);
    };
    inline Qv2rayApplicationManager *qvApplicationInstance = nullptr;

#define QvCoreApplication static_cast<Qv2rayApplicationManager *>(qvApplicationInstance)

} // namespace Qv2ray
