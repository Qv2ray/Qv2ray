//  Qv2rayBase, the modular feature-rich infrastructure library for Qv2ray.
//  Copyright (C) 2021 Moody and relavent Qv2ray contributors.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"

#include "Qv2rayBase/Common/Settings.hpp"
#include "Qv2rayBase/Plugin/LatencyTestHost.hpp"
#include "Qv2rayBase/Plugin/PluginAPIHost.hpp"
#include "Qv2rayBase/Plugin/PluginManagerCore.hpp"
#include "Qv2rayBase/Profile/KernelManager.hpp"
#include "Qv2rayBase/Profile/ProfileManager.hpp"
#include "Qv2rayBase/private/Common/SettingsUpgrade_p.hpp"
#include "Qv2rayBase/private/Qv2rayBaseLibrary_p.hpp"

// Private headers
#include "Qv2rayBase/private/Interfaces/BaseStorageProvider_p.hpp"

#include <QDir>
#include <QStandardPaths>

namespace Qv2rayBase
{
    using namespace Qv2rayBase::Profile;
    using namespace Qv2rayBase::Plugin;
    Qv2rayBaseLibrary *m_instance = nullptr;

    QV2RAYBASE_FAILED_REASON Qv2rayBaseLibrary::Initialize(Qv2rayStartFlags flags,                    //
                                                           const Interfaces::StorageContext &ctx,     //
                                                           Interfaces::IUserInteractionInterface *ui, //
                                                           Interfaces::IStorageProvider *stor)
    {
        Q_ASSERT_X(m_instance == nullptr, "Qv2rayBaseLibrary", "m_instance is not null! Cannot construct another Qv2rayBaseLibrary when there's one existed");
        m_instance = this;
        Q_D(Qv2rayBaseLibrary);

        d->startupFlags = flags;
        d->uiInterface = ui;

        connect(this, &Qv2rayBaseLibrary::_warnInternal, this, [d](auto title, auto content) { d->uiInterface->p_MessageBoxWarn(title, content); });
        connect(this, &Qv2rayBaseLibrary::_infoInternal, this, [d](auto title, auto content) { d->uiInterface->p_MessageBoxInfo(title, content); });

        if (stor)
            d->storageProvider = stor;
        else
            d->storageProvider = new Interfaces::Qv2rayBasePrivateStorageProvider;

        d->configuration = new Models::Qv2rayBaseConfigObject;

        if (!d->storageProvider->LookupConfigurations(ctx))
        {
            Shutdown();
            return ERR_LOCATE_CONFIGURATION;
        }

        auto configuration = d->storageProvider->GetBaseConfiguration();
        const auto result = _private::MigrateSettings(configuration);
        if (!result)
        {
            Shutdown();
            return ERR_MIGRATE_CONFIGURATION;
        }

        d->configuration->loadJson(configuration);

        d->pluginCore = new Plugin::PluginManagerCore;
        d->pluginAPIHost = new Plugin::PluginAPIHost;

        if (!flags.testFlag(START_NO_PLUGINS))
        {
            d->pluginCore->LoadPlugins();
            d->pluginAPIHost->InitializePlugins();
        }

        qRegisterMetaType<StatisticsObject>();

        d->latencyTestHost = new Plugin::LatencyTestHost;

        // ProfileManager needs to connect a statistics signal from KernelManager, so construct kernel manager first.
        d->kernelManager = new Profile::KernelManager;
        d->profileManager = new Profile::ProfileManager;

        return NORMAL;
    }

    void Qv2rayBaseLibrary::SaveConfigurations() const
    {
        Q_D(const Qv2rayBaseLibrary);

        assert(d->profileManager), d->profileManager->SaveConnectionConfig();
        assert(d->latencyTestHost), d->latencyTestHost->StopAllLatencyTest();
        assert(d->pluginCore), d->pluginCore->SavePluginSettings();
        assert(d->storageProvider), d->storageProvider->EnsureSaved();
    }

