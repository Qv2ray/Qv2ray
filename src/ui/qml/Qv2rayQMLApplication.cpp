#include "Qv2rayQMLApplication.hpp"

#include "components/translations/QvTranslator.hpp"
#include "core/settings/SettingsBackend.hpp"

#include <QDesktopServices>
#include <QMessageBox>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#ifdef QV2RAY_QMLLIVE_DEBUG
#include <qmllive/livenodeengine.h>
#include <qmllive/qmllive.h>
#include <qmllive/remotereceiver.h>
#endif

Qv2rayQMLApplication::Qv2rayQMLApplication(int &argc, char *argv[]) : Qv2rayPlatformApplication(argc, argv)
{
}

void Qv2rayQMLApplication::MessageBoxWarn(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox::warning(parent, title, text);
}
void Qv2rayQMLApplication::MessageBoxInfo(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox::information(parent, title, text);
}

MessageOpt Qv2rayQMLApplication::MessageBoxAsk(QWidget *parent, const QString &title, const QString &text, const QList<MessageOpt> &list)
{
    QFlags<QMessageBox::StandardButton> btns;
    for (const auto &b : list)
    {
        btns.setFlag(MessageBoxButtonMap[b]);
    }
    return MessageBoxButtonMap.key(QMessageBox::question(parent, title, text, btns));
}

QStringList Qv2rayQMLApplication::checkPrerequisitesInternal()
{
    return {};
}

Qv2rayExitReason Qv2rayQMLApplication::runQv2rayInternal()
{
    QQuickStyle::setStyle("Material");
    QQmlApplicationEngine engine;
    const QUrl url("qrc:/forms/MainWindow.qml");
    const auto connectLambda = [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    };
    connect(&engine, &QQmlApplicationEngine::objectCreated, this, connectLambda, Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("qv2ray", &uiProperty);
    engine.addImportPath(QStringLiteral("qrc:/forms/"));
    engine.load(url);

#ifdef QV2RAY_QMLLIVE_DEBUG
    LiveNodeEngine node;

    // Let QmlLive know your runtime
    node.setQmlEngine(&engine);

    // Allow it to display QML components with non-QQuickWindow root object
    QQuickView fallbackView(&engine, 0);
    node.setFallbackView(&fallbackView);

    // Tell it where file updates should be stored relative to
    node.setWorkspace(applicationDirPath() + "/forms", LiveNodeEngine::AllowUpdates | LiveNodeEngine::UpdatesAsOverlay);

    // Listen to IPC call from remote QmlLive Bench
    RemoteReceiver receiver;
    receiver.registerNode(&node);
    receiver.listen(10234);

    QQuickWindow *window = qobject_cast<QQuickWindow *>(engine.rootObjects().first());

    // Advanced use: let it know the initially loaded QML component (do this
    // only after registering to receiver!)
    QList<QQmlError> warnings;
    node.usePreloadedDocument(applicationDirPath() + "/forms/MainWindow.qml", window, warnings);
#endif
    return (Qv2rayExitReason) exec();
}

void Qv2rayQMLApplication::terminateUIInternal()
{
}

void Qv2rayQMLApplication::OpenURL(const QString &url)
{
    QDesktopServices::openUrl(url);
}

#ifndef QV2RAY_NO_SINGLEAPPLICATON
void Qv2rayQMLApplication::onMessageReceived(quint32, QByteArray)
{
}
#endif
