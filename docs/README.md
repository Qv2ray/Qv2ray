# Qv2ray 用户手册

欢迎使用基于 Qt 的跨平台 v2ray 图形客户端

## 操作之前

请查看最新版本的发行说明：

- 当前最新版本为 [v1.3.2](./ReleaseNote/ReleaseNote-v1.3.2.md)
- 历史版本：
  -  [v1.3.0](./ReleaseNote/ReleaseNote-v1.3.md)
  -  [v1.2.0](./ReleaseNote/ReleaseNote-v1.2.md)

--------

## 首次使用

1. 根据对应系统下载程序包

2. 双击打开 Qv2ray 程序

   *程序运行后，会在用户目录中分别生成程序配置文件（见附录1）*

4. 第一次运行，Qv2ray 将会提示找不到核心文件，在这时：
   
   - 如果你使用的是 Linux 系统，并且你的发行版包含 `v2ray` 包：
     - 请安装 `v2ray` 程序包，**注意，`ArchLinux` 用户需要在 `v2ray` 包的基础上 额外需要 `v2ray-domain-list-community` 和 `v2ray-geoip` 进行分流代理** 详见（Issue: [#23-511384858](https://github.com/lhy0403/Qv2ray/issues/23#issuecomment-511384858)）
     - Linux 版 Qv2ray 默认使用 `/bin/v2ray` 和 `/etc/v2ray` 作为 core 和 assets 路径，如果与你的位置不一样，请到首选项中修改
     - 安装完成后重新启动 Qv2ray
   
   - 如果你的发行版**不**包含 v2ray 程序包，或者你是用的是 **Windows** 或 **MacOS**：
     - 你需要从官方 [releases](https://github.com/v2ray/v2ray-core/releases/latest) 下载核心文件，并解压缩到一个单独的文件夹 
       - **推荐使用配置文件夹中的 `vcore` 子文件夹**
     - 打开首选项，设置 vcore 路径和 assets 路径到解压后的文件夹
       - **此种情况下，首选项中的 core 路径和 assets 路径（通常）指向同一文件夹**
   
5. 点击OK保存配置，并开始添加连接，你可以选择导入现有链接或者手动输入链接设置

6. 使用托盘图标中键，窗口按钮或右键菜单均可启动当前连接

## 自动连接

- 该功能自 [v1.2.0](./ReleaseNote/ReleaseNote-v1.2.md) 版本添加

如要将某个连接设为自动连接，请打开首选项

1. 从 "自动连接到" 下拉菜单中选择需要自动建立连接的配置

2. 点击 OK 并重启 Qv2ray 查看效果

3. 如在启动时自动连接了某个配置文件时，主窗口将不会显示，请从托盘图标处访问 Qv2ray



# 附录

## 1. Qv2ray 在不同操作系统中生成的配置文件夹位置

- Windows: `C:\Users\用户名\.qv2ray`
- MacOS: `/Users/用户名/.qv2ray`
- Linux: `/home/用户名/.qv2ray`