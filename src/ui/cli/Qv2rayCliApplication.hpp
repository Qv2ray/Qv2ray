#include "base/Qv2rayBaseApplication.hpp"

#include <iostream>

namespace Qv2ray
{
    class Qv2rayCliApplication
        : public QCoreApplication
        , public Qv2rayApplicationManagerInterface
    {
        Q_OBJECT
      public:
        explicit Qv2rayCliApplication(int argc, char *argv[]);
        void MessageBoxWarn(QWidget *parent, const QString &title, const QString &text, MessageOptions button = OK) override
        {
            std::cout << text.toStdString() << std::endl;
        }
        void MessageBoxInfo(QWidget *parent, const QString &title, const QString &text, MessageOptions button = OK) override
        {
            std::cout << text.toStdString() << std::endl;
        }
        MessageOptions MessageBoxAsk(QWidget *parent, const QString &title, const QString &text, const QList<MessageOptions> &buttons) override
        {
            std::cout << text.toStdString() << std::endl;
            return {};
        }
        Qv2raySetupStatus Initialize() override
        {
            std::cout << "Welcome to use Qv2ray!" << std::endl;
            return {};
        }
        bool FindAndCreateInitialConfiguration() override
        {
            std::cout << "Looking for configuration files." << std::endl;
            return true;
        }
        Qv2rayExitCode RunQv2ray() override
        {
            PluginHost = new QvPluginHost();
            std::cout << "Please select a node to connect" << std::endl;
            return {};
        }
        void OpenURL(const QString &url) override
        {
        }
    };

#ifdef Qv2rayApplication
    #undef Qv2rayApplication
#endif

#define Qv2rayApplication Qv2rayCliApplication
#define QvCliApplication static_cast<Qv2rayCliApplication *>(qApp)

} // namespace Qv2ray
