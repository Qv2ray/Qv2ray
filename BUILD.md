# 编译 Qv2ray

- 如果想测试当前的开发分支，请使用 `git checkout dev` 切换到开发分支
  - <span style="color: red">!! 该分支可能不稳定 或 甚至无法正常工作 !!</span>

## Linux

```bash
git clone https://github.com/lhy0403/Qv2ray && cd Qv2ray

# 按需要签出开发分支
# git checkout dev

lrelease ./Qv2ray.pro
mkdir build && cd build
qmake ../
make
```

## Windows

- 建议使用 Qt Creator

```batch
REM 首先把 %QTROOT%/bin 和 你的 MinGW/bin 文件夹添加到 PATH 里面
REM 示例配置：
REM SET PATH=%PATH%;D:\Qt\5.12.3\mingw73_32\bin\;D:\Qt\Tools\mingw730_32\bin\

SET PATH=%PATH%;Qt安装目录\Qt版本号\编译器类型\bin;Qt安装目录\Tools\编译器类型\bin

git clone https://github.com/lhy0403/Qv2ray && cd Qv2ray
REM 按需要签出开发分支
REM git checkout dev

lrelease ./Qv2ray.pro
mkdir build && cd build
qmake ../
mingw32-make.exe
```

## MacOS
 - TODO 建议使用 Qt Creator

