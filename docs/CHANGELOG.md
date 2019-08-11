# Qv2ray 开发版更新日志

**当前最新版本: ** [v1.2.0](https://github.com/lhy0403/Qv2ray/releases/tag/v1.2.0)

-------

## 开发分支

**2019-08-05**: 正在添加 VMess 订阅支持，未完成，计划在 v2.0 实现

**2019-07-11**: 准备 dev 分支开始 v2.0 版本

-------

## Version [v1.3.0](https://github.com/lhy0403/Qv2ray/releases/tag/v1.3.0)

**2019-08-11**: 发布功能更新 [v1.3.0](https://github.com/lhy0403/Qv2ray/releases/tag/v1.3.0)

**2019-08-11**: 添加对于 ShadowSocks 的支持

**2019-08-10**: 正在添加 ShadowSocks 配置 UI，未完成 (See [531c26](https://github.com/lhy0403/Qv2ray/commit/531c2625f021a097fd53aba8e5fe0fd0b84ad836) and [#19](https://github.com/lhy0403/Qv2ray/issues/19))

---------------

## Version [v1.2.0](https://github.com/lhy0403/Qv2ray/releases/tag/v1.2.0)

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

---------------

## Version [v1.1.0](https://github.com/lhy0403/Qv2ray/releases/tag/v1.1.0a)  （未发布）

**2019-07-23**: v1.1.0 版本添加了自动启动功能

---------------

## Version [v1.0.1](https://github.com/lhy0403/Qv2ray/releases/tag/v1.0.1)

**2019-07-14**: *v1.0.1* 修复一个拼写错误，该错误源自 [`0e9b90f`](https://github.com/lhy0403/Qv2ray/commit/0e9b90fb116b790156314a21a6ef1abc8d60fa63#diff-c3f4a6d32c4ab34067ba5fa647341c6aR12) 提交

---------------

## Version [v1.0.0](https://github.com/lhy0403/Qv2ray/releases/tag/v1.0.0)

**2019-07-11**: *v1.0.0* 发布第一个 RC 修改 MacOS 默认文件位置

---------------

## Version [v0.9.9b](https://github.com/lhy0403/Qv2ray/releases/tag/v0.9.9b)

**2019-07-09**: *v0.9.9b* 发布第一个公开测试版本 [v0.9.9b](https://github.com/lhy0403/Qv2ray/releases/tag/v0.9.9b)

**2019-07-08**: dev 分支的 [`v0.9.2a`](https://github.com/lhy0403/Qv2ray/releases/tag/v0.9.2a) 版本现在可以使用 GUI 修改配置，并做到动态重载配置（包括入站设置，日志，Mux选项）此版本完成了所有翻译工作，添加了双击配置列表即可启动对应配置的功能

**2019-07-07**: [Commit: [9b02ff](https://github.com/lhy0403/Qv2ray/commit/9b02ff9da8f96325bafa08958ba12c0dff66e715) ] 现在可以启动导入的配置文件 (包括导入现有文件和 `vmess://` 协议)，手动添加配置尚未实现，入站设置现在只能通过编辑配置文件完成 (Linux: `~/.qv2ray/Qv2ray.conf`, MacOS & Windows: 程序当前文件夹)，此版本部分翻译不完整

---------------

## 早期版本

**2019-07-04**: 我们终于摆脱了对于 Python 的依赖，现在 Qv2ray 可以自行解析 `vmess://` 协议 [WIP]

**2019-07-03**: 主配置文件序列化/反序列化工作完成，并添加更多协议配置

**2019-07-02**: 等待上游依赖完成更新 [JSON 序列化 std::list](https://github.com/xyz347/x2struct/issues/11#issuecomment-507671091)

**2019-07-01**: 休息了几天，主要是去关注别的项目了。现在开始重构 v2ray 交互部分。

**2019-06-24**: Mac OS 测试构建完成，合并到开发分支 [dev](https://github.com/lhy0403/Qv2ray/tree/dev)

**2019-06-24**: 新建分支 MacOS-Build 开始测试 MacOS 构建，当前状态：![Build Status](https://travis-ci.com/lhy0403/Qv2ray.svg?branch=MacOS-Build)

**2019-06-23**: UI 结构已经固定，新建分支 translations 进行翻译 UI

**2019-06-23**: 基本 UI 完成，切换到 [dev](https://github.com/lhy0403/Qv2ray/tree/dev) 分支进行代码实现

**2019-06-22**: 当前开发分支[ui-implementation](https://github.com/lhy0403/Qv2ray/tree/ui-implementation) - 用于实现基本 UI
