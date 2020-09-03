#include "Qv2rayQMLApplication.hpp"

#include "components/translations/QvTranslator.hpp"
#include "core/settings/SettingsBackend.hpp"

#include <QDesktopServices>
#include <QMessageBox>
#include <QQmlContext>
#include <QQuickStyle>

Qv2rayQMLApplication::Qv2rayQMLApplication(int &argc, char *argv[]) : Qv2rayPlatformApplication(argc, argv)
{
}

void Qv2rayQMLApplication::MessageBoxWarn(QWidget *parent, const QString &title, const QString &text, MessageOpt button)
{
    QMessageBox::warning(parent, title, text, MessageBoxButtonMap[button]);
}
void Qv2rayQMLApplication::MessageBoxInfo(QWidget *parent, const QString &title, const QString &text, MessageOpt button)
{
    QMessageBox::information(parent, title, text, MessageBoxButtonMap[button]);
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

Qv2raySetupStatus Qv2rayQMLApplication::Initialize()
{
    return InitializeInternal();
}

Qv2rayExitCode Qv2rayQMLApplication::RunQv2ray()
{
    RunInternal();
    QQuickStyle::setStyle("Material");
    const QUrl url("qrc:/forms/MainWindow.qml");
    qmlViewer.setSource(url);
    return (Qv2rayExitCode) exec();
}

void Qv2rayQMLApplication::TerminateUI()
{
}

void Qv2rayQMLApplication::OpenURL(const QString &url)
{
    QDesktopServices::openUrl(url);
}

#ifndef QV2RAY_NO_SINGLEAPPLICATON
void Qv2rayQMLApplication::onMessageReceived(quint32 clientId, QByteArray msg)
{
}
#endif
