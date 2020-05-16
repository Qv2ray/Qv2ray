#include "w_PreferencesWindow.hpp"

#include "common/HTTPRequestHelper.hpp"
#include "common/QvHelpers.hpp"
#include "common/QvTranslator.hpp"
#include "components/autolaunch/QvAutoLaunch.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/kernel/V2rayKernelInteractions.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "src/plugin-interface/QvPluginInterface.hpp"
#include "ui/styles/StyleManager.hpp"
#include "ui/widgets/RouteSettingsMatrix.hpp"

#include <QColorDialog>
#include <QCompleter>
#include <QDesktopServices>
#include <QFileDialog>
#include <QStyle>
#include <QStyleFactory>

using Qv2ray::common::validation::IsValidIPAddress;

#define LOADINGCHECK                                                                                                                            \
    if (!finishedLoading)                                                                                                                       \
        return;
#define NEEDRESTART                                                                                                                             \
    LOADINGCHECK                                                                                                                                \
    if (finishedLoading)                                                                                                                        \
        NeedRestart = true;

PreferencesWindow::PreferencesWindow(QWidget *parent) : QDialog(parent), CurrentConfig()
{
    setupUi(this);
    //
    // We currently don't support this feature.
    //    tProxyGroupBox->setVisible(false);
    tProxyCheckBox->setVisible(false);
    label_7->setVisible(false);
    //
    QvMessageBusConnect(PreferencesWindow);
    textBrowser->setHtml(StringFromFile(":/assets/credit.html"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // We add locales
    auto langs = Qv2rayTranslator->GetAvailableLanguages();
    if (!langs.empty())
    {
        languageComboBox->clear();
        languageComboBox->addItems(langs);
    }
    else
    {
        languageComboBox->setDisabled(true);
    }

    // Set auto start button state
    SetAutoStartButtonsState(GetLaunchAtLoginStatus());
    themeCombo->addItems(StyleManager->AllStyles());
    //
    qvVersion->setText(QV2RAY_VERSION_STRING);
    qvBuildInfo->setText(QV2RAY_BUILD_INFO);
    qvBuildExInfo->setText(QV2RAY_BUILD_EXTRA_INFO);
    qvBuildTime->setText(__DATE__ " " __TIME__);
    qvPluginInterfaceVersionLabel->setText(tr("Version: %1").arg(QSTRN(QV2RAY_PLUGIN_INTERFACE_VERSION)));
    //
    // Deep copy
    CurrentConfig = GlobalConfig;
    //
    themeCombo->setCurrentText(CurrentConfig.uiConfig.theme);
    darkThemeCB->setChecked(CurrentConfig.uiConfig.useDarkTheme);
    darkTrayCB->setChecked(CurrentConfig.uiConfig.useDarkTrayIcon);
#if (QV2RAY_USE_BUILTIN_DARKTHEME)
    // If we use built in theme, it should always be fusion.
    themeCombo->setEnabled(!CurrentConfig.uiConfig.useDarkTheme);
    darkThemeLabel->setText(tr("Use built-in darkmode Theme"));
#endif
    languageComboBox->setCurrentText(CurrentConfig.uiConfig.language);
    logLevelComboBox->setCurrentIndex(CurrentConfig.logLevel);
    tProxyCheckBox->setChecked(CurrentConfig.tProxySupport);
    //
    //
    listenIPTxt->setText(CurrentConfig.inboundConfig.listenip);
    //
    bool have_http = CurrentConfig.inboundConfig.useHTTP;
    httpGroupBox->setChecked(have_http);
    httpPortLE->setValue(CurrentConfig.inboundConfig.http_port);
    httpAuthCB->setChecked(CurrentConfig.inboundConfig.http_useAuth);
    //
    httpAuthCB->setChecked(CurrentConfig.inboundConfig.http_useAuth);
    httpAuthUsernameTxt->setEnabled(CurrentConfig.inboundConfig.http_useAuth);
    httpAuthPasswordTxt->setEnabled(CurrentConfig.inboundConfig.http_useAuth);
    httpAuthUsernameTxt->setText(CurrentConfig.inboundConfig.httpAccount.user);
    httpAuthPasswordTxt->setText(CurrentConfig.inboundConfig.httpAccount.pass);
    httpSniffingCB->setChecked(CurrentConfig.inboundConfig.httpSniffing);
    //
    //
    bool have_socks = CurrentConfig.inboundConfig.useSocks;
    socksGroupBox->setChecked(have_socks);
    socksPortLE->setValue(CurrentConfig.inboundConfig.socks_port);
    //
    socksAuthCB->setChecked(CurrentConfig.inboundConfig.socks_useAuth);
    socksAuthUsernameTxt->setEnabled(CurrentConfig.inboundConfig.socks_useAuth);
    socksAuthPasswordTxt->setEnabled(CurrentConfig.inboundConfig.socks_useAuth);
    socksAuthUsernameTxt->setText(CurrentConfig.inboundConfig.socksAccount.user);
    socksAuthPasswordTxt->setText(CurrentConfig.inboundConfig.socksAccount.pass);
    // Socks UDP Options
    socksUDPCB->setChecked(CurrentConfig.inboundConfig.socksUDP);
    socksUDPIP->setEnabled(CurrentConfig.inboundConfig.socksUDP);
    socksUDPIP->setText(CurrentConfig.inboundConfig.socksLocalIP);
    socksSniffingCB->setChecked(CurrentConfig.inboundConfig.socksSniffing);
    //
    //
    bool have_tproxy = CurrentConfig.inboundConfig.useTPROXY;
    tproxGroupBox->setChecked(have_tproxy);
    tproxyListenAddr->setText(CurrentConfig.inboundConfig.tproxy_ip);
    tProxyPort->setValue(CurrentConfig.inboundConfig.tproxy_port);
    tproxyEnableTCP->setChecked(CurrentConfig.inboundConfig.tproxy_use_tcp);
    tproxyEnableUDP->setChecked(CurrentConfig.inboundConfig.tproxy_use_udp);
    tproxyFollowRedirect->setChecked(CurrentConfig.inboundConfig.tproxy_followRedirect);
    tproxyMode->setCurrentText(CurrentConfig.inboundConfig.tproxy_mode);
    outboundMark->setValue(CurrentConfig.outboundConfig.mark);
    dnsIntercept->setChecked(CurrentConfig.inboundConfig.dnsIntercept);
    DnsFreedomCb->setChecked(CurrentConfig.connectionConfig.v2rayFreedomDNS);
    //
    //
    vCorePathTxt->setText(CurrentConfig.kernelConfig.KernelPath());
    vCoreAssetsPathTxt->setText(CurrentConfig.kernelConfig.AssetsPath());
    enableAPI->setChecked(CurrentConfig.kernelConfig.enableAPI);
    statsPortBox->setValue(CurrentConfig.kernelConfig.statsPort);
    //
    //
    bypassCNCb->setChecked(CurrentConfig.connectionConfig.bypassCN);
    bypassBTCb->setChecked(CurrentConfig.connectionConfig.bypassBT);
    proxyDefaultCb->setChecked(CurrentConfig.connectionConfig.enableProxy);
    //
    localDNSCb->setChecked(CurrentConfig.connectionConfig.withLocalDNS);
    //
    pluginKernelV2rayIntegrationCB->setChecked(CurrentConfig.pluginConfig.v2rayIntegration);
    pluginKernelPortAllocateCB->setValue(CurrentConfig.pluginConfig.portAllocationStart);
    //
    qvProxyPortCB->setValue(CurrentConfig.networkConfig.port);
    qvProxyAddressTxt->setText(CurrentConfig.networkConfig.address);
    qvProxyTypeCombo->setCurrentText(CurrentConfig.networkConfig.type);
    qvNetworkUATxt->setText(CurrentConfig.networkConfig.userAgent);
    switch (CurrentConfig.networkConfig.proxyType)
    {
        case Qv2rayConfig_Network::QVPROXY_NONE:
        {
            qvProxyNoProxy->setChecked(true);
            break;
        }
        case Qv2rayConfig_Network::QVPROXY_SYSTEM:
        {
            qvProxySystemProxy->setChecked(true);
            break;
        }
        case Qv2rayConfig_Network::QVPROXY_CUSTOM:
        {
            qvProxyCustomProxy->setChecked(true);
            break;
        }
    }
    //
    quietModeCB->setChecked(CurrentConfig.uiConfig.quietMode);
    //
    // Advanced config.
    setAllowInsecureCB->setChecked(CurrentConfig.advancedConfig.setAllowInsecure);
    setAllowInsecureCiphersCB->setChecked(CurrentConfig.advancedConfig.setAllowInsecureCiphers);
    setTestLatenctCB->setChecked(CurrentConfig.advancedConfig.testLatencyPeriodcally);
    //
    DNSListTxt->clear();
    for (const auto &dnsStr : CurrentConfig.connectionConfig.dnsList)
    {
        auto str = dnsStr.trimmed();
        if (!str.isEmpty())
        {
            DNSListTxt->appendPlainText(str);
        }
    }

#ifdef DISABLE_AUTO_UPDATE
    updateSettingsGroupBox->setEnabled(false);
    updateSettingsGroupBox->setToolTip(tr("Update is disabled by your vendor."));
#endif
    //
    updateChannelCombo->setCurrentIndex(CurrentConfig.updateConfig.updateChannel);
    cancelIgnoreVersionBtn->setEnabled(!CurrentConfig.updateConfig.ignoredVersion.isEmpty());
    ignoredNextVersion->setText(CurrentConfig.updateConfig.ignoredVersion);
    //
    // Empty for global config.
    auto autoStartConnId = CurrentConfig.autoStartId.connectionId;
    auto autoStartGroupId = CurrentConfig.autoStartId.groupId;

    for (const auto &group : ConnectionManager->AllGroups())
    {
        autoStartSubsCombo->addItem(GetDisplayName(group), group.toString());
    }

    autoStartSubsCombo->setCurrentText(GetDisplayName(autoStartGroupId));

    for (const auto &conn : ConnectionManager->Connections(autoStartGroupId))
    {
        autoStartConnCombo->addItem(GetDisplayName(conn), conn.toString());
    }

    autoStartConnCombo->setCurrentText(GetDisplayName(autoStartConnId));

    // FP Settings
    if (CurrentConfig.connectionConfig.forwardProxyConfig.type.trimmed().isEmpty())
    {
        CurrentConfig.connectionConfig.forwardProxyConfig.type = "http";
    }

    fpGroupBox->setChecked(CurrentConfig.connectionConfig.forwardProxyConfig.enableForwardProxy);
    fpUsernameTx->setText(CurrentConfig.connectionConfig.forwardProxyConfig.username);
    fpPasswordTx->setText(CurrentConfig.connectionConfig.forwardProxyConfig.password);
    fpAddressTx->setText(CurrentConfig.connectionConfig.forwardProxyConfig.serverAddress);
    fpTypeCombo->setCurrentText(CurrentConfig.connectionConfig.forwardProxyConfig.type);
    fpPortSB->setValue(CurrentConfig.connectionConfig.forwardProxyConfig.port);
    fpUseAuthCB->setChecked(CurrentConfig.connectionConfig.forwardProxyConfig.useAuth);
    fpUsernameTx->setEnabled(fpUseAuthCB->isChecked());
    fpPasswordTx->setEnabled(fpUseAuthCB->isChecked());
    //
    maxLogLinesSB->setValue(CurrentConfig.uiConfig.maximumLogLines);
    //
    setSysProxyCB->setChecked(CurrentConfig.inboundConfig.setSystemProxy);
    //
    finishedLoading = true;
    routeSettingsWidget = new RouteSettingsMatrixWidget(CurrentConfig.kernelConfig.AssetsPath(), this);
    routeSettingsWidget->SetRouteConfig(CurrentConfig.connectionConfig.routeConfig);
    advRouteSettingsLayout->addWidget(routeSettingsWidget);
}

QvMessageBusSlotImpl(PreferencesWindow)
{
    switch (msg)
    {
        case UPDATE_COLORSCHEME:
            break; //
            MBShowDefaultImpl MBHideDefaultImpl MBRetranslateDefaultImpl
    }
}

PreferencesWindow::~PreferencesWindow()
{
}

void PreferencesWindow::on_buttonBox_accepted()
{
    // Note:
    // A signal-slot connection from buttonbox_accpted to QDialog::accepted()
    // has been removed. To prevent closing this Dialog.
    QSet<int> ports;
    auto size = 0;

    if (CurrentConfig.inboundConfig.useHTTP)
    {
        size++;
        ports << CurrentConfig.inboundConfig.http_port;
    }

    if (CurrentConfig.inboundConfig.useSocks)
    {
        size++;
        ports << CurrentConfig.inboundConfig.socks_port;
    }

    if (CurrentConfig.inboundConfig.useTPROXY)
    {
        size++;
        ports << CurrentConfig.inboundConfig.tproxy_port;
    }

    if (!StartupOption.noAPI)
    {
        size++;
        ports << CurrentConfig.kernelConfig.statsPort;
    }

    if (ports.size() != size)
    {
        // Duplicates detected.
        QvMessageBoxWarn(this, tr("Preferences"), tr("Duplicated port numbers detected, please check the port number settings."));
    }
    else if (CurrentConfig.inboundConfig.listenip.toLower() != "localhost" && !IsValidIPAddress(CurrentConfig.inboundConfig.listenip))
    {
        QvMessageBoxWarn(this, tr("Preferences"), tr("Invalid inbound listening address."));
    }
    else
    {
        if (CurrentConfig.uiConfig.language != GlobalConfig.uiConfig.language)
        {
            // Install translator
            if (Qv2rayTranslator->InstallTranslation(CurrentConfig.uiConfig.language))
            {
                UIMessageBus.EmitGlobalSignal(QvMBMessage::RETRANSLATE);
                QApplication::processEvents();
            }
            else
            {
                LOG(MODULE_UI, "Failed to translate UI to: " + CurrentConfig.uiConfig.language)
            }
        }
        CurrentConfig.connectionConfig.routeConfig = routeSettingsWidget->GetRouteConfig();
        if (!(CurrentConfig.connectionConfig.routeConfig == GlobalConfig.connectionConfig.routeConfig))
        {
            NEEDRESTART
        }
        StyleManager->ApplyStyle(CurrentConfig.uiConfig.theme);
        SaveGlobalSettings(CurrentConfig);
        UIMessageBus.EmitGlobalSignal(QvMBMessage::UPDATE_COLORSCHEME);
        if (NeedRestart)
        {
            this->setEnabled(false);
            qApp->processEvents();
            ConnectionManager->RestartConnection();
        }
        emit accept();
    }
}

void PreferencesWindow::on_httpAuthCB_stateChanged(int checked)
{
    NEEDRESTART
    bool enabled = checked == Qt::Checked;
    httpAuthUsernameTxt->setEnabled(enabled);
    httpAuthPasswordTxt->setEnabled(enabled);
    CurrentConfig.inboundConfig.http_useAuth = enabled;
}

void PreferencesWindow::on_socksAuthCB_stateChanged(int checked)
{
    NEEDRESTART
    bool enabled = checked == Qt::Checked;
    socksAuthUsernameTxt->setEnabled(enabled);
    socksAuthPasswordTxt->setEnabled(enabled);
    CurrentConfig.inboundConfig.socks_useAuth = enabled;
}

void PreferencesWindow::on_languageComboBox_currentTextChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.language = arg1;
}

void PreferencesWindow::on_logLevelComboBox_currentIndexChanged(int index)
{
    NEEDRESTART
    CurrentConfig.logLevel = index;
}

void PreferencesWindow::on_vCoreAssetsPathTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.kernelConfig.AssetsPath(arg1);
}

