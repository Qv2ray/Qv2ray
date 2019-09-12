# Qv2ray 安装方法 - 任何系统

*此方法为通用操作方法，任何系统都可以使用这种方法安装并使用 Qv2ray*

1. 你需要从官方 [releases](https://github.com/v2ray/v2ray-core/releases/latest) 下载对应系统版本的核心文件，并解压缩
2. 根据系统类别下载 Qv2ray 发布版，解压缩或双击打开 AppImage
3. 打开首选项，设置 vcore 路径和 assets 路径到解压后的文件夹
   - 此种情况下，首选项中的 core 路径和 assets 路径（通常）指向同一文件夹（Qv2ray 包含自动补全功能）
4. 点击OK保存配置，并开始添加连接，你可以选择导入现有链接或者手动输入链接设置
5. 使用托盘图标中键，窗口按钮或右键菜单均可启动当前连接

# *附录*

## *1. Qv2ray 在不同操作系统中生成的配置文件夹位置*

- *Windows: `C:\Users\用户名\.qv2ray`*
- *MacOS: `/Users/用户名/.qv2ray`*
- *Linux: `/home/用户名/.qv2ray`*