# `install-qt-action`

Installing Qt on Github Actions workflows manually is the worst.

You know what's easier than dealing with that? Just using this:
```yml
    - name: Install Qt
      uses: jurplel/install-qt-action@v1
```

All done.

## Options

### `version`
The desired version of Qt to install.

Default: `5.12.5` (Latest LTS at the time of writing)

### `host`
This is the host platform of the Qt version you will be installing. It's unlikely that you will need to set this manually if you are just building.

For example, if you are building on Linux and targeting desktop, you would set host to `linux`. If you are building on Linux and targeting android, you would set host to `linux` also. The host platform is the platform that your application will build on, not its target platform.

Possible values: `windows`, `mac`, or `linux`

Defaults to the current platform it is being run on.


### `target`
This is the target platform that you will be building for. You will want to set this if you are building for iOS or Android. 

**Please note that iOS builds are supported only on macOS hosts**

Possible values: `desktop`, `android`, or `ios`

Default: `desktop`

### `arch`
This is the target architecture that your program will be built for. This is only used for Windows and Android.

**Linux x86 packages are not supported by this action.** Qt does not offer pre-built Linux x86 packages. Please consider using your distro's repository or building it manually.

**Possible values:**

Windows: `win64_msvc2017_64`, `win64_msvc2015_64`, `win32_msvc2015`, `win32_mingw53`, or `win64_mingw73`

Android: `android_x86`, `android_armv7`

**Default values:**

Windows: `win64_msvc2017_64`

Android: `android_armv7`

### `dir`
This is the directory prefix that Qt will be installed to.

For example, if you set dir to `/example/`, your bin folder will be located at `/example/Qt5.12.5/5.12.5/(your_arch)/bin`. When possible, access your Qt directory through the `Qt5_Dir` environment variable.

Default: `${RUNNER_WORKSPACE}` (this is one above the starting directory)

## More info

The Qt bin directory is added to your `path` environment variable. `Qt5_Dir` is also set appropriately for cmake. 

Big thanks to the [aqtinstall](https://github.com/miurahr/aqtinstall/) developers for making this easy. Please go support them, they did all of the hard work here.

This action is distributed under the [MIT license](LICENSE).

By using this action, you agree to the terms of Qt's licensing. See [Qt licensing](https://www.qt.io/licensing/) and [Licenses used by Qt](https://doc.qt.io/qt-5/licenses-used-in-qt.html). 
