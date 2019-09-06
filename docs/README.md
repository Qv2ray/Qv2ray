# Qv2ray 用户手册

欢迎使用基于 Qt 的跨平台 v2ray 图形客户端

- #### 当前最新版本： [v1.3.5.2](./ReleaseNote/ReleaseNote-v1.3.5.2.md)

- #### 历史版本：v1.3.5(由于bug停止下载) & [v1.3.2](./ReleaseNote/ReleaseNote-v1.3.2.md) & [v1.3.0](./ReleaseNote/ReleaseNote-v1.3.md)
  

--------

## 1. 首次使用

1. 根据对应系统下载程序包，见最新 [release](https://github.com/lhy0403/Qv2ray/releases/latest)

2. 打开 Qv2ray 程序

3. 下面步骤会根据系统与发行版不同，分为以下几种情况:

----------------------------------------------
### ArchLinux 及 Arch 衍生版

- 从 AUR 安装 `qv2ray` 包
  - *注意，ArchLinux 用户需要在 `v2ray` 包的基础上 额外需要 `v2ray-domain-list-community` 和 `v2ray-geoip` 进行分流代理*
  - *详见（Issue: [#23-511384858](https://github.com/lhy0403/Qv2ray/issues/23#issuecomment-511384858)）*

### 其他Linux，且发行版包含 `v2ray` 包：

1.  请安装 `v2ray` 程序包
2.  Linux 版 Qv2ray 默认使用 `/bin/v2ray` 和 `/etc/v2ray` 作为内核与 assets 路径，如果与你的位置不一样，请到首选项中修改
3.  安装完成后重新启动 Qv2ray

### Windows & MacOS & 不包含 v2ray 程序包的 Linux 发行版

1. 你需要从官方 [releases](https://github.com/v2ray/v2ray-core/releases/latest) 下载对应系统版本的核心文件，并解压缩
    
2. 打开首选项，设置 vcore 路径和 assets 路径到解压后的文件夹
    
    - *推荐使用**配置文件夹**中的 `vcore` 子文件夹作为解压缩目录*
    
    - *此种情况下，首选项中的 core 路径和 assets 路径（通常）指向同一文件夹*

----------------------------------------------
4. 点击OK保存配置，并开始添加连接，你可以选择导入现有链接或者手动输入链接设置

5. 使用托盘图标中键，窗口按钮或右键菜单均可启动当前连接

## 2. 功能简介

见 [Features](./Features.md)



---------------------------------



# *附录*

## *1. Qv2ray 在不同操作系统中生成的配置文件夹位置*

- *Windows: `C:\Users\用户名\.qv2ray`*
- *MacOS: `/Users/用户名/.qv2ray`*
- *Linux: `/home/用户名/.qv2ray*`