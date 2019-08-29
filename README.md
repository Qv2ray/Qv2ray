# Qv2ray [![Codacy Badge](https://api.codacy.com/project/badge/Grade/a034dd186c36408c92ffb04449fb6996)](https://app.codacy.com/app/lhy0403/Qv2ray?utm_source=github.com&utm_medium=referral&utm_content=lhy0403/Qv2ray&utm_campaign=Badge_Grade_Dashboard) [![HitCount](http://hits.dwyl.io/lhy0403/Qv2ray.svg)](http://hits.dwyl.io/lhy0403/Qv2ray) 

使用 Qt 的跨平台 v2ray 客户端.

## 项目主页地址和使用方法
- https://lhy0403.github.io/Qv2ray

-------------------------

Travis per machine badge provided by: [badge-matrix](https://github.com/exogen/badge-matrix)

| OS      | 主分支 [master](https://github.com/lhy0403/Qv2ray/tree/master) | 开发分支 [dev](https://github.com/lhy0403/Qv2ray/tree/dev)   | 版本分支 [v1](https://github.com/lhy0403/Qv2ray/tree/version-v1) |
| ------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| [Linux](https://travis-ci.com/lhy0403/Qv2ray)   | ![Build Status Linux in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-master&branch=master) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-dev&branch=dev) | ![Build Status Linux in v1.0.0](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=linux&label=Linux-v1&branch=version-v1) |
| [MacOS](https://travis-ci.com/lhy0403/Qv2ray)   | ![Build Status MacOS in master](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-master&branch=master) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-dev&branch=dev) | ![Build Status Linux in dev](http://badges.herokuapp.com/travis.com/lhy0403/Qv2ray?style=flat-square&env=BADGE=osx&label=MacOS-v1&branch=version-v1) |
| [Windows](https://ci.appveyor.com/project/lhy0403/qv2ray) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/master?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/dev?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/dev) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/version-v1?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/version-v1) |

----------------

## 编译

  - Qt >= 5.12
  - gcc >=8 (Linux 需要 c++11 支持) 或 MinGW (Windows) 或 clang (MacOS)
  - QtCreator (推荐)


- 如果想测试当前的开发分支，请使用 `git checkout dev` 切换到开发分支
  - <span style="color: red">!! 该分支可能不稳定 或 甚至无法正常工作 !!</span>

### Linux & MacOS

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

[Jenkins](https://jenkins.himeki.net/job/Qv2ray/)编译服务器提供 `dev` 分支的二进制可执行程序下载，提供者 [@aliyuchang33](https://github.com/aliyuchang33)

下载后请添加可执行权限

```
chmod +x ./Qv2ray
./Qv2ray
```

### Windows

- 建议使用 Qt Creator

```batch
REM 首先把 %QTROOT%/bin 和 你的 MinGW/bin 文件夹添加到 PATH 里面
REM 示例配置：
REM SET PATH=%PATH%;D:\Qt\5.12.3\mingw73_32\bin\;D:\Qt\Tools\mingw730_32\bin\

SET PATH=%PATH%;Qt安装目录\Qt版本号\编译器类型\bin;Qt安装目录\Tools\编译器类型\bin

git clone --recursive https://github.com/lhy0403/Qv2ray && cd Qv2ray
REM 按需要签出开发分支
REM git checkout dev

lrelease ./Qv2ray.pro
mkdir build && cd build
qmake ../
mingw32-make.exe
```


# License

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