void PreferencesWindow::on_listenIPTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.listenip = arg1;

    if (IsValidIPAddress(arg1))
    {
        BLACK(listenIPTxt)
    }
    else
    {
        RED(listenIPTxt)
    }

    // pacAccessPathTxt->setText("http://" + arg1 + ":" +
    // QSTRN(pacPortSB->value()) + "/pac");
}

void PreferencesWindow::on_httpAuthUsernameTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.httpAccount.user = arg1;
}

void PreferencesWindow::on_httpAuthPasswordTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.httpAccount.pass = arg1;
}

void PreferencesWindow::on_socksAuthUsernameTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksAccount.user = arg1;
}

void PreferencesWindow::on_socksAuthPasswordTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksAccount.pass = arg1;
}

void PreferencesWindow::on_proxyDefaultCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.connectionConfig.enableProxy = arg1 == Qt::Checked;
}

void PreferencesWindow::on_localDNSCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.connectionConfig.withLocalDNS = arg1 == Qt::Checked;
}

void PreferencesWindow::on_selectVAssetBtn_clicked()
{
    NEEDRESTART
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open V2ray assets folder"), QDir::currentPath());

    if (!dir.isEmpty())
    {
        vCoreAssetsPathTxt->setText(dir);
        on_vCoreAssetsPathTxt_textEdited(dir);
    }
}

