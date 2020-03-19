#include "QvPluginHost.hpp"

#include "base/Qv2rayLog.hpp"
#include "components/plugins/interface/QvPluginInterface.hpp"

#include <QApplication>
#include <QDir>
#include <QPluginLoader>
#include <iostream>
using namespace Qv2ray::base;
namespace Qv2ray::components::plugins
{
    QvPluginHost::QvPluginHost(QObject *parent) : QObject(parent)
    {
        LoadPlugin();
    }
    bool QvPluginHost::LoadPlugin()
    {
        QDir pluginsDir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_WIN)
        if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
            pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
        if (pluginsDir.dirName() == "MacOS")
        {
            pluginsDir.cdUp();
            pluginsDir.cdUp();
            pluginsDir.cdUp();
        }
#endif
        pluginsDir.cd("plugins");
        const QStringList entries = pluginsDir.entryList(QDir::Files);
        for (const QString &fileName : entries)
        {
            QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = pluginLoader.instance();
            if (plugin)
            {
                auto echoInterface = qobject_cast<Qv2rayInterface *>(plugin);

                //
                if (echoInterface)
                {
                    QString name = echoInterface->Name();
                    QString author = echoInterface->Author();
                    //
                    std::cout << name.toStdString() << std::endl;
                    std::cout << author.toStdString() << std::endl;
                    return true;
                }
                pluginLoader.unload();
            }
        }

        return false;
    }
} // namespace Qv2ray::components::plugins
