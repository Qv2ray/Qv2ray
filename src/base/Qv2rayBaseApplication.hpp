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
        QVEXIT_CONFIG_PATH_FAIL = -3,
        QVEXIT_CONFIG_FILE_FAIL = -4,
        QVEXIT_SSL_FAIL = -5,
        QVEXIT_NEW_VERSION = -6
    };

    enum MessageOptions
    {
        OK,
        Cancel,
        Yes,
        No,
        Ignore
    };

    class Qv2rayBaseApplication : public QCoreApplication
    {
        Q_OBJECT
      public:
        Qv2rayBaseApplication(int argc, char *argv[]);
        virtual ~Qv2rayBaseApplication(){};
        virtual void MessageBoxWarn(QWidget *parent, const QString &title, const QString &text, MessageOptions button) = 0;
        virtual void MessageBoxInfo(QWidget *parent, const QString &title, const QString &text, MessageOptions button) = 0;
        virtual MessageOptions MessageBoxAsk(QWidget *parent, const QString &title, const QString &text,
                                             const QList<MessageOptions> &buttons) = 0;
    };

#define QvBaseApplication (static_cast<Qv2rayBaseApplication *>(qApp))
} // namespace Qv2ray
