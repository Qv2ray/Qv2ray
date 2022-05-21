#include "w_V2RayKernelSettings.hpp"

#include "V2RayCorePluginTemplate.hpp"
#include "common/CommonHelpers.hpp"

#include <QFileDialog>
#include <QProcessEnvironment>
#include <QStandardPaths>

using namespace V2RayPluginNamespace;

V2RayKernelSettings::V2RayKernelSettings(QWidget *parent) : Qv2rayPlugin::Gui::PluginSettingsWidget(parent)
{
    setupUi(this);
    settingsObject.APIEnabled.ReadWriteBind(enableAPI, "checked", &QCheckBox::toggled);
    settingsObject.APIPort.ReadWriteBind(statsPortBox, "value", &QSpinBox::valueChanged);
    settingsObject.AssetsPath.ReadWriteBind(vCoreAssetsPathTxt, "text", &QLineEdit::textEdited);
    settingsObject.CorePath.ReadWriteBind(vCorePathTxt, "text", &QLineEdit::textEdited);
    settingsObject.LogLevel.ReadWriteBind(logLevelComboBox, "currentIndex", &QComboBox::currentIndexChanged);
    settingsObject.OutboundMark.ReadWriteBind(somarkSB, "value", &QSpinBox::valueChanged);
}

void V2RayKernelSettings::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void V2RayKernelSettings::Load()
{
    settingsObject.loadJson(settings);
}

void V2RayKernelSettings::Store()
{
    settings = settingsObject.toJson();
}

void V2RayKernelSettings::on_selectVCoreBtn_clicked()
{
    const auto core = QFileDialog::getOpenFileName(this, tr("Open V2Ray core file"), QDir::currentPath());
    if (!core.isEmpty())
        settingsObject.CorePath = core;
}

void V2RayKernelSettings::on_selectVAssetBtn_clicked()
{
    const auto dir = QFileDialog::getExistingDirectory(this, tr("Open V2Ray assets folder"), QDir::currentPath());
    if (!dir.isEmpty())
        settingsObject.AssetsPath = dir;
}

void V2RayKernelSettings::on_checkVCoreSettings_clicked()
{
#if V2RayCoreType == CORETYPE_V2Ray5 || V2RayCoreType == CORETYPE_V2RaySN
    // They have a newer format.
    const auto &[result, msg] = ValidateKernel(settingsObject.CorePath, settingsObject.AssetsPath, { u"version"_qs });
#else
    const auto &[result, msg] = ValidateKernel(settingsObject.CorePath, settingsObject.AssetsPath, { u"--version"_qs });
#endif

    if (!result)
    {
        V2RayCorePluginClass::ShowMessageBox(tr("V2Ray Core Settings"), *msg);
    }
    else
    {
        const auto content = tr("V2Ray path configuration check passed.") + //
                             u"\n\n"_qs +                                   //
                             tr("Kernel Output: ") +                        //
                             u"\n"_qs +                                     //
                             *msg;
        V2RayCorePluginClass::ShowMessageBox(tr("V2Ray Core Settings"), content);
    }
}

void V2RayKernelSettings::on_detectCoreBtn_clicked()
{
    QStringList searchPaths;

    // A cursed v2ray core searcher.
#ifdef Q_OS_WINDOWS
#define PATH_SPLITTER ';'
#else
#define PATH_SPLITTER ':'
#endif

    searchPaths << QProcessEnvironment::systemEnvironment().value(u"PATH"_qs).split(QChar::fromLatin1(PATH_SPLITTER));

    searchPaths << QDir::homePath();
    for (const auto &sp : {
             QStandardPaths::AppDataLocation,      //
             QStandardPaths::AppConfigLocation,    //
             QStandardPaths::AppLocalDataLocation, //
             QStandardPaths::ApplicationsLocation, //
             QStandardPaths::HomeLocation,         //
             QStandardPaths::DesktopLocation,      //
             QStandardPaths::DocumentsLocation,    //
             QStandardPaths::DownloadLocation,     //
         })
        for (const auto &dn : {
                 u"v2ray"_qs,            //
                 u"v2ray-core"_qs,       //
                 u"v2ray-windows-64"_qs, //
             })
            searchPaths << QStandardPaths::locateAll(sp, dn, QStandardPaths::LocateDirectory);

#ifdef Q_OS_WINDOWS
    // Scoop shim causes problems: https://github.com/lukesampson/scoop/issues/3294
    searchPaths.removeIf([](const QString &s) { return s.contains(u"shims"_qs); });
    searchPaths << QDir::homePath() + u"/scoop/apps/v2ray/current/"_qs;
    searchPaths << QDir::homePath() + u"/source/repos/v2ray-core/"_qs;
    searchPaths << QDir::homePath() + u"/source/repos/v2ray/"_qs;

    for (const auto &dl : QDir::drives())
        for (const auto &dn : {
                 u"v2ray"_qs,            //
                 u"v2ray-core"_qs,       //
                 u"v2ray-windows-64"_qs, //
             })
            searchPaths << dl.absolutePath() + dn;
#elif defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
    searchPaths << u"/bin"_qs;
    searchPaths << u"/usr/bin"_qs;
    searchPaths << u"/usr/local/bin"_qs;
    searchPaths << u"/usr/share/v2ray"_qs;
    searchPaths << u"/usr/local/share/v2ray"_qs;
    searchPaths << u"/usr/lib/v2ray"_qs;
    searchPaths << u"/usr/local/lib/v2ray"_qs;
    searchPaths << u"/opt/bin"_qs;
    searchPaths << u"/opt/v2ray"_qs;
    searchPaths << u"/usr/local/opt/bin"_qs;
    searchPaths << u"/usr/local/opt/v2ray"_qs;
#endif

    searchPaths << settingsObject.AssetsPath;
    searchPaths.removeDuplicates();

    QString corePath = settingsObject.CorePath;
    QString assetsPath = settingsObject.AssetsPath;

    bool assetsFound = false;
    bool coreFound = false;

    const auto result = QStandardPaths::findExecutable(u"v2ray"_qs, searchPaths);
    if (!result.isEmpty())
    {
        corePath = result;
        coreFound = true;
    }

    for (const auto &d : searchPaths)
    {
        const QDir assetsdir{ d };
        if (assetsdir.entryList().contains(u"geosite.dat"_qs) && assetsdir.entryList().contains(u"geoip.dat"_qs))
        {
            assetsFound = true;
            assetsPath = assetsdir.path();
            break;
        }
    }

    QStringList messages;
    messages << (coreFound ? u"Found v2ray core at: "_qs + corePath : u"Cannot find v2ray core."_qs);
    messages << (assetsFound ? u"Found v2ray assets at: "_qs + assetsPath : u"Cannot find v2ray assets."_qs);

    V2RayCorePluginClass::ShowMessageBox(u"V2Ray Core Detection"_qs, messages.join(QChar::fromLatin1('\n')));

    settingsObject.CorePath = corePath;
    settingsObject.AssetsPath = assetsPath;
}

void V2RayKernelSettings::on_resetVCoreBtn_clicked()
{
    settingsObject.CorePath = QString::fromUtf8(QV2RAY_DEFAULT_VCORE_PATH);
}

void V2RayKernelSettings::on_resetVAssetBtn_clicked()
{
    settingsObject.AssetsPath = QString::fromUtf8(QV2RAY_DEFAULT_VASSETS_PATH);
}
