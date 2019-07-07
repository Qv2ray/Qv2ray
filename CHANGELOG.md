# News
## 最新

**2019-07-07**: [Commit: [9b02ff](https://github.com/lhy0403/Qv2ray/commit/9b02ff9da8f96325bafa08958ba12c0dff66e715) ] 现在可以启动导入的配置文件 (包括导入现有文件和 `vmess://` 协议)，手动添加配置尚未实现，入站设置现在只能通过编辑配置文件完成 (Linux: `~/.qv2ray/Qv2ray.conf`, MacOS & Windows: 程序当前文件夹)，此版本部分翻译不完整

## 历史

**2019-07-04**: 我们终于摆脱了对于 Python 的依赖，现在 Qv2ray 可以自行解析 `vmess://` 协议 [WIP]

**2019-07-03**: 主配置文件序列化/反序列化工作完成，并添加更多协议配置

**2019-07-02**: 等待上游依赖完成更新 [JSON 序列化 std::list](https://github.com/xyz347/x2struct/issues/11#issuecomment-507671091)

**2019-07-01**: 休息了几天，主要是去关注别的项目了。现在开始重构 v2ray 交互部分。

**2019-06-24**: Mac OS 测试构建完成，合并到开发分支 [dev](https://github.com/lhy0403/Qv2ray/tree/dev)

**2019-06-24**: 新建分支 MacOS-Build 开始测试 MacOS 构建，当前状态：![Build Status](https://travis-ci.com/lhy0403/Qv2ray.svg?branch=MacOS-Build)

**2019-06-23**: UI 结构已经固定，新建分支 translations 进行翻译 UI

**2019-06-23**: 基本 UI 完成，切换到 [dev](https://github.com/lhy0403/Qv2ray/tree/dev) 分支进行代码实现

**2019-06-22**: 当前开发分支[ui-implementation](https://github.com/lhy0403/Qv2ray/tree/ui-implementation) - 用于实现基本 UI