void PreferencesWindow::on_selectVCoreBtn_clicked()
{
    QString core = QFileDialog::getOpenFileName(this, tr("Open V2ray core file"), QDir::currentPath());

    if (!core.isEmpty())
    {
        vCorePathTxt->setText(core);
        on_vCorePathTxt_textEdited(core);
    }
}

void PreferencesWindow::on_vCorePathTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.kernelConfig.KernelPath(arg1);
}

void PreferencesWindow::on_DNSListTxt_textChanged()
{
    LOADINGCHECK
    try
    {
        QStringList hosts = DNSListTxt->toPlainText().replace("\r", "").split("\n");
        CurrentConfig.connectionConfig.dnsList.clear();

        for (const auto &host : hosts)
        {
            if (host != "" && host != "\r")
            {
                // Not empty, so we save.
                CurrentConfig.connectionConfig.dnsList.push_back(host);
                NEEDRESTART
            }
        }

        BLACK(DNSListTxt)
    }
    catch (...)
    {
        RED(DNSListTxt)
    }
}

void PreferencesWindow::on_aboutQt_clicked()
{
    QApplication::aboutQt();
}

void PreferencesWindow::on_cancelIgnoreVersionBtn_clicked()
{
    CurrentConfig.updateConfig.ignoredVersion.clear();
    cancelIgnoreVersionBtn->setEnabled(false);
}

