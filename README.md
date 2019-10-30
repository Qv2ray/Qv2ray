# README in Other Languages

- [English](./README_l10n/en-US.md)

---------------------

***注意：Qv2ray 仅能用于 Qt/c++/linux/CI/自动化 等相关技术的学习和在法律允许范围内的使用，任何个人或集体不得使用 Qv2ray 进行任何违反相关法律法规的操作。***

*任何尝试下载或下载 Qv2ray 任意分支或发行版即代表您同意本项目作者不承担任何由于您违反以上准则所带来的任何法律责任。*

# Qv2ray - Make v2ray real cross-platform

[![HitCount](http://hits.dwyl.io/lhy0403/Qv2ray.svg)](http://hits.dwyl.io/lhy0403/Qv2ray) ![GitHub Releases](https://img.shields.io/github/downloads/lhy0403/Qv2ray/latest/total?style=flat-square)

使用 Qt 框架的跨平台 v2ray 客户端. 支持 Windows, Linux, macOS

支持连接编辑，支持导入任意配置和 `vmess://`  分享链接

来自于 [Hv2ray](https://github.com/aliyuchang33/Hv2ray)，原作者 [@aliyuchang33](https://github.com/aliyuchang33)

## 相关链接：

 - **功能介绍: https://lhy0403.github.io/Qv2ray**
 - 最新 **[Release 发布版](https://github.com/lhy0403/Qv2ray/releases/latest)**
 - Crowdin 翻译平台, Translations are welcome at here: **[Public Translation Platform](https://crwd.in/qv2ray)**
 - ArchLinux - AUR: **[qv2ray](https://aur.archlinux.org/packages/qv2ray/)**
    - **[qv2ray-dev-git](https://aur.archlinux.org/packages/qv2ray-dev-git)**: `dev` 分支的开发版本，由 **[@axionl](https://github.com/axionl)** 维护
    - **注意：dev 分支包含不稳定的功能更新和（更）不稳定的bug修复，不建议将 dev 分支作为日常使用**

### 首次使用请查看 ➡ [用户手册](https://lhy0403.github.io/Qv2ray)



## Qv2ray 项目状态

### Jenkins CI 

- 访问链接: [https://jenkins.lhy0403.top/](https://jenkins.lhy0403.top/)

|                | [Master](https://github.com/lhy0403/Qv2ray/tree/master)      | [Version v1](https://github.com/lhy0403/Qv2ray/tree/version-v1) |
| -------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Linux AppImage | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Release/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Release/) | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Version1/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Version1/) |
| Windows Zip    | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Release/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Release/) | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Version1/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Version1/) |

### Travis & Appveyor 多平台构建状态

*Travis per machine badge provided by: [badge-matrix](https://github.com/exogen/badge-matrix)*

| OS                                                        | 主分支 [master](https://github.com/lhy0403/Qv2ray/tree/master) | 版本分支 [version-v1](https://github.com/lhy0403/Qv2ray/tree/version-v1) |
| --------------------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| [Linux](https://travis-ci.com/lhy0403/Qv2ray)             | ![Build Status Linux in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-master&branch=master) | ![Build Status Linux in v1.0.0](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-v1&branch=version-v1) |
| [macOS](https://travis-ci.com/lhy0403/Qv2ray)             | ![Build Status macOS in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=macOS-master&branch=master) | ![Build Status macOS in v1](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=macOS-v1&branch=version-v1) |
| [Windows](https://ci.appveyor.com/project/lhy0403/qv2ray) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/master?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/version-v1?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/version-v1) |



## 鸣谢

| 姓名 (@Github帐号)                                           | 贡献内容                 |
| ------------------------------------------------------------ | ------------------------ |
| Leroy.H.Y [@lhy0403](https://github.com/lhy0403)             | Qv2ray 当前维护人员之一  |
| Hork [@aliyuchang33](https://github.com/aliyuchang33)        | Hv2ray 灵感与设计        |
| SOneWinstone [@SoneWinstone](https://github.com/SoneWinstone) | 使用 Qt 的 HTTP 交互逻辑 |
| ArielAxionL [@axionl](https://github.com/axionl)             | Qv2ray 部分 UI           |
| TheBadGateway [@thebadgateway](https://github.com/thebadgateway) | 俄罗斯文翻译             |




## 许可证

Qv2ray 使用 [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0) 

Submodule [X2Struct](https://github.com/xyz347/x2struct) 使用 ![License: GPL v3](https://img.shields.io/badge/License-MIT-blue.svg)

[QJsonModel](https://github.com/dridk/QJsonModel) is licensed under![License: GPL v3](https://img.shields.io/badge/License-MIT-blue.svg) (Copyright (c) 2011 SCHUTZ Sacha)

```
    Qv2ray, A Qt frontend for v2ray. Written in c++

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
