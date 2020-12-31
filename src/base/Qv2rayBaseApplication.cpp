#include "Qv2rayBaseApplication.hpp"

#include "components/translations/QvTranslator.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "BaseApplication"
inline QString makeAbs(const QString &p)
{
    return QDir(p).absolutePath();
}

Qv2rayApplicationInterface::Qv2rayApplicationInterface()
{
    ConfigObject = new Qv2rayConfigObject;
    QvCoreApplication = this;
    LOG("Qv2ray", QV2RAY_VERSION_STRING, "on", QSysInfo::prettyProductName(), QSysInfo::currentCpuArchitecture());
    DEBUG("Qv2ray Start Time: ", QTime::currentTime().msecsSinceStartOfDay());
    DEBUG("QV2RAY_BUILD_INFO", QV2RAY_BUILD_INFO);
    DEBUG("QV2RAY_BUILD_EXTRA_INFO", QV2RAY_BUILD_EXTRA_INFO);
    DEBUG("QV2RAY_BUILD_NUMBER", QSTRN(QV2RAY_VERSION_BUILD));
    QStringList licenseList;
    licenseList << "This program comes with ABSOLUTELY NO WARRANTY.";
    licenseList << "This is free software, and you are welcome to redistribute it";
    licenseList << "under certain conditions.";
    licenseList << "Copyright (c) 2019-2021 Qv2ray Development Group.";
    licenseList << "Third-party libraries that have been used in this program can be found in the About page.";
    LOG(licenseList.join(NEWLINE));
}

Qv2rayApplicationInterface::~Qv2rayApplicationInterface()
{
    delete ConfigObject;
    QvCoreApplication = nullptr;
}

QStringList Qv2rayApplicationInterface::GetAssetsPaths(const QString &dirName) const
{
    // Configuration Path
    QStringList list;

    if (qEnvironmentVariableIsSet("QV2RAY_RESOURCES_PATH"))
        list << makeAbs(qEnvironmentVariable("QV2RAY_RESOURCES_PATH") + "/" + dirName);

    // Default behavior on Windows
    list << makeAbs(QCoreApplication::applicationDirPath() + "/" + dirName);
    list << makeAbs(QV2RAY_CONFIG_DIR + dirName);
    list << ":/" + dirName;

    list << QStandardPaths::locateAll(QStandardPaths::AppDataLocation, dirName, QStandardPaths::LocateDirectory);
    list << QStandardPaths::locateAll(QStandardPaths::AppConfigLocation, dirName, QStandardPaths::LocateDirectory);

#ifdef Q_OS_UNIX
    if (qEnvironmentVariableIsSet("APPIMAGE"))
        list << makeAbs(QCoreApplication::applicationDirPath() + "/../share/qv2ray/" + dirName);

    if (qEnvironmentVariableIsSet("SNAP"))
        list << makeAbs(qEnvironmentVariable("SNAP") + "/usr/share/qv2ray/" + dirName);

    if (qEnvironmentVariableIsSet("XDG_DATA_DIRS"))
        list << makeAbs(qEnvironmentVariable("XDG_DATA_DIRS") + "/" + dirName);

    list << makeAbs("/usr/local/share/qv2ray/" + dirName);
    list << makeAbs("/usr/local/lib/qv2ray/" + dirName);
    list << makeAbs("/usr/share/qv2ray/" + dirName);
    list << makeAbs("/usr/lib/qv2ray/" + dirName);
    list << makeAbs("/lib/qv2ray/" + dirName);
#endif

#ifdef Q_OS_MAC
    // macOS platform directories.
    list << QDir(QCoreApplication::applicationDirPath() + "/../Resources/" + dirName).absolutePath();
#endif

    list.removeDuplicates();
    return list;
}