void PreferencesWindow::on_tProxyCheckBox_stateChanged(int arg1)
{
    LOADINGCHECK
#ifdef Q_OS_LINUX

    // Setting up tProxy for linux
    // Steps:
    // --> 1. Copy V2ray core files to the QV2RAY_TPROXY_VCORE_PATH and
    // QV2RAY_TPROXY_VCTL_PATH dir.
    // --> 2. Change GlobalConfig.v2CorePath.
    // --> 3. Call `pkexec setcap
    // CAP_NET_ADMIN,CAP_NET_RAW,CAP_NET_BIND_SERVICE=eip` on the V2ray core.
    auto const kernelPath = CurrentConfig.kernelConfig.KernelPath();
    if (arg1 == Qt::Checked)
    {
        // We enable it!
        if (QvMessageBoxAsk(this, tr("Enable tProxy Support"),
                            tr("This will append capabilities to the V2ray executable.") + NEWLINE + NEWLINE +
                                tr("Qv2ray will copy your V2ray core to this path: ") + NEWLINE + QV2RAY_TPROXY_VCORE_PATH + NEWLINE + NEWLINE +
                                tr("If anything goes wrong after enabling this, please check issue #57 or the link below:") + NEWLINE +
                                " https://github.com/Qv2ray/Qv2ray/wiki/FAQ ") != QMessageBox::Yes)
        {
            tProxyCheckBox->setChecked(false);
            LOG(MODULE_UI, "Canceled enabling tProxy feature.")
        }
        else
        {

            LOG(MODULE_VCORE, "ENABLING tProxy Support")
            LOG(MODULE_FILEIO, " --> Origin V2ray core file is at: " + kernelPath)
            auto v2ctlPath = QFileInfo(kernelPath).absolutePath() + "/v2ctl";
            auto newPath = QFileInfo(QV2RAY_TPROXY_VCORE_PATH).absolutePath();
            QString mkPathResult = QDir().mkpath(newPath) ? "OK" : "FAILED";
            LOG(MODULE_FILEIO, " --> mkPath result: " + mkPathResult)
            //
            LOG(MODULE_FILEIO, " --> Origin v2ctl file is at: " + v2ctlPath)
            LOG(MODULE_FILEIO, " --> New V2ray files will be placed in: " + newPath)
            //
            LOG(MODULE_FILEIO, " --> Copying files....")

            if (QFileInfo(kernelPath).absoluteFilePath() != QFileInfo(QV2RAY_TPROXY_VCORE_PATH).absoluteFilePath())
            {
                // Only trying to remove file when they are not in the default
                // dir. (In other words...) Keep using the current files.
                // <Because we don't know where else we can copy the file
                // from...>
                //
                if (QFile(QV2RAY_TPROXY_VCORE_PATH).exists())
                {
                    LOG(MODULE_FILEIO, QString(QV2RAY_TPROXY_VCORE_PATH) + ": File already exists.")
                    LOG(MODULE_FILEIO, QString(QV2RAY_TPROXY_VCORE_PATH) + ": Deleting file.")
                    QFile(QV2RAY_TPROXY_VCORE_PATH).remove();
                }

                if (QFile(QV2RAY_TPROXY_VCTL_PATH).exists())
                {
                    LOG(MODULE_FILEIO, QV2RAY_TPROXY_VCTL_PATH + ": File already exists.")
                    LOG(MODULE_FILEIO, QV2RAY_TPROXY_VCTL_PATH + ": Deleting file.")
                    QFile(QV2RAY_TPROXY_VCTL_PATH).remove();
                }

                QString vCoreresult = QFile(kernelPath).copy(QV2RAY_TPROXY_VCORE_PATH) ? "OK" : "FAILED";
                LOG(MODULE_FILEIO, " --> V2ray Core: " + vCoreresult)
                //
                QString vCtlresult = QFile(v2ctlPath).copy(QV2RAY_TPROXY_VCTL_PATH) ? "OK" : "FAILED";
                LOG(MODULE_FILEIO, " --> V2ray Ctl: " + vCtlresult)
                //

                if (vCoreresult == "OK" && vCtlresult == "OK")
                {
                    LOG(MODULE_VCORE, " --> Done copying files.")
                    on_vCorePathTxt_textEdited(QV2RAY_TPROXY_VCORE_PATH);
                }
                else
                {
                    LOG(MODULE_VCORE, "FAILED to copy V2ray files. Aborting.")
                    QvMessageBoxWarn(this, tr("Enable tProxy Support"),
                                     tr("Qv2ray cannot copy one or both V2ray files from: ") + NEWLINE + NEWLINE + kernelPath + NEWLINE +
                                         v2ctlPath + NEWLINE + NEWLINE + tr("to this path: ") + NEWLINE + newPath);
                    return;
                }
            }
            else
            {
                LOG(MODULE_VCORE, "Skipped removing files since the current V2ray core is in the default path.")
                LOG(MODULE_VCORE, " --> Actually because we don't know where else to obtain the files.")
            }

            LOG(MODULE_UI, "Calling pkexec and setcap...")
            int ret = QProcess::execute("pkexec /usr/sbin/setcap CAP_NET_ADMIN,CAP_NET_RAW,CAP_NET_BIND_SERVICE=eip " + kernelPath);

            if (ret != 0)
            {
                LOG(MODULE_UI, "WARN: setcap exits with code: " + QSTRN(ret))
                QvMessageBoxWarn(this, tr("Preferences"), tr("Failed to setcap onto V2ray executable. You may need to run `setcap` manually."));
            }

            CurrentConfig.tProxySupport = true;
            NEEDRESTART
        }
    }
    else
    {
        int ret = QProcess::execute("pkexec /usr/sbin/setcap -r " + kernelPath);

        if (ret != 0)
        {
            LOG(MODULE_UI, "WARN: setcap exits with code: " + QSTRN(ret))
            QvMessageBoxWarn(this, tr("Preferences"), tr("Failed to setcap onto V2ray executable. You may need to run `setcap` manually."));
        }

        CurrentConfig.tProxySupport = false;
        NEEDRESTART
    }

#else
    Q_UNUSED(arg1)
    // No such tProxy thing on Windows and macOS
    QvMessageBoxWarn(this, tr("Preferences"), tr("tProxy is not supported on macOS and Windows"));
    CurrentConfig.tProxySupport = false;
    tProxyCheckBox->setChecked(false);
#endif
}

