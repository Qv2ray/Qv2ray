# Qv2ray Base Library (libQv2rayBase)

[![Build for Android](https://github.com/moodyhunter/Qv2rayBase/actions/workflows/build-android.yml/badge.svg)](https://github.com/moodyhunter/Qv2rayBase/actions/workflows/build-android.yml) [![Build for Desktop](https://github.com/moodyhunter/Qv2rayBase/actions/workflows/build-desktop.yml/badge.svg)](https://github.com/moodyhunter/Qv2rayBase/actions/workflows/build-desktop.yml) [![Build for WASM](https://github.com/moodyhunter/Qv2rayBase/actions/workflows/build-wasm.yml/badge.svg)](https://github.com/moodyhunter/Qv2rayBase/actions/workflows/build-wasm.yml)

## Description

Qv2rayBase is a C++/Qt library, extracted from Qv2ray but is enhanced into a generic, cross-platform, feature-rich connection manager.

## Supported Platforms

- Desktop
  - Linux
  - Windows
  - macOS
- Browsers
  - Emscripten (WASM)
- Mobile
  - Android

## Features

### Profile Management
- Connection Management
    - Create, Remove, Update, Get
    - Copy, Move, Link to Group (using a refcount mechanism)
- Group Management
    - Create, Remove, Update, Get
    - Custom subscription provider
    - Subscription filtration
- Routing Management
    - Per-connection, per-group and global-scoped routing settings
- Latency Test Engines
    - With async (provided by `libuv` and `uvw`) and sync calls.
- Connection Statistics
    - Uplink & Downlink Statistics for proxied connections
    - Uplink & Downlink Statistics for direct connections

### Plugin Management

#### Supported Plugin Categories

- Profile Proprocessor Plugin
    - Preprocess a profile before sending it to the kernel.
- Event Handler Plugin
    - Pre-connection
    - Post-connection
    - Pre-disconnection
    - Post-disconnection
    - Connection-Created
    - Connection-Edited
    - Connection-Renamed
    - Connection-LinkedWithGroup
    - Connection-RemovedFromGroup
    - Connection-FullyRemoved (refcount drops to 0)
    - Group-Created
    - Group-Edited
    - Group-Renamed
    - Statistics Event

- Outbound Processor Plugin
- Subscription Decoder Plugin
- Kernel Plugin
- Latency Test Engine Plugin
- GUI Plugin
    - MainWindow Entry Button
    - Tray Icon Menus
    - Inbound / Outbound Editors
    - Settings Widget for the Plugin Itself.

#### All Plugins have complete access to the `ProfileManager`

- That is, all connections, groups, routings-related functionalities that can be achiveved by using a GUI can be done inside by plugins programmatically.
- This enables many extra possibilities, e.g.
    1. A plugin with a `MainWindow Entry Button` can open a 3rd-party editor, to edit a profile in low-level.
    2. A plugin with `Profile Preprocessor` can process a profile with some extra data from somewhere else:
       Consider a plugin with `Outbound Processor` that registered a protocol called `external` which actually appeared to be a symbolic link of an outside connection, referenced using connectionId, in this case, the preprocessor can go and get the information of that referenced connection.
    3. A plugin can now add a profile without suggesting user to use the `Import Window`

#### Static Plugins v.s. Shared Library Plugins

- Shared plugins are dll/so/dylibs, which are needed to be placed into some specific directories, those searched directories are determined by `IStorageProvider`
- Static plugins are built into the executable, they are bundled into the executable and can be automatically found by the plugin loader (`PluginManagerCore`), no extra deployment is required.

### `IStorageProvider`, the abstracted storage backend

- The extensible storage backend for Qv2rayBase, allowing developers use a specific configuration backend rather than the old file-based storage.
- A built-in provider is used when `nullptr` is provided when constructing `Qv2rayBaseLibrary`

### `IUserInteractionInterface`, the abstracted user interaction interface

- Giving custom user interaction options, this includes:
    - Warning MessageBoxes
    - Infomative MessageBoxes
    - Questioning MessageBoxes
    - Opening External URLs
- Since Qv2rayBase is a base library, it does not provide a builtin implementation, different frontends (CLI, QWidget, QML) may need to provide their own implementation.

## CMake Usage
- Qv2rayBase is built by CMake, and can be used by any CMake-based projects:
    - When using as a package:
        Use `find_package(Qv2rayBase REQUIRED)`
    - When using as a submodule:
        Use `add_subdirectory(path/to/Qv2rayBase)`

    ... Then link your target with `Qv2ray::Qv2rayBase`

## License

GPLv3
