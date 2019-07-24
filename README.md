# Qv2ray 

使用 Qt 的跨平台 v2ray 客户端.

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/a034dd186c36408c92ffb04449fb6996)](https://app.codacy.com/app/lhy0403/Qv2ray?utm_source=github.com&utm_medium=referral&utm_content=lhy0403/Qv2ray&utm_campaign=Badge_Grade_Dashboard) [![HitCount](http://hits.dwyl.io/lhy0403/Qv2ray.svg)](http://hits.dwyl.io/lhy0403/Qv2ray) 

Travis (Linux and MacOS): https://travis-ci.com/lhy0403/Qv2ray

Appveyor (Windows): https://ci.appveyor.com/project/lhy0403/qv2ray

Travis per machine badge provided by: https://github.com/exogen/badge-matrix ( @exogen )

| OS      | 主分支 [master](https://github.com/lhy0403/Qv2ray/tree/master) | 开发分支 [dev](https://github.com/lhy0403/Qv2ray/tree/dev)   | 版本分支 [v1](https://github.com/lhy0403/Qv2ray/tree/version-v1) |
| ------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Linux   | ![Build Status Linux in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-master&branch=master) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-dev&branch=dev) | ![Build Status Linux in v1.0.0](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-v1&branch=version-v1) |
| MacOS   | ![Build Status MacOS in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-master&branch=master) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-dev&branch=dev) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-v1&branch=version-v1) |
| Windows | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/master?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/dev?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/dev) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/version-v1?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/version-v1) |

## 1. 依赖

- 运行依赖：

  - ArchLinux

    - 在 `v2ray` 包的基础上 额外需要 `v2ray-domain-list-community` 和 `v2ray-geoip` 进行分流代理

      Issue: https://github.com/lhy0403/Qv2ray/issues/23#issuecomment-511384858 

- 编译依赖

  - Qt >= 5.12
  - gcc >=8 (Linux 需要 c++11 支持) 或 MinGW (Windows) 或 clang (MacOS)
  - QtCreator (推荐)

## 2. 编译方法

 - 详见 [BUILD.md](BUILD.md)

## 3. 最近更新内容

### a. 最新

**2019-07-25**: 发布功能更新 [v1.2.0](https://github.com/lhy0403/Qv2ray/releases/tag/v1.2.0)

**2019-07-24**: Windows 版本修复了难看的字体

**2019-07-24**:

1. 增加使用 Github Release API 的自动检测更新机制

2. 改善修改配置后的重连机制，部分修改不需要重新连接

3. 增加了启动时的 GPLv3 版权显示

4. 增强选择 v2ray 传输方式时的 UX

5. 增强 v2ray Assets 目录的自动识别机制

6. 修复导入配置时放弃按钮误识别为确认按钮

7. 修复日志窗口在光标位于内容中间时的错乱问题

8. 增强日志自动滚动。

   **[同时更新版本号到 v1.2]**

**2019-07-23**: v1.1 版本添加了自动启动功能

**2019-07-11**: 准备 dev 分支开始 v2.0 版本

### b. 历史

**v1.0 更新日志请见 [ChangeLog-v1](./ChangeLogs/CHANGELOG-v1.0.md)**

### c. 更多内容

- 详见 [dev 开发分支](https://github.com/lhy0403/Qv2ray/tree/dev) 或 [Release 发布版](https://github.com/lhy0403/Qv2ray/releases)

----------------------

## 4. License

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