void PreferencesWindow::on_bypassCNCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.connectionConfig.bypassCN = arg1 == Qt::Checked;
}

void PreferencesWindow::on_bypassBTCb_stateChanged(int arg1)
{
    NEEDRESTART
    if (arg1 == Qt::Checked)
    {
        QvMessageBoxInfo(this, tr("Note"),
                         tr("To recognize the protocol of a connection, one must enable sniffing option in inbound proxy.") + NEWLINE +
                             tr("tproxy inbound's sniffing is enabled by default."));
    }
    CurrentConfig.connectionConfig.bypassBT = arg1 == Qt::Checked;
}

void PreferencesWindow::on_statsPortBox_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.kernelConfig.statsPort = arg1;
}

void PreferencesWindow::on_socksPortLE_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socks_port = arg1;
}

void PreferencesWindow::on_httpPortLE_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.http_port = arg1;
}

void PreferencesWindow::on_socksUDPCB_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksUDP = arg1 == Qt::Checked;
    socksUDPIP->setEnabled(arg1 == Qt::Checked);
}

void PreferencesWindow::on_socksUDPIP_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksLocalIP = arg1;

    if (IsValidIPAddress(arg1))
    {
        BLACK(socksUDPIP)
    }
    else
    {
        RED(socksUDPIP)
    }
}

