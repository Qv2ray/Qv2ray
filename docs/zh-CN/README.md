# Qv2ray 用户手册

- 基于 Qt 的跨平台 v2ray 图形客户端

- #### 当前最新版本： [v1.3.8.0](./ReleaseNotes/1.3/v1.3.8.0.md)

--------

## 1. 功能简介

见 [简单的功能介绍](./Features.md)

## 2. 首次使用，安装与配置

使用 Qv2ray 需要以下几个步骤：

1. 在 [Release](https://github.com/lhy0403/Qv2ray/releases/latest) 下载 Qv2ray (ArchLinux 用户可以使用 `qv2ray` 的 [AUR](https://aur.archlinux.org/packages/qv2ray/))
2. 在 `v2ray` 官方下载 [v2ray 内核](https://github.com/v2ray/v2ray-core/releases/latest) 
3. 打开 Qv2ray, 首次运行会提示无法找到 v2ray 内核
   - 点击确定，将刚刚下载的 v2ray内核 解压到自动打开的文件夹内
   - 该文件夹包含一个 `Put your v2ray.exe here.txt` 以供识别
4. 解压完成后，关闭弹出的文件夹，即可开始导入或新建连接配置

```
注： Qv2ray 在不同系统中查找 v2ray 内核的文件路径 （即找不到内核时自动弹出的文件夹路径）
- Windows: C:\Users\用户名\.qv2ray\vcore
- macOS: /Users/用户名/.qv2ray/vcore
- Linux: ~/.qv2ray/vcore
```


## 3. 更新历史

见 [更新历史记录](./History.md)

## 4. 常见问题 FAQ

见 [FAQ.md](./FAQ.md)

## 5. 作出贡献

1. 帮助翻译 Qv2ray 到多个语言
   - 翻译平台使用 Crowdin [https://crowdin.com/project/qv2ray](https://crowdin.com/project/qv2ray)
2. 提出新功能建议
   - 使用 Github Issues
