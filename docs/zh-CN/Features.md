# Qv2ray 功能简介


## 1. Linux 友好

Qv2ray 支持 Linux 桌面环境

## 2. 连接导入

Qv2ray 支持以下位置导入配置文件

- 文件导入
- VMess 连接字符串
- 手动添加

## 3. 连接编辑

- 大概会成为全平台最强大的 v2ray 配置文件编辑器了吧
- 路由编辑功能正在开发中 见 [dev 分支](https://github.com/lhy0403/Qv2ray/tree/dev) 或 [开发版本 AUR](https://aur.archlinux.org/packages/qv2ray-dev-git)
- 截至到 v1.3.5.3 已经支持以下出站协议的编辑
  - VMess
  - ShadowSocks
  - Socks

## 4. 自动连接

- 在启动时不显示主窗口，直接连接到设置的服务器
- 该功能自 [v1.2.0](./ReleaseNotes/1.2/v1.2.0.0.md) 版本添加

### 如要将某个连接设为自动连接，请打开首选项

1. 从 "自动连接到" 下拉菜单中选择需要自动建立连接的配置
2. 点击确定，保存设置即可

## 5. 自动更新

- 使用 Github Release 的自动更新机制，保证软件版本为最新，减少 Bug 数量，及时尝试新功能

## 6. 多语言支持

- 截止到 v1.3.7.1，Qv2ray 现已支持一下两种语言

  - 英文
  - 中文
- 俄罗斯语 （感谢 @thebadgateway）
  
  （还希望大家共同翻译啊 https://crowdin.com/project/qv2ray）

## 7. 实时显示 v2ray 的输出

- 查看 bug 更方便

## 8. 自动 tProxy 支持 （Linux）

- 一键对 v2ray 主程序启用透明代理功能 (`setcap`)