void PreferencesWindow::on_themeCombo_currentTextChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.theme = arg1;
}

void PreferencesWindow::on_darkThemeCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.useDarkTheme = arg1 == Qt::Checked;
#if (QV2RAY_USE_BUILTIN_DARKTHEME)
    themeCombo->setEnabled(arg1 != Qt::Checked);

    if (arg1 == Qt::Checked)
    {
        themeCombo->setCurrentIndex(QStyleFactory::keys().indexOf("Fusion"));
        CurrentConfig.uiConfig.theme = "Fusion";
    }

#endif
}

void PreferencesWindow::on_darkTrayCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.useDarkTrayIcon = arg1 == Qt::Checked;
}

void PreferencesWindow::on_setSysProxyCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.setSystemProxy = arg1 == Qt::Checked;
}

void PreferencesWindow::on_autoStartSubsCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK if (arg1.isEmpty())
    {
        CurrentConfig.autoStartId.clear();
        autoStartConnCombo->clear();
    }
    else
    {
        auto list = ConnectionManager->Connections(GroupId(autoStartSubsCombo->currentData().toString()));
        autoStartConnCombo->clear();

        for (const auto &id : list)
        {
            autoStartConnCombo->addItem(GetDisplayName(id), id.toString());
        }
    }
}

void PreferencesWindow::on_autoStartConnCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    if (arg1.isEmpty())
    {
        CurrentConfig.autoStartId.clear();
    }
    else
    {
        // Fully qualify the connection item.
        // Will not work when duplicated names are in the same group.
        CurrentConfig.autoStartId.groupId = GroupId(autoStartSubsCombo->currentData().toString());
        CurrentConfig.autoStartId.connectionId = ConnectionId(autoStartConnCombo->currentData().toString());
        //= autoStartConnCombo->currentData().toString();
        //            ConnectionManager->GetConnectionIdByDisplayName(arg1,
        //            ConnectionManager->GetGroupIdByDisplayName(autoStartSubsCombo->currentText()))
        //              .toString();
    }
}

