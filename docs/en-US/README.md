# Qv2ray User Manual

- A cross platform v2ray GUI written in QT

- #### Current Latest Release Version: [v1.3.8](./ReleaseNotes/1.3/v1.3.8.0.md)

--------

## 1. Features

Please see [Brief Feature Introduction](./Features.md)

## 2. Initial Setup and Usage

There are several steps:

1. Download Qv2ray form [Release](https://github.com/lhy0403/Qv2ray/releases/latest) ([AUR](https://aur.archlinux.org/packages/qv2ray/) is available for Arch Linux Users)
2. Download [v2ray kernel](https://github.com/v2ray/v2ray-core/releases/latest) from `v2ray-core` repo.
3. Open Qv2ray and a message box will tell you `Failed to find v2ray kernal`
   - Click OK, and extract the kernel to the folder pop up just now.
   - The folder contains a file named: `Put your v2ray.exe here.txt` for reference.
4. Close the folder after extracting and you may start importing configs.

```
Notes: Directories that Qv2ray will search for v2ray-kernel
- Windows: C:\Users\USERNAME\.qv2ray\vcore
- macOS: /Users/USERNAME/.qv2ray/vcore
- Linux: ~/.qv2ray/vcore
```


## 3. Update History

See [Update History](./History.md)

## 4. FAQ

Please refer to: [FAQ.md](./FAQ.md)

## 5. Contribute

1. Help translating Qv2ray to more languages. 
   - Crowdin [https://crowdin.com/project/qv2ray](https://crowdin.com/project/qv2ray)
2. New feature request.
   - You can use Github Issues
