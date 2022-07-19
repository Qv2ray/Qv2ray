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

#include "Qv2rayBase/Qv2rayBaseFeatures.hpp"
#include "QvPlugin/Utils/JsonConversion.hpp"

#include <QJsonObject>
#include <QMap>
#include <QString>

constexpr int QV2RAY_SETTINGS_VERSION = 16;

namespace Qv2rayBase::Models
{
    struct NetworkProxyConfig
    {
        enum ProxyType
        {
            PROXY_NONE,
            PROXY_SYSTEM,
            PROXY_HTTP,
            PROXY_SOCKS5,
        };

        ProxyType type = PROXY_SYSTEM;
        QString address = "127.0.0.1";
        int port = 8000;
        QString ua = "Qv2rayBase/$VERSION WebRequestHelper";
        QJS_JSON(F(type, address, port, ua))
    };

    struct PluginConfigObject
    {
        int plugin_port_allocation = 15490;
        QMap<QString, bool> plugin_states;
        QJS_JSON(F(plugin_port_allocation, plugin_states))
    };

    struct Qv2rayBaseConfigObject
    {
        int config_version = QV2RAY_SETTINGS_VERSION;
        NetworkProxyConfig network_config;
        PluginConfigObject plugin_config;
        QJsonObject extra_options;
        QJS_JSON(F(config_version, network_config, plugin_config, extra_options))
    };
} // namespace Qv2rayBase::Models
