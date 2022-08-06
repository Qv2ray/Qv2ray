<p align="center">
<img width="210" height="210" align="left" style="float: left; margin: 0 10px 0 0;" src="https://raw.githubusercontent.com/Qv2ray/Qv2ray/master/assets/icons/qv2ray.png" alt="Qv2ray"/>
</br>
<h1>Qv2ray - Unleash Your V2Ray</h1> 
使用 Qt 框架的跨平台 V2Ray 客户端。支持 Windows, Linux, macOS
</br>
插件系统支持 SSR / Trojan / Trojan-Go / NaiveProxy
</p>

[![GitHub Releases](https://img.shields.io/github/downloads/Qv2ray/Qv2ray/latest/total?style=flat-square&logo=github)](https://github.com/Qv2ray/Qv2ray/releases)
[![GitHub All Releases](https://img.shields.io/github/downloads/Qv2ray/Qv2ray/total?label=downloads-total&logo=github&style=flat-square)](https://github.com/Qv2ray/Qv2ray/releases)

## Qv2ray 将继续维护
Qv2ray v3.0 维护计划

* Support TUN Mode
* 支持 [V5](https://www.v2fly.org/v5/config/overview.html) 和 [Rust](https://github.com/Qv2ray/v2ray-rust) 内核
* 自动检查并更新 Core 文件

-----

## Availability & CI/CD Status

[![Snap Status](https://img.shields.io/travis/com/Qv2ray/Qv2ray?label=snapcraft-travis&logo=github)](https://travis-ci.com/Qv2ray/Qv2ray)
[![Qv2ray build debian package](https://github.com/Qv2ray/Qv2ray/actions/workflows/deb.yml/badge.svg)](https://github.com/Qv2ray/Qv2ray/actions/workflows/deb.yml)
[![Qv2ray Windows Installer](https://github.com/Qv2ray/Qv2ray/actions/workflows/nsis.yml/badge.svg)](https://github.com/Qv2ray/Qv2ray/actions/workflows/nsis.yml)
[![Qv2ray build matrix - cmake](https://github.com/Qv2ray/Qv2ray/actions/workflows/build-qv2ray-cmake.yml/badge.svg)](https://github.com/Qv2ray/Qv2ray/actions/workflows/build-qv2ray-cmake.yml)
[![Qv2ray build matrix - Qt6](https://github.com/Qv2ray/Qv2ray/actions/workflows/build-qv2ray-qt6.yml/badge.svg)](https://github.com/Qv2ray/Qv2ray/actions/workflows/build-qv2ray-qt6.yml)

<table>
 <tr>
  <td>ArchLinuxCN</td>
  <td>
   <a href="https://build.archlinuxcn.org/packages/#/qv2ray">
    <img alt="ArchlinuxCN Stable" src="https://img.shields.io/badge/dynamic/json?label=archlinuxcn-stable&query=%24.latest.pkgver&url=https%3A%2F%2Fbuild.archlinuxcn.org%2Fapi%2Fpackages%2Fqv2ray" />
   </a><br />

   <a href="https://build.archlinuxcn.org/packages/#/qv2ray-dev-git">
    <img alt="ArchlinuxCN Development" src="https://img.shields.io/badge/dynamic/json?label=archlinuxcn-dev-git&query=%24.latest.pkgver&url=https%3A%2F%2Fbuild.archlinuxcn.org%2Fapi%2Fpackages%2Fqv2ray-dev-git" />
   </a>
  </td>
 </tr>

 <tr>
 <td>AUR</td>
 <td>
   <a href="https://aur.archlinux.org/packages/qv2ray">
    <img alt="AUR Stable" src="https://img.shields.io/aur/version/qv2ray?label=aur-stable" />
   </a><br/>

   <a href="https://aur.archlinux.org/packages/qv2ray-dev-git">
    <img alt="AUR Development" src="https://img.shields.io/aur/version/qv2ray-dev-git?label=aur-development" /></a>
 </td>
 </tr>

 <tr>
  <td>Fedora</td>
  <td>
   <a href="https://build.opensuse.org/package/show/home:zzndb:Qv2ray/Qv2ray">
    <img alt="OBS Stable" src="https://img.shields.io/badge/dynamic/xml?color=3c6eb4&label=OBS%20stable&query=substring-before%28substring-after%28%2F%2F%40filename%5Bcontains%28.%2C%20%27src.rpm%27%29%5D%2C%20%27Qv2ray-%27%29%2C%20%27-%27%29&url=https%3A%2F%2Fapi.opensuse.org%2Fpublic%2Fbuild%2Fhome%3Azzndb%3AQv2ray%2FFedora_Rawhide%2Fx86_64%2FQv2ray" />
   </a><br/>
   <a href="https://build.opensuse.org/package/show/home:zzndb:Qv2ray/Qv2ray-preview">
    <img alt="OBS Preview" src="https://img.shields.io/badge/dynamic/xml?color=3c6eb4&label=OBS%20preview&query=substring-before%28substring-after%28%2F%2F%40filename%5Bcontains%28.%2C%20%27src.rpm%27%29%5D%2C%20%27Qv2ray-preview-%27%29%2C%20%27-%27%29&url=https%3A%2F%2Fapi.opensuse.org%2Fpublic%2Fbuild%2Fhome%3Azzndb%3AQv2ray%2FFedora_Rawhide%2Fx86_64%2FQv2ray-preview" />
   </a>
  </td>
 </tr>

 <tr>
  <td>Flathub</td>
  <td><a href="https://flathub.org/apps/details/com.github.Qv2ray"><img alt="flathub" src="https://img.shields.io/badge/flathub-available-success" /></a></td>
 </tr>

 <tr>
  <td>Scoop</td>
  <td><a href="https://github.com/lukesampson/scoop-extras/blob/master/bucket/qv2ray.json"><img alt="scoop" src="https://img.shields.io/badge/scoop--extras-available-blue" /></a></td>
 </tr>

 <tr>
 <td>Chocolatey</td>
 <td><a href="https://chocolatey.org/packages/qv2ray"><img alt="Chocolatey Version" src="https://img.shields.io/chocolatey/v/qv2ray"></a></td>
 </tr>

 <tr>
  <td>Snapcraft</td>
  <td>
   <a href="https://snapcraft.io/qv2ray/"><img alt="snap" src="https://snapcraft.io/qv2ray/badge.svg" /></a>
  </td>
 </tr>

 <tr>
  <td>openSUSE</td>
  <td>
   <a href="https://build.opensuse.org/package/show/home:zzndb:Qv2ray/Qv2ray">
    <img alt="OBS Stable" src="https://img.shields.io/badge/dynamic/xml?color=73ba25&label=OBS%20stable&query=substring-before%28substring-after%28%2F%2F%40filename%5Bcontains%28.%2C%20%27src.rpm%27%29%5D%2C%20%27Qv2ray-%27%29%2C%20%27-%27%29&url=https%3A%2F%2Fapi.opensuse.org%2Fpublic%2Fbuild%2Fhome%3Azzndb%3AQv2ray%2FopenSUSE_Tumbleweed%2Fx86_64%2FQv2ray" />
   </a><br/>
   <a href="https://build.opensuse.org/package/show/home:zzndb:Qv2ray/Qv2ray-preview">
    <img alt="OBS Stable" src="https://img.shields.io/badge/dynamic/xml?color=73ba25&label=OBS%20preview&query=substring-before%28substring-after%28%2F%2F%40filename%5Bcontains%28.%2C%20%27src.rpm%27%29%5D%2C%20%27Qv2ray-preview-%27%29%2C%20%27-%27%29&url=https%3A%2F%2Fapi.opensuse.org%2Fpublic%2Fbuild%2Fhome%3Azzndb%3AQv2ray%2FopenSUSE_Tumbleweed%2Fx86_64%2FQv2ray-preview" />
   </a>
  </td>
 </tr>
</table>

Translation Platform: **[Crowdin](https://crowdin.com/project/qv2ray)**

## Special Thanks

**[JetBrains Developer Toolbox](https://www.jetbrains.com/?from=Qv2ray)**

-------------------------------

## Licences

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

![Star](https://api.star-history.com/svg?repos=Qv2ray/Qv2ray&type=Date)
