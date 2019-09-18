# Qv2ray [![Codacy Badge](https://api.codacy.com/project/badge/Grade/a034dd186c36408c92ffb04449fb6996)](https://app.codacy.com/app/lhy0403/Qv2ray?utm_source=github.com&utm_medium=referral&utm_content=lhy0403/Qv2ray&utm_campaign=Badge_Grade_Dashboard) [![HitCount](http://hits.dwyl.io/lhy0403/Qv2ray.svg)](http://hits.dwyl.io/lhy0403/Qv2ray) 

使用 Qt 框架的跨平台 v2ray 客户端. 支持 Windows, Linux, MacOS

支持连接编辑，支持导入配置和 VMess

# 项目主页，下载链接和使用方法
 - 项目网站: https://lhy0403.github.io/Qv2ray
 - 最新 [Release](https://github.com/lhy0403/Qv2ray/releases/latest) 
 - AppImage 编译平台:  [主分支/Release 版本](https://jenkins.himeki.net/job/Qv2ray-AppImage-master/)
 - Crowdin 翻译平台 (Public Translation Platform): [Crowdin](https://crowdin.com/project/qv2ray) [开始翻译](https://crwd.in/qv2ray)
 - ArchLinux - AUR: [qv2ray](https://aur.archlinux.org/packages/qv2ray/) 由 @aliyuchang33 维护

## Jenkins (Linux)
Himeki.net 的 Jenkins 编译服务器提供开发分支 `dev` 的 Linux AppImage 版本
 - 访问链接: [https://jenkins.himeki.net/job/Qv2ray-AppImage-dev/](https://jenkins.himeki.net/job/Qv2ray-AppImage-dev/)
 - 开发分支仅供测试使用
 - 提供者 [@aliyuchang33](https://github.com/aliyuchang33)
 
-------------------------

Travis per machine badge provided by: [badge-matrix](https://github.com/exogen/badge-matrix)

| OS      | 主分支 [master](https://github.com/lhy0403/Qv2ray/tree/master) | 开发分支 [dev](https://github.com/lhy0403/Qv2ray/tree/dev)   | 版本分支 [v1](https://github.com/lhy0403/Qv2ray/tree/version-v1) |
| ------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| [Linux](https://travis-ci.com/lhy0403/Qv2ray)   | ![Build Status Linux in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-master&branch=master) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-dev&branch=dev) | ![Build Status Linux in v1.0.0](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-v1&branch=version-v1) |
| [MacOS](https://travis-ci.com/lhy0403/Qv2ray)   | ![Build Status MacOS in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-master&branch=master) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-dev&branch=dev) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-v1&branch=version-v1) |
| [Windows](https://ci.appveyor.com/project/lhy0403/qv2ray) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/master?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/dev?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/dev) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/version-v1?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/version-v1) |

----------------

# 编译
 - 依赖项：`Qt >= 5.12` `gcc >=8` 或 `MinGW` (Windows) 或 `clang` (MacOS)
 - 建议使用 QtCreator

## Linux & MacOS

- 对于 MacOS，需要额外使用 HomeBrew 安装 Qt 并正确设定 $PATH 环境变量，或者使用 Qt 官方包 (包含 QtCreator)
- 对于 Linux，请根据不同发行版安装对应的 Qt 开发包 

```bash
git clone --recursive https://github.com/lhy0403/Qv2ray && cd Qv2ray

# 按需要签出开发分支
# git checkout dev

lrelease ./Qv2ray.pro
mkdir build && cd build
qmake ../
make
```

## Windows

- 建议使用 Qt Creator

```batch
REM 首先把 %QTROOT%/bin 和 MinGW/bin 文件夹添加到 PATH 环境变量
REM SET PATH=%PATH%;C:\Qt\5.13.0\mingw73_32\bin\;C:\Qt\Tools\mingw730_32\bin\

SET PATH=%PATH%;Qt安装目录\Qt版本号\编译器类型\bin;Qt安装目录\Tools\编译器类型\bin

git clone --recursive https://github.com/lhy0403/Qv2ray && cd Qv2ray
REM git checkout dev （可选项，用于测试 dev 分支）

mkdir build && cd build
qmake ../
mingw32-make.exe
```

# License

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
