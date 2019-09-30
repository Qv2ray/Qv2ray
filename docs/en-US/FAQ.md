# Frequently Asked Question

## v2ray Failed to start after enabling tProxy

Detail:  `Segmentation Fault` occurd after enabling tProxy

It's caused by a limit in `SUID` feature on some Linux OSes. Detailed error analysis please see: [#59](https://github.com/lhy0403/Qv2ray/issues/59)

 - Solution:`sudo sysctl fs.suid_dumpable=1`
 - The solution will lost on reboot, please refer to [this blog](http://ssdxiao.github.io/linux/2017/03/20/Sysctl-not-applay-on-boot.html) if you want to keep it.

## Ubuntu gives an super ugly UI

- Solution: Append `--style fusion` to the command line arg solves this problem. 