void PreferencesWindow::on_startWithLoginCB_stateChanged(int arg1)
{
    bool isEnabled = arg1 == Qt::Checked;
    SetLaunchAtLoginStatus(isEnabled);

    if (GetLaunchAtLoginStatus() != isEnabled)
    {
        QvMessageBoxWarn(this, tr("Start with boot"), tr("Failed to set auto start option."));
    }

    SetAutoStartButtonsState(GetLaunchAtLoginStatus());
}

void PreferencesWindow::SetAutoStartButtonsState(bool isAutoStart)
{
    startWithLoginCB->setChecked(isAutoStart);
}

void PreferencesWindow::on_fpTypeCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.connectionConfig.forwardProxyConfig.type = arg1.toLower();
}

void PreferencesWindow::on_fpAddressTx_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.connectionConfig.forwardProxyConfig.serverAddress = arg1;

    if (IsValidIPAddress(arg1))
    {
        BLACK(fpAddressTx)
    }
    else
    {
        RED(fpAddressTx)
    }
}

void PreferencesWindow::on_spPortSB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.connectionConfig.forwardProxyConfig.port = arg1;
}

void PreferencesWindow::on_fpUseAuthCB_stateChanged(int arg1)
{
    bool authEnabled = arg1 == Qt::Checked;
    CurrentConfig.connectionConfig.forwardProxyConfig.useAuth = authEnabled;
    fpUsernameTx->setEnabled(authEnabled);
    fpPasswordTx->setEnabled(authEnabled);
}

void PreferencesWindow::on_fpUsernameTx_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.connectionConfig.forwardProxyConfig.username = arg1;
}

void PreferencesWindow::on_fpPasswordTx_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.connectionConfig.forwardProxyConfig.password = arg1;
}

void PreferencesWindow::on_fpPortSB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.connectionConfig.forwardProxyConfig.port = arg1;
}

void PreferencesWindow::on_checkVCoreSettings_clicked()
{
    auto vcorePath = vCorePathTxt->text();
    auto vAssetsPath = vCoreAssetsPathTxt->text();
    QString result;

    if (!V2rayKernelInstance::ValidateKernel(vcorePath, vAssetsPath, &result))
    {
        QvMessageBoxWarn(this, tr("V2ray Core Settings"), result);
    }
    else
    {
        QvMessageBoxInfo(this, tr("V2ray Core Settings"),
                         tr("V2ray path configuration check passed.") + NEWLINE + NEWLINE + tr("Current version of V2ray is: ") + NEWLINE +
                             result);
    }
}

void PreferencesWindow::on_httpGroupBox_clicked(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.useHTTP = checked;
}

void PreferencesWindow::on_socksGroupBox_clicked(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.useSocks = checked;
}

void PreferencesWindow::on_fpGroupBox_clicked(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.connectionConfig.forwardProxyConfig.enableForwardProxy = checked;
}

void PreferencesWindow::on_maxLogLinesSB_valueChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.uiConfig.maximumLogLines = arg1;
}

void PreferencesWindow::on_enableAPI_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART

    CurrentConfig.kernelConfig.enableAPI = arg1 == Qt::Checked;
    if (arg1 == Qt::Unchecked)
    {
        const auto msgAPIDisableTitle = tr("Disabling API Subsystem");
        const auto msgAPIDisableMsg = tr("Disabling API subsystem will also disable the statistics function of Qv2ray.") + NEWLINE + //
                                      tr("Speed chart and traffic statistics will be disabled.");
        QvMessageBoxWarn(this, msgAPIDisableTitle, msgAPIDisableMsg);
    }
}

