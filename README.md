# Qv2ray 

TODO: 使用 Qt 的跨平台 v2ray 客户端，目前仍处于早期开发，多数功能尚未齐全。

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/a034dd186c36408c92ffb04449fb6996)](https://app.codacy.com/app/lhy0403/Qv2ray?utm_source=github.com&utm_medium=referral&utm_content=lhy0403/Qv2ray&utm_campaign=Badge_Grade_Dashboard)[![HitCount](http://hits.dwyl.io/lhy0403/Qv2ray.svg)](http://hits.dwyl.io/lhy0403/Qv2ray)[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/lhy0403/Qv2ray.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/lhy0403/Qv2ray/context:cpp) [![Total alerts](https://img.shields.io/lgtm/alerts/g/lhy0403/Qv2ray.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/lhy0403/Qv2ray/alerts/)

| OS            | 主分支 [master](https://github.com/lhy0403/Qv2ray/tree/master) | 开发分支 [dev](https://github.com/lhy0403/Qv2ray/tree/dev)   |
| ------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Linux & MacOS | [![Build Status](https://travis-ci.com/lhy0403/Qv2ray.svg?branch=master)](https://travis-ci.com/lhy0403/Qv2ray) |  [![Build Status](https://travis-ci.com/lhy0403/Qv2ray.svg?branch=dev)](https://travis-ci.com/lhy0403/Qv2ray)                                                            |
| Windows       | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/master?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/i1l524ws0hiitpm4/branch/dev?svg=true)](https://ci.appveyor.com/project/lhy0403/qv2ray/branch/dev) |

## 最近更新内容

- 详见 [ChangeLog.md](CHANGELOG.md)



## 鸣谢

[@aliyuchang33](https://github.com/aliyuchang33) - 项目原作者/发起人，感谢他使用 Qt 作为基础框架。 

 - 可惜原项目不更新了qwq



## 项目依赖
### Linux
- `Qt >= 5.12`
- `gcc >=8` (需要 c++11 支持)
### Windows
Qt Creator (Qt5)

### MacOS

- Qt >= 5.12

## 编译
 - 如果想测试当前的开发分支，请使用 `git checkout dev` 切换到开发分支
   - <span style="color: red">!! 该分支可能不稳定 或 甚至无法正常工作 !!</span>
### Linux
```bash
git clone https://github.com/lhy0403/Qv2ray && cd Qv2ray
lrelease ./Qv2ray.pro
mkdir build && cd build
qmake ../
make
```
### Windows
`目前没有稳定的构建方案，请参考 AppVeyor.yml 需要 python3.7 和 Qt Creator`

## License

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
