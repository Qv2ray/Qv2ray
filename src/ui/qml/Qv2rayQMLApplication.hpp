#include "base/Qv2rayBaseApplication.hpp"
#include "ui/common/platforms/Qv2rayPlatformApplication.hpp"

#include <QQuickView>

namespace Qv2ray
{
    class Qv2rayQMLApplication : public Qv2rayPlatformApplication
    {
        Q_OBJECT
      public:
        explicit Qv2rayQMLApplication(int &argc, char *argv[]);
        void MessageBoxWarn(QWidget *parent, const QString &title, const QString &text, MessageOpt button = OK) override;
        void MessageBoxInfo(QWidget *parent, const QString &title, const QString &text, MessageOpt button = OK) override;
        MessageOpt MessageBoxAsk(QWidget *parent, const QString &title, const QString &text, const QList<MessageOpt> &buttons) override;
        Qv2raySetupStatus Initialize() override;
        Qv2rayExitCode RunQv2ray() override;
        void OpenURL(const QString &url) override;

      private slots:
        void TerminateUI() override;
#ifndef QV2RAY_NO_SINGLEAPPLICATON
        void onMessageReceived(quint32 clientId, QByteArray msg) override;
#endif
      private:
        QQuickView qmlViewer;
    };

#ifdef Qv2rayApplication
    #undef Qv2rayApplication
#endif

#define Qv2rayApplication Qv2rayQMLApplication
#define QvQmlApplication static_cast<Qv2rayQMLApplication *>(qApp)

} // namespace Qv2ray
