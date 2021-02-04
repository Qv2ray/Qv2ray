#include "Qv2rayQMLProperty.hpp"
#include "base/Qv2rayBaseApplication.hpp"
#include "ui/Qv2rayPlatformApplication.hpp"

#include <QQuickView>

namespace Qv2ray
{
    class Qv2rayQMLApplication : public Qv2rayPlatformApplication
    {
        Q_OBJECT
      public:
        explicit Qv2rayQMLApplication(int &argc, char *argv[]);
        void MessageBoxWarn(QWidget *parent, const QString &title, const QString &text) override;
        void MessageBoxInfo(QWidget *parent, const QString &title, const QString &text) override;
        MessageOpt MessageBoxAsk(QWidget *parent, const QString &title, const QString &text, const QList<MessageOpt> &buttons) override;
        QStringList checkPrerequisitesInternal() override;
        Qv2rayExitReason runQv2rayInternal() override;
        void terminateUIInternal() override;
        void OpenURL(const QString &url) override;

      private slots:
#ifndef QV2RAY_NO_SINGLEAPPLICATON
        void onMessageReceived(quint32, QByteArray) override;
#endif
      private:
        QQuickView qmlViewer;
        Qv2rayQMLProperty uiProperty;
    };

#ifdef Qv2rayApplication
#undef Qv2rayApplication
#endif

#define Qv2rayApplication Qv2rayQMLApplication
#define QvQmlApplication static_cast<Qv2rayQMLApplication *>(qApp)

} // namespace Qv2ray
