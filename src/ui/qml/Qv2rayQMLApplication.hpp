#include "base/Qv2rayBaseApplication.hpp"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <iostream>

namespace Qv2ray
{
    class Qv2rayQMLApplication
        : public QApplication
        , public Qv2rayApplicationManagerInterface
    {
        Q_OBJECT
      public:
        explicit Qv2rayQMLApplication(int argc, char *argv[]);
        void MessageBoxWarn(QWidget *parent, const QString &title, const QString &text, MessageOptions button = OK) override
        {
        }
        void MessageBoxInfo(QWidget *parent, const QString &title, const QString &text, MessageOptions button = OK) override
        {
        }
        MessageOptions MessageBoxAsk(QWidget *parent, const QString &title, const QString &text, const QList<MessageOptions> &buttons) override
        {
            return {};
        }
        Qv2raySetupStatus Initialize() override
        {
            return {};
        }

        bool FindAndCreateInitialConfiguration() override
        {
            return true;
        }
        Qv2rayExitCode RunQv2ray() override
        {
            PluginHost = new QvPluginHost();
            QQmlApplicationEngine engine{ this };
            QQuickStyle::setStyle("Material");
            const QUrl url("qrc:/forms/MainWindow.qml");
            const auto connectLambda = [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                {
                    QCoreApplication::exit(-1);
                }
            };
            connect(&engine, &QQmlApplicationEngine::objectCreated, this, connectLambda, Qt::QueuedConnection);
            engine.addImportPath(QStringLiteral("qrc:/forms/"));
            engine.load(url);
            return (Qv2rayExitCode) exec();
        }
        void OpenURL(const QString &url) override
        {
        }
    };

#ifdef Qv2rayApplication
    #undef Qv2rayApplication
#endif

#define Qv2rayApplication Qv2rayQMLApplication
#define QvQmlApplication static_cast<Qv2rayQMLApplication *>(qApp)

} // namespace Qv2ray
