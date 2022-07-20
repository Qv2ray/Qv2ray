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

#pragma once

#include "Interfaces/IStorageProvider.hpp"
#include "Interfaces/IUserInteractionInterface.hpp"
#include "Qv2rayBaseFeatures.hpp"

// clang-format off
// Do not use these within Qv2rayBase library itself.
#ifndef Qv2rayBase_EXPORTS
#define QvBaselib            Qv2rayBase::Qv2rayBaseLibrary::instance()
#define QvKernelManager      Qv2rayBase::Qv2rayBaseLibrary::KernelManager()
#define QvLatencyTestHost    Qv2rayBase::Qv2rayBaseLibrary::LatencyTestHost()
#define QvProfileManager     Qv2rayBase::Qv2rayBaseLibrary::ProfileManager()
#define QvPluginManagerCore  Qv2rayBase::Qv2rayBaseLibrary::PluginManagerCore()
#define QvPluginAPIHost      Qv2rayBase::Qv2rayBaseLibrary::PluginAPIHost()
#define QvStorageProvider    Qv2rayBase::Qv2rayBaseLibrary::StorageProvider()
#endif
// clang-format on

namespace Qv2rayBase
{
    Q_NAMESPACE

    // clang-format off
    namespace Models  { struct Qv2rayBaseConfigObject; }
    namespace Plugin  { class LatencyTestHost; }
    namespace Plugin  { class PluginManagerCore; }
    namespace Plugin  { class PluginAPIHost; }
    namespace Profile { class ProfileManager; }
    namespace Profile { class KernelManager; }
    namespace Interfaces { class IStorageProvider; };
    // clang-format on

    enum QV2RAYBASE_FAILED_REASON
    {
        NORMAL = 0,

        // Anything larger than this value is considered to be an error.
        ERROR = 100,
        ERR_LOCATE_CONFIGURATION,
        ERR_MIGRATE_CONFIGURATION,
    };
    Q_ENUM_NS(QV2RAYBASE_FAILED_REASON)

    enum Qv2rayStartFlag
    {
        // clang-format off
        START_NORMAL        = 0x0000,
        START_NO_PLUGINS    = 0x0001,
        // clang-format on
    };

    Q_DECLARE_FLAGS(Qv2rayStartFlags, Qv2rayStartFlag)
    Q_FLAG_NS(Qv2rayStartFlags)

    class Qv2rayBaseLibraryPrivate;
    ///
    /// \brief The Qv2rayBaseLibrary class
    /// This is the class used by Qv2ray baselib, and must be initialized before using the library,
    ///
    class QV2RAYBASE_EXPORT Qv2rayBaseLibrary : public QObject
    {
        Q_OBJECT

      public:
        ///
        /// \brief Qv2rayBaseLibrary
        /// construct a Qv2rayBaseLibrary instance, the constructor should only be called once during the
        /// whole application lifetime.
        explicit Qv2rayBaseLibrary();
        ~Qv2rayBaseLibrary();

        QV2RAYBASE_FAILED_REASON Initialize(Qv2rayStartFlags flags,                  //
                                            const Interfaces::StorageContext &ctx,   //
                                            Interfaces::IUserInteractionInterface *, //
                                            Interfaces::IStorageProvider * = nullptr);

        void SaveConfigurations() const;

        ///
        /// \brief Clean up all resources that was allocated by Qv2rayBaseLibrary, stop the current
        /// connection and all ongoing latency tests, save connections, groups, routing and plugin
        /// setings, then unload all plugins.
        ///
        void Shutdown();

      public:
        ///
        /// \brief Get current instance of Qv2ray base library, returns nullptr when the library is not initialized.
        /// \return returns the pointer to the current instance of Qv2ray base library
        ///
        static Qv2rayBaseLibrary *instance();

        ///
        /// \brief Get configurations for Qv2ray base library.
        ///
        static Qv2rayBase::Models::Qv2rayBaseConfigObject *GetConfig();

        ///
        /// \brief Get search paths for assets, (e.g. when searching for plugins)
        /// \param dirName The directory suffix name used to search, (e.g. "plugins")
        ///
        static QStringList GetAssetsPaths(const QString &dirName);

        ///
        /// \brief Warn Show a warning message to user
        /// \param title The title of message
        /// \param text The content of message
        ///
        static void Warn(const QString &title, const QString &text);

        ///
        /// \brief Warn Show an information message to user
        /// \param title The title of message
        /// \param text The content of message
        ///
        static void Info(const QString &title, const QString &text);

        ///
        /// \brief Warn Let user to make a choice
        /// \param title The title of message
        /// \param text The content of message
        /// \param options The possible options to the user
        ///
        static MessageOpt Ask(const QString &title, const QString &text, const QList<MessageOpt> &options = { MessageOpt::Yes, MessageOpt::No });

        ///
        /// \brief OpenURL Opens a URL in external app
        /// \param url the URL to be opened
        ///
        static void OpenURL(const QUrl &url);

        ///
        /// \brief PluginAPIHost returns Plugin API host
        /// \return The pointer to the Plugin API Host
        ///
        static Qv2rayBase::Plugin::PluginAPIHost *PluginAPIHost();

        ///
        /// \brief PluginManagerCore Get the core plugin manager
        /// \return The pointer to the core plugin manager
        ///
        static Qv2rayBase::Plugin::PluginManagerCore *PluginManagerCore();

        ///
        /// \brief LatencyTestHost Get the latency tester host
        /// \return The pointer to the latency test host.
        static Qv2rayBase::Plugin::LatencyTestHost *LatencyTestHost();

        ///
        /// \brief ProfileManager Get the profile manager.
        /// \return The pointer to the profile manager.
        ///
        static Qv2rayBase::Profile::ProfileManager *ProfileManager();

        ///
        /// \brief KernelManager Get the kernel manager.
        /// \return The pointer to the kernel manager.
        ///
        static Qv2rayBase::Profile::KernelManager *KernelManager();

        ///
        /// \brief StorageProvider Get the storage provider, implemented by downstream to provide Qv2rayBase a stable storage to place connections.
        /// \return The pointer to the storage provider
        ///
        static Qv2rayBase::Interfaces::IStorageProvider *StorageProvider();

      signals:
        void _warnInternal(const QString &title, const QString &text, QPrivateSignal);
        void _infoInternal(const QString &title, const QString &text, QPrivateSignal);

      private:
        Q_DECLARE_PRIVATE_D(d_ptr, Qv2rayBaseLibrary)
        QScopedPointer<Qv2rayBaseLibraryPrivate> d_ptr;
    };

} // namespace Qv2rayBase
