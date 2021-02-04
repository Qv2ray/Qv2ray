#include "base/Qv2rayBaseApplication.hpp"
#include "src/ui/Qv2rayPlatformApplication.hpp"

namespace Qv2ray
{
    class Qv2rayCliApplication : public Qv2rayPlatformApplication
    {
        Q_OBJECT
      public:
        explicit Qv2rayCliApplication(int &argc, char *argv[]);
        QStringList checkPrerequisitesInternal() override;
        Qv2rayExitReason runQv2rayInternal() override;

      public:
        void MessageBoxWarn(QWidget *, const QString &, const QString &) override
        {
        }
        void MessageBoxInfo(QWidget *, const QString &, const QString &) override
        {
        }
        MessageOpt MessageBoxAsk(QWidget *, const QString &, const QString &, const QList<MessageOpt> &) override
        {
            return OK;
        }
        void OpenURL(const QString &) override
        {
        }

      private:
        void terminateUIInternal() override;
#ifndef QV2RAY_NO_SINGLEAPPLICATON
        void onMessageReceived(quint32 clientID, QByteArray msg) override;
#endif
    };

#ifdef Qv2rayApplication
#undef Qv2rayApplication
#endif

#define Qv2rayApplication Qv2rayCliApplication
#define QvCliApplication static_cast<Qv2rayCliApplication *>(qApp)

} // namespace Qv2ray