    void Qv2rayBaseLibrary::Shutdown()
    {
        Q_D(Qv2rayBaseLibrary);

        if (d->kernelManager)
            d->kernelManager->StopConnection();

        if (d->profileManager)
            d->profileManager->SaveConnectionConfig();

        if (d->latencyTestHost)
            d->latencyTestHost->StopAllLatencyTest();

        if (d->pluginCore)
            d->pluginCore->SavePluginSettings();

        if (d->storageProvider)
            d->storageProvider->EnsureSaved();

        delete d->kernelManager;
        delete d->latencyTestHost;
        delete d->pluginAPIHost;
        delete d->pluginCore;
        delete d->profileManager;

        if (d->storageProvider)
            d->storageProvider->StoreBaseConfiguration(d->configuration->toJson());
        delete d->storageProvider;

        delete d->configuration;

        // delete d->uiInterface;
        m_instance = nullptr;
    }

    Qv2rayBaseLibrary *Qv2rayBaseLibrary::instance()
    {
        Q_ASSERT_X(m_instance, "Qv2rayBaseLibrary", "m_instance is null! Did you forget to initialize Qv2rayBase?");
        return m_instance;
    }

    Qv2rayBaseLibrary::Qv2rayBaseLibrary() : d_ptr(new Qv2rayBaseLibraryPrivate)
    {
        qInfo() << "Qv2ray Base Library" << QV2RAY_BASELIB_VERSION << "on" << QSysInfo::prettyProductName() << QSysInfo::currentCpuArchitecture();
        qDebug() << "Qv2ray Start Time:" << QTime::currentTime().msecsSinceStartOfDay();
    }

    Qv2rayBaseLibrary::~Qv2rayBaseLibrary()
    {
    }

    QStringList Qv2rayBaseLibrary::GetAssetsPaths(const QString &dirName)
    {
        static const auto makeAbs = [](const QDir &p) { return p.absolutePath(); };

        // Configuration Path
        QStringList list;

        if (qEnvironmentVariableIsSet("QV2RAYBASE_RESOURCES_PATH"))
            list << makeAbs(qEnvironmentVariable("QV2RAYBASE_RESOURCES_PATH") + "/" + dirName);

        list << instance()->StorageProvider()->GetAssetsPath(dirName);
        list.removeDuplicates();
        return list;
    }

    void Qv2rayBaseLibrary::Warn(const QString &title, const QString &text)
    {
        emit instance()->_warnInternal(title, text, QPrivateSignal{});
    }

    void Qv2rayBaseLibrary::Info(const QString &title, const QString &text)
    {
        emit instance()->_infoInternal(title, text, QPrivateSignal{});
    }

    MessageOpt Qv2rayBaseLibrary::Ask(const QString &title, const QString &text, const QList<MessageOpt> &options)
    {
        return instance()->d_ptr->uiInterface->p_MessageBoxAsk(title, text, options);
    }

    void Qv2rayBaseLibrary::OpenURL(const QUrl &url)
    {
        instance()->d_ptr->uiInterface->p_OpenURL(url);
    }

    PluginAPIHost *Qv2rayBaseLibrary::PluginAPIHost()
    {
        return instance()->d_ptr->pluginAPIHost;
    }

    Models::Qv2rayBaseConfigObject *Qv2rayBaseLibrary::GetConfig()
    {
        return instance()->d_ptr->configuration;
    }

    PluginManagerCore *Qv2rayBaseLibrary::PluginManagerCore()
    {
        return instance()->d_ptr->pluginCore;
    }

    LatencyTestHost *Qv2rayBaseLibrary::LatencyTestHost()
    {
        return instance()->d_ptr->latencyTestHost;
    }

    Interfaces::IStorageProvider *Qv2rayBaseLibrary::StorageProvider()
    {
        return instance()->d_ptr->storageProvider;
    }

    ProfileManager *Qv2rayBaseLibrary::ProfileManager()
    {
        return instance()->d_ptr->profileManager;
    }

    KernelManager *Qv2rayBaseLibrary::KernelManager()
    {
        return instance()->d_ptr->kernelManager;
    }
} // namespace Qv2rayBase
