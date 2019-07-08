# 编译 Qv2ray

- 如果想测试当前的开发分支，请使用 `git checkout dev` 切换到开发分支
  - <span style="color: red">!! 该分支可能不稳定 或 甚至无法正常工作 !!</span>

### Linux

```bash
git clone https://github.com/lhy0403/Qv2ray && cd Qv2ray
lrelease ./Qv2ray.pro
mkdir build && cd build
qmake ../
make
```

### Windows & MacOS

- 建议使用 Qt Creator
- TODO