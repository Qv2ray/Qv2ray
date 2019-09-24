# 经常出现的问题

## 在启用了 tPtoxy 支持后 v2ray 启动失败
详细内容：启动 v2ray 时出现 `Segmentation Fault`
此问题是部分 linux 的 suid 功能受限引起的，具体错误分析可以参考 [#59](https://github.com/lhy0403/Qv2ray/issues/59)
 - 解决方案：`sudo sysctl fs.suid_dumpable=1`
 - 此操作不会保存 sysctl 设置，重启之后会失效。如果需要持久性修改此参数，请参考 [这篇blog](http://ssdxiao.github.io/linux/2017/03/20/Sysctl-not-applay-on-boot.html)

## 在 Ubuntu 系统下 UI 变得超级丑

- 解决方案：运行时添加 `--style fusion` 即可解决问题