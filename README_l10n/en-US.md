***Note: Qv2ray can only be used for learning related technologies such as Qt/c++/linux/CI/automation and use within the scope permitted by law. Any individual or group may not use Qv2ray for any violation of relevant laws and regulations.***

*Any attempt to download or download any branch or distribution of Qv2ray constitutes your agreement that the author of the project will not be liable for any legal liability arising from your breach of the above guidelines.*

# Qv2ray

[![HitCount](http://hits.dwyl.io/lhy0403/Qv2ray.svg)](http://hits.dwyl.io/lhy0403/Qv2ray) ![GitHub Releases](https://img.shields.io/github/downloads/lhy0403/Qv2ray/latest/total?style=flat-square)

The v2ray GUI client using Qt, supports Windows, Linux and macOS

Supports editing connection config, importing any file and `vmess://` shared link.

Special Thanks to [Hv2ray](https://github.com/aliyuchang33/Hv2ray) by [@aliyuchang33](https://github.com/aliyuchang33)

## Relevant Links

 - **Brief Feature Introduction: https://lhy0403.github.io/Qv2ray**
 - Latest **[Release Distribution](https://github.com/lhy0403/Qv2ray/releases/latest)**
 - Crowdin Translation Platform: **[Public Translation Platform](https://crwd.in/qv2ray)**
 - ArchLinux - AUR: **[qv2ray](https://aur.archlinux.org/packages/qv2ray/)**
    - **[qv2ray-dev-git](https://aur.archlinux.org/packages/qv2ray-dev-git)**: Development version from branch`dev`, maintained by **[@axionl](https://github.com/axionl)** 
    - **Note: The development branch contains unstable feature update and (even more) unstable bug fixes, it's not recommended to use a development version in production.**

### First time using Qv2ray please refer to ➡ [User Manual](https://lhy0403.github.io/Qv2ray)



## Qv2ray Project Status

### Jenkins CI 

- Server Website: [https://jenkins.lhy0403.top/](https://jenkins.lhy0403.top/)

|                | [Master](https://github.com/lhy0403/Qv2ray/tree/master)      | [Developement](https://github.com/lhy0403/Qv2ray/tree/dev)   | [Version v1](https://github.com/lhy0403/Qv2ray/tree/version-v1) | [Version v2](https://github.com/lhy0403/Qv2ray/tree/version-v2) | Testing                                                      |
| -------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Linux AppImage | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Release/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Release/) | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Dev/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Dev/) | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Version1/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Version1/) | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Version2/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Version2/) | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Testing/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-AppImage-Testing/) |
| Windows Zip    | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Release/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Release/) | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Dev/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Dev/) | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Version1/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Version1/) | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Version2/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Version2/) | [![Build Status](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Testing/badge/icon)](https://jenkins.lhy0403.top/job/Qv2ray-Win32-Testing/) |

### Travis & Appveyor Cross Platform Build Status

*Travis per machine badge provided by: [badge-matrix](https://github.com/exogen/badge-matrix)*

| OS                                                        | [master](https://github.com/lhy0403/Qv2ray/tree/master)      | [dev](https://github.com/lhy0403/Qv2ray/tree/dev)            | [version-v1](https://github.com/lhy0403/Qv2ray/tree/version-v1) | [version-v2](https://github.com/lhy0403/Qv2ray/tree/version-v2) |
| --------------------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| [Linux](https://travis-ci.com/lhy0403/Qv2ray)             | ![Build Status Linux in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-master&branch=master) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-dev&branch=dev) | ![Build Status Linux in v1.0.0](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-v1&branch=version-v1) | ![Build Status Linux in v2.0.0](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-v2&branch=version-v2) |
| [macOS](https://travis-ci.com/lhy0403/Qv2ray)             | ![Build Status macOS in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=macOS-master&branch=master) | ![Build Status macOS in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=macOS-dev&branch=dev) | ![Build Status macOS in v1](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=macOS-v1&branch=version-v1) | ![Build Status macOS in v2](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=macOS-v2&branch=version-v2) |
| [Windows](https://ci.appveyor.com/project/lhy0403/qv2ray) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/master?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/dev?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/dev) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/version-v1?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/version-v1) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/version-v2?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/version-v2) |



## Special Thanks

| NickName (@GithubAccount)                                    | Contributions                  |
| ------------------------------------------------------------ | ------------------------------ |
| Leroy.H.Y [@lhy0403](https://github.com/lhy0403)             | Qv2ray Current Maintainer      |
| Hork [@aliyuchang33](https://github.com/aliyuchang33)        | Hv2ray Initial Idea and Design |
| SOneWinstone [@SoneWinstone](https://github.com/SoneWinstone) | Qt HTTP Interactive Logics     |
| ArielAxionL [@axionl](https://github.com/axionl)             | Qv2ray Artworks                |
| TheBadGateway [@thebadgateway](https://github.com/thebadgateway) | Translation for Russian        |



## Open Source License

Qv2ray is licensed under [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0) 

Submodule [X2Struct](https://github.com/xyz347/x2struct) is licensed under ![License: GPL v3](https://img.shields.io/badge/License-MIT-blue.svg)

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