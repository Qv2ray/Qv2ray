# Qv2ray

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/a034dd186c36408c92ffb04449fb6996)](https://app.codacy.com/app/lhy0403/Qv2ray?utm_source=github.com&utm_medium=referral&utm_content=lhy0403/Qv2ray&utm_campaign=Badge_Grade_Dashboard) [![HitCount](http://hits.dwyl.io/lhy0403/Qv2ray.svg)](http://hits.dwyl.io/lhy0403/Qv2ray) 

使用 Qt 框架的跨平台 v2ray 客户端. 支持 Windows, Linux, MacOS

支持连接编辑，支持导入配置和 VMess

Special thanks to: [Hv2ray](https://github.com/aliyuchang33/Hv2ray) by [@aliyuchang33](https://github.com/aliyuchang33)

 - **项目网站: https://lhy0403.github.io/Qv2ray**
 - **最新 [Release](https://github.com/lhy0403/Qv2ray/releases/latest)** 
 - **AppImage 编译平台:  [主分支/Release 版本](https://jenkins.himeki.net/job/Qv2ray-AppImage-master/)**
 - **Crowdin 翻译平台 (Public Translation Platform): [开始翻译](https://crwd.in/qv2ray)**
 - **ArchLinux - AUR: [qv2ray](https://aur.archlinux.org/packages/qv2ray/) 由 [@aliyuchang33](https://github.com/aliyuchang33) 维护**



## Jenkins AppImage (Linux)

Himeki.net 的 Jenkins 编译服务器提供 `master` `dev` 和 `version-v1` 分支的 Linux AppImage 版本

- 访问链接: [https://jenkins.himeki.net/](https://jenkins.himeki.net/)
- 提供者 [@aliyuchang33](https://github.com/aliyuchang33)



## Qv2ray 项目状态

### Linux AppImage

| Master                                                       | Development                                                  | Versioning - v1                                              | Pull Requests                                                |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| [![Build Status](https://jenkins.himeki.net/job/Qv2ray-AppImage-master/badge/icon)](https://jenkins.himeki.net/job/Qv2ray-AppImage-master/) | [![Build Status](https://jenkins.himeki.net/job/Qv2ray-AppImage-dev/badge/icon)](https://jenkins.himeki.net/job/Qv2ray-AppImage-dev/) | [![Build Status](https://jenkins.himeki.net/job/Qv2ray-AppImage-version-v1/badge/icon)](https://jenkins.himeki.net/job/Qv2ray-AppImage-version-v1/) | [![Build Status](https://jenkins.himeki.net/job/Qv2ray-AppImage-PR/badge/icon)](https://jenkins.himeki.net/job/Qv2ray-AppImage-PR/) |

### 跨平台 CI

*Travis per machine badge provided by: [badge-matrix](https://github.com/exogen/badge-matrix)*

| OS                                                        | 主分支 [master](https://github.com/lhy0403/Qv2ray/tree/master) | 开发分支 [dev](https://github.com/lhy0403/Qv2ray/tree/dev)   | 版本分支 [version-v1](https://github.com/lhy0403/Qv2ray/tree/version-v1) |
| --------------------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| [Linux](https://travis-ci.com/lhy0403/Qv2ray)             | ![Build Status Linux in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-master&branch=master) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-dev&branch=dev) | ![Build Status Linux in v1.0.0](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-v1&branch=version-v1) |
| [MacOS](https://travis-ci.com/lhy0403/Qv2ray)             | ![Build Status MacOS in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-master&branch=master) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-dev&branch=dev) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-v1&branch=version-v1) |
| [Windows](https://ci.appveyor.com/project/lhy0403/qv2ray) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/master?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/dev?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/dev) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/version-v1?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/version-v1) |


## 编译


 - 依赖项：`Qt >= 5.12` `gcc >=8` 或 `MinGW` (Windows) 或 `clang` (MacOS)
 - 建议使用 QtCreator

### Linux & MacOS

- 对于 MacOS，需要额外使用 HomeBrew 安装 Qt 并正确设定 $PATH 环境变量，或者使用 Qt 官方包 (包含 QtCreator)
- 对于 Linux，请根据不同发行版安装对应的 Qt 开发包 (对于 Arch Linux 用户也可从 [qv2ray-dev-git](https://aur.archlinux.org/packages/qv2ray-dev-git) 拉取 `dev` 分支并构建)

```bash
git clone --recursive https://github.com/lhy0403/Qv2ray && cd Qv2ray

# 按需要签出开发分支
# git checkout dev

lrelease ./Qv2ray.pro
mkdir build && cd build
qmake ../
make
```

### Windows

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



## Contributors

```
 Leroy.H.Y (@lhy0403)            ---> Qv2ray Current Maintainer
 Hork (@aliyuchang33)	         ---> Hv2ray Initial Idea and Designs   
 SOneWinstone (@SoneWinstone)    ---> Hv2ray/Qv2ray HTTP Request Helper
 ArielAxionL (@axionl)		 ---> Qv2ray ArtWork
 TheBadGateway (@thebadgateway)  ---> Russian Translation
```



## License

Qv2ray is licensed under [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0) 

[X2Struct](https://github.com/xyz347/x2struct) is licensed ![License: GPL v3](https://img.shields.io/badge/License-MIT-blue.svg)

```
    Qv2ray, A Qt frontend for v2ray. Written in c++
    Copyright (C) 2019 Leroy.H.Y (@lhy0403)         ---> Qv2ray Current Maintainer
    Copyright (C) 2019 Hork (@aliyuchang33)	        ---> Hv2ray Initial Idea and Designs   
    Copyright (C) 2019 SOneWinstone (@SoneWinstone) ---> Hv2ray/Qv2ray HTTP Request Helper
	Copyright (C) 2019 ArielAxionL (@axionl)		---> Qv2ray ArtWork
    
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

注意：Qv2ray 仅能用于 Qt/c++/linux/CI/自动化 等相关技术的学习和在法律允许范围内的使用，任何个人或集体不得使用 Qv2ray 进行任何违反相关法律法规的操作。

启动 Qv2ray 即代表您同意本项目作者不承担任何由于您违反以上准则所带来的任何法律责任。
