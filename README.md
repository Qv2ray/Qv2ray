# Qv2ray 

使用 Qt 的跨平台 v2ray 客户端.

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/a034dd186c36408c92ffb04449fb6996)](https://app.codacy.com/app/lhy0403/Qv2ray?utm_source=github.com&utm_medium=referral&utm_content=lhy0403/Qv2ray&utm_campaign=Badge_Grade_Dashboard) [![HitCount](http://hits.dwyl.io/lhy0403/Qv2ray.svg)](http://hits.dwyl.io/lhy0403/Qv2ray) 

| OS      | 主分支 [master](https://github.com/lhy0403/Qv2ray/tree/master) | 开发分支 [dev](https://github.com/lhy0403/Qv2ray/tree/dev)   |
| ------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Linux   | ![Build Status Linux in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-master&branch=master) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-dev&branch=dev) |
| MacOS   | ![Build Status MacOS in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-master&branch=master) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-dev&branch=dev) |
| Windows | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/master?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/dev?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/dev) |

## 项目依赖包

- Qt >= 5.12
- gcc >=8 (Linux 需要 c++11 支持) 或 MinGW (Windows) 或 clang (MacOS)
- QtCreator (推荐)

## 最近更新内容

详见 开发分支 [dev](https://github.com/lhy0403/Qv2ray/tree/dev) 或 [Release](https://github.com/lhy0403/Qv2ray/releases)

### 最新

**2019-07-09**: 发布第一个公开测试版本 [v0.9.9b](https://github.com/lhy0403/Qv2ray/releases/tag/v0.9.9b)

### 历史

**2019-07-08**: dev 分支的 [`v0.9.2a`](https://github.com/lhy0403/Qv2ray/releases/tag/v0.9.2a) 版本现在可以使用 GUI 修改配置，并做到动态重载配置（包括入站设置，日志，Mux选项）此版本完成了所有翻译工作，添加了双击配置列表即可启动对应配置的功能

**2019-07-07**: [Commit: [9b02ff](https://github.com/lhy0403/Qv2ray/commit/9b02ff9da8f96325bafa08958ba12c0dff66e715) ] 现在可以启动导入的配置文件 (包括导入现有文件和 `vmess://` 协议)，手动添加配置尚未实现，入站设置现在只能通过编辑配置文件完成 (Linux: `~/.qv2ray/Qv2ray.conf`, MacOS & Windows: 程序当前文件夹)，此版本部分翻译不完整

**2019-07-04**: 我们终于摆脱了对于 Python 的依赖，现在 Qv2ray 可以自行解析 `vmess://` 协议 [WIP]

**2019-07-03**: 主配置文件序列化/反序列化工作完成，并添加更多协议配置

**2019-07-02**: 等待上游依赖完成更新 [JSON 序列化 std::list](https://github.com/xyz347/x2struct/issues/11#issuecomment-507671091)

**2019-07-01**: 休息了几天，主要是去关注别的项目了。现在开始重构 v2ray 交互部分。

**2019-06-24**: Mac OS 测试构建完成，合并到开发分支 [dev](https://github.com/lhy0403/Qv2ray/tree/dev)

**2019-06-24**: 新建分支 MacOS-Build 开始测试 MacOS 构建，当前状态：![Build Status](https://travis-ci.com/lhy0403/Qv2ray.svg?branch=MacOS-Build)

**2019-06-23**: UI 结构已经固定，新建分支 translations 进行翻译 UI

**2019-06-23**: 基本 UI 完成，切换到 [dev](https://github.com/lhy0403/Qv2ray/tree/dev) 分支进行代码实现

**2019-06-22**: 当前开发分支[ui-implementation](https://github.com/lhy0403/Qv2ray/tree/ui-implementation) - 用于实现基本 UI

----------------------


## 编译
 - 详见 [BUILD.md](BUILD.md)
## License

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
