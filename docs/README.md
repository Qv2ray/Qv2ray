# Qv2ray

使用 Qt 的跨平台 v2ray 图形客户端 - 用户手册



## 操作之前

- 请查看最新的 ReleaseNote （当前版本为 [v1.2.0](./ReleaseNote/ReleaseNote-v1.2.md)）



## 运行依赖

- ArchLinux

  - 在 `v2ray` 包的基础上 额外需要 `v2ray-domain-list-community` 和 `v2ray-geoip` 进行分流代理

    Issue: https://github.com/lhy0403/Qv2ray/issues/23#issuecomment-511384858



## 下载和配置

1. 根据对应系统下载程序包
2. 双击打开 Qv2ray 程序
3. 程序运行后，根据操作系统不同，在以下目录中分别生成程序配置文件 `Qv2ray.conf` 和 `generated` 文件夹

| OS       | Windows                 | Linux                      | MacOS                       |
| -------- | ----------------------- | -------------------------- | --------------------------- |
| 文件路径 | `C:\你的用户名\.qv2ray` | `/home/你的用户名/.qv2ray` | `/Users/你的用户名/.qv2ray` |

4. 点击首选项 或 Preference，设置 v2ray core 路径 和 v2ray assets 文件夹路径 
   - 部分 Linux 发行版分别使用 `/bin/v2ray` 和 `/etc/v2ray` 作为 core 和 assets 路径
   - Windows 和 MacOS 需要从官方 [releases](https://github.com/v2ray/v2ray-core/releases/latest) 下载并解压缩
     - 此种情况 core 路径和 assets 路径（通常）指向同一文件夹 

5. 点击OK保存配置，并开始添加连接

6. 使用托盘图标中键，窗口按钮或右键菜单均可启动 v2ray



## 更多设置详解

### 自动连接

- 该功能自 [v1.2.0](./ReleaseNote/ReleaseNote-v1.2.md) 版本添加

如要将某个连接设为自动连接，请打开首选项

1. 从 "自动连接到" 下拉菜单中选择需要自动建立连接的配置

2. 点击 OK 并重启 Qv2ray 查看效果

3. 如在启动时自动连接了某个配置文件时，主窗口将不会显示，请从托盘图标处访问 Qv2ray

