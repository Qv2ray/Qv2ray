# <img width="32" src="assets/icons/qv2ray.png" alt="Qv2ray"/> Qv2ray - For developers. By developers.

[![Qv2ray build matrix - Qt6](https://github.com/Shadowsocks-NET/Qv2ray/actions/workflows/build-qv2ray-qt6.yml/badge.svg)](https://github.com/Shadowsocks-NET/Qv2ray/actions/workflows/build-qv2ray-qt6.yml)
[![GitHub Releases](https://img.shields.io/github/downloads/Shadowsocks-NET/Qv2ray/latest/total?style=flat-square&logo=github)](https://github.com/Shadowsocks-NET/Qv2ray/releases)
[![GitHub All Releases](https://img.shields.io/github/downloads/Shadowsocks-NET/Qv2ray/total?label=downloads-total&logo=github&style=flat-square)](https://github.com/Shadowsocks-NET/Qv2ray/releases)

Qv2ray is a cross-platform Qt frontend for V2Ray.

This is the original Qv2ray project by the author who contributed over 95% code and other contributors.
Unlike Qv2ray/Qv2ray whose control has been forcibly taken away from the author by another organization owner.

## Development Goal: For developers. By developers.

Further development of this project will focus on my own needs and use cases.

Feature requests from non-developers might get rejected. Code contribution is always welcome.

## Compatibility

This repo is developed based on the original `dev-v3` branch, so the plugins, configuration
files will not be compatible with the original version.

### Xray

No Such Word In My Dictionary

## Plugins At A Glance

~~*Not All Plugins Are Available Since I'm Very Busy*~~

### Built-in Plugins
- **QvPlugin-BuiltinLatencyTest** Latency Tests Plugin (ICMP, TCP, HTTPS)
- **QvPlugin-BuiltinProtocolSupport** Builtin Protocol Support
- **QvPlugin-BuiltinSubscriptionSupport** Builtin Subscription Support
- **QvPlugin-BuiltinUtils** Builtin Utilities

### Utility Plugins
- **QvPlugin-Command** Command Hook Plugin
- **QvPlugin-RemoteCore** Remove V2Ray Core Plugin
- **QvPlugin-WindowsToolbar** Windows Taskband & Performance Counter Provider
- **QvPlugin-WinNetFilter** Windows Transparent Proxy Plugin

### Protocol Support
- ~~**QvPlugin-SSR** ShadowsocksR Plugin~~ Removed, see [Shadowsocks-NET/QvPlugins-v4](https://github.com/Shadowsocks-NET/QvPlugins-v4)
- ~~**QvPlugin-SS** Shadowsocks Plugin~~ Removed, see [Shadowsocks-NET/QvPlugins-v4](https://github.com/Shadowsocks-NET/QvPlugins-v4)
- **QvPlugin-Trojan-Go** Trojan-Go Plugin
- ~~**QvPlugin-Trojan** Trojan Plugin~~ Removed, please use V2Ray instead, see [Shadowsocks-NET/QvPlugins-v4](https://github.com/Shadowsocks-NET/QvPlugins-v4)
- **QvPlugin-NaiveProxy** NaiveProxy Plugin

## Licencing

This is free software, and you are welcome to redistribute it under certain conditions.

### Third-party Libraries and Resources
See: [assets/credit.html](assets/credit.html)

Qv2ray is licenced under [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

```
Qv2ray, A Qt frontend for V2Ray. Written in C++

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
```
## Star History

![stars](https://starchart.cc/Shadowsocks-NET/Qv2ray.svg)
