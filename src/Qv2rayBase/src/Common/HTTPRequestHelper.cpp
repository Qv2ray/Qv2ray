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

#include "Qv2rayBase/Common/HTTPRequestHelper.hpp"

#include "Qv2rayBase/Common/Settings.hpp"
#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"

#include <QByteArray>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>

namespace Qv2rayBase::Utils
{
    void NetworkRequestHelper::setHeader(QNetworkRequest &request, const QByteArray &key, const QByteArray &value)
    {
        qDebug() << "Adding HTTP request header:" << key << ":" << value;
        request.setRawHeader(key, value);
    }

    void NetworkRequestHelper::setAccessManagerAttributes(QNetworkRequest &request, QNetworkAccessManager &accessManager)
    {
        const auto &netconf = Qv2rayBaseLibrary::GetConfig()->network_config;
        switch (netconf.type)
        {
            case Models::NetworkProxyConfig::PROXY_NONE:
            {
                qDebug() << "Get without proxy.";
                accessManager.setProxy(QNetworkProxy(QNetworkProxy::ProxyType::NoProxy));
                break;
            }
            case Models::NetworkProxyConfig::PROXY_SYSTEM:
            {
                accessManager.setProxy(QNetworkProxyFactory::systemProxyForQuery().constFirst());
                break;
            }
            case Models::NetworkProxyConfig::PROXY_HTTP:
            {
                QNetworkProxy p{ QNetworkProxy::HttpProxy, netconf.address, quint16(netconf.port) };
                accessManager.setProxy(p);
                break;
            }
            case Models::NetworkProxyConfig::PROXY_SOCKS5:
            {
                QNetworkProxy p{ QNetworkProxy::Socks5Proxy, netconf.address, quint16(netconf.port) };
                p.setCapabilities(p.capabilities() | QNetworkProxy::HostNameLookupCapability);
                accessManager.setProxy(p);
                break;
            }
            default: Q_UNREACHABLE();
        }

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        auto ua = netconf.ua;
        ua.replace(u"$VERSION"_qs, QV2RAY_BASELIB_VERSION);
        request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, ua);
    }

    Qv2rayPlugin::Utils::INetworkRequestHelper::GetResult NetworkRequestHelper::StaticGet(const QUrl &url, const EncryptedCallback &callback)
    {
        QNetworkRequest request;
        QNetworkAccessManager accessManager;
        request.setUrl(url);
        setAccessManagerAttributes(request, accessManager);

        {
            QEventLoop loop;
#ifndef QT_NO_SSL
            if (callback)
                QObject::connect(&accessManager, &QNetworkAccessManager::encrypted, callback);
#else
            Q_UNUSED(callback)
#endif
            auto reply = accessManager.get(request);
            QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            QObject::connect(reply, &QNetworkReply::errorOccurred, &loop, &QEventLoop::quit);
            loop.exec();

            qInfo() << reply->error();
            return { reply->error(), reply->errorString(), reply->readAll() };
        }
    }

    Qv2rayPlugin::Utils::INetworkRequestHelper::GetResult NetworkRequestHelper::Get(const QUrl &url, const EncryptedCallback &onEncrypted)
    {
        return StaticGet(url, onEncrypted);
    }

    void NetworkRequestHelper::StaticAsyncGet(const QString &url, QObject *ctx,
                                              const std::function<void(const Qv2rayPlugin::Utils::INetworkRequestHelper::GetResult &)> &func)
    {
        QNetworkRequest request;
        request.setUrl(url);
        auto accessManagerPtr = new QNetworkAccessManager();
        setAccessManagerAttributes(request, *accessManagerPtr);
        auto reply = accessManagerPtr->get(request);
        QObject::connect(reply, &QNetworkReply::finished, ctx,
                         [=]()
                         {
                             func({ reply->error(), reply->errorString(), reply->readAll() });
                             accessManagerPtr->deleteLater();
                         });
    }
} // namespace Qv2rayBase::Utils