void PreferencesWindow::on_updateChannelCombo_currentIndexChanged(int index)
{
    LOADINGCHECK
    CurrentConfig.updateConfig.updateChannel = index;
    CurrentConfig.updateConfig.ignoredVersion.clear();
}

void PreferencesWindow::on_pluginKernelV2rayIntegrationCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.pluginConfig.v2rayIntegration = arg1 == Qt::Checked;
}

void PreferencesWindow::on_pluginKernelPortAllocateCB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.pluginConfig.portAllocationStart = arg1;
}

void PreferencesWindow::on_qvProxyAddressTxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.networkConfig.address = arg1;
}

void PreferencesWindow::on_qvProxyTypeCombo_currentTextChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.networkConfig.type = arg1.toLower();
}

void PreferencesWindow::on_qvProxyPortCB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.networkConfig.port = arg1;
}

void PreferencesWindow::on_qvNetworkUATxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentConfig.networkConfig.userAgent = arg1;
}

void PreferencesWindow::on_setAllowInsecureCB_stateChanged(int arg1)
{
    LOADINGCHECK
    if (arg1 == Qt::Checked)
    {
        QvMessageBoxWarn(this, tr("Dangerous Operation"), tr("You will lose the advantage of TLS and make your connection under MITM attack."));
    }
    CurrentConfig.advancedConfig.setAllowInsecure = arg1 == Qt::Checked;
}

void PreferencesWindow::on_setTestLatenctCB_stateChanged(int arg1)
{
    LOADINGCHECK
    if (arg1 == Qt::Checked)
    {
        QvMessageBoxWarn(this, tr("Dangerous Operation"), tr("This will (probably) make it easy to fingerprint your connection."));
    }
    CurrentConfig.advancedConfig.testLatencyPeriodcally = arg1 == Qt::Checked;
}

void PreferencesWindow::on_setAllowInsecureCiphersCB_stateChanged(int arg1)
{
    LOADINGCHECK
    if (arg1 == Qt::Checked)
    {
        QvMessageBoxWarn(this, tr("Dangerous Operation"), tr("You will lose the advantage of TLS and make your connection under MITM attack."));
    }
    CurrentConfig.advancedConfig.setAllowInsecureCiphers = arg1 == Qt::Checked;
}

void PreferencesWindow::on_quietModeCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentConfig.uiConfig.quietMode = arg1 == Qt::Checked;
}

void PreferencesWindow::on_tproxGroupBox_toggled(bool arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.useTPROXY = arg1;
}

void PreferencesWindow::on_tProxyPort_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tproxy_port = arg1;
}

void PreferencesWindow::on_tproxyEnableTCP_toggled(bool checked)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tproxy_use_tcp = checked;
}

void PreferencesWindow::on_tproxyEnableUDP_toggled(bool checked)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tproxy_use_udp = checked;
}

void PreferencesWindow::on_tproxyFollowRedirect_toggled(bool checked)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tproxy_followRedirect = checked;
}

void PreferencesWindow::on_tproxyMode_currentTextChanged(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tproxy_mode = arg1;
}

void PreferencesWindow::on_tproxyListenAddr_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.tproxy_ip = arg1;
}

void PreferencesWindow::on_jumpListCountSB_valueChanged(int arg1)
{
    CurrentConfig.uiConfig.maxJumpListCount = arg1;
}

void PreferencesWindow::on_outboundMark_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.outboundConfig.mark = arg1;
}

void PreferencesWindow::on_dnsIntercept_toggled(bool checked)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.dnsIntercept = checked;
}

void PreferencesWindow::on_qvProxyCustomProxy_clicked()
{
    CurrentConfig.networkConfig.proxyType = Qv2rayConfig_Network::QVPROXY_CUSTOM;
}

void PreferencesWindow::on_qvProxySystemProxy_clicked()
{
    CurrentConfig.networkConfig.proxyType = Qv2rayConfig_Network::QVPROXY_SYSTEM;
}

void PreferencesWindow::on_qvProxyNoProxy_clicked()
{
    CurrentConfig.networkConfig.proxyType = Qv2rayConfig_Network::QVPROXY_NONE;
}

void PreferencesWindow::on_DnsFreedomCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.connectionConfig.v2rayFreedomDNS = arg1 == Qt::Checked;
}

void PreferencesWindow::on_httpSniffingCB_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.httpSniffing = arg1 == Qt::Checked;
}

void PreferencesWindow::on_socksSniffingCB_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.socksSniffing = arg1 == Qt::Checked;
}
