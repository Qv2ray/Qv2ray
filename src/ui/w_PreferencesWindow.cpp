#include "w_PreferencesWindow.hpp"
#include <QFileDialog>
#include <QColorDialog>
#include <QStyleFactory>
#include <QStyle>
#include <QDesktopServices>

#include "common/QvHelpers.hpp"
#include "common/HTTPRequestHelper.hpp"
#include "core/config/ConfigBackend.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/kernel/KernelInteractions.hpp"
#include "components/plugins/toolbar/QvToolbar.hpp"
#include "components/autolaunch/QvAutoLaunch.hpp"
#include <common/QvTranslator.hpp>

using Qv2ray::common::validation::IsValidIPAddress;

#define LOADINGCHECK if(!finishedLoading) return;
#define NEEDRESTART if(finishedLoading) IsConnectionPropertyChanged = true;

PreferencesWindow::PreferencesWindow(QWidget *parent) : QDialog(parent),
    CurrentConfig()
{
    setupUi(this);
    QvMessageBusConnect(PreferencesWindow);
    textBrowser->setHtml(StringFromFile(":/assets/credit.html"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    //
    // Set network Toolbar page state.
    networkToolbarPage->setEnabled(StartupOption.enableToolbarPlguin);

    if (!StartupOption.enableToolbarPlguin) {
        networkToolbarInfoLabel->setText(tr("Qv2ray Network Toolbar is disabled and still under test. Add --withToolbarPlugin to enable."));
    }

    // We add locales
    languageComboBox->clear();
    QDirIterator it(":/translations");

    while (it.hasNext()) {
        languageComboBox->addItem(it.next().split("/").last().split(".").first());
    }

    // Set auto start button state
    SetAutoStartButtonsState(GetLaunchAtLoginStatus());
    //
    nsBarContentCombo->addItems(NetSpeedPluginMessages.values());
    themeCombo->addItems(QStyleFactory::keys());
    //
    qvVersion->setText(QV2RAY_VERSION_STRING);
    qvBuildInfo->setText(QV2RAY_BUILD_INFO);
    qvBuildExInfo->setText(QV2RAY_BUILD_EXTRA_INFO);
    qvBuildTime->setText(__DATE__ " " __TIME__);
    //
    // Deep copy
    CurrentConfig = GlobalConfig;
    //
    themeCombo->setCurrentText(CurrentConfig.uiConfig.theme);
    darkThemeCB->setChecked(CurrentConfig.uiConfig.useDarkTheme);
    darkTrayCB->setChecked(CurrentConfig.uiConfig.useDarkTrayIcon);
#ifdef QV2RAY_USE_BUILTIN_DARKTHEME
    // If we use built in theme, it should always be fusion.
    themeCombo->setEnabled(!CurrentConfig.uiConfig.useDarkTheme);
    darkThemeLabel->setText(tr("Use Darkmode Theme"));
#endif
    languageComboBox->setCurrentText(CurrentConfig.uiConfig.language);
    logLevelComboBox->setCurrentIndex(CurrentConfig.logLevel);
    tProxyCheckBox->setChecked(CurrentConfig.tProxySupport);
    //
    //
    listenIPTxt->setText(CurrentConfig.inboundConfig.listenip);
    bool pacEnabled = CurrentConfig.inboundConfig.pacConfig.enablePAC;
    pacGroupBox->setChecked(pacEnabled);
    setSysProxyCB->setChecked(CurrentConfig.inboundConfig.setSystemProxy);
    //
    // PAC
    pacPortSB->setValue(CurrentConfig.inboundConfig.pacConfig.port);
    pacProxyTxt->setText(CurrentConfig.inboundConfig.pacConfig.localIP);
    pacProxyCB->setCurrentIndex(CurrentConfig.inboundConfig.pacConfig.useSocksProxy ? 1 : 0);
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
    //
    //
    vCorePathTxt->setText(CurrentConfig.v2CorePath);
    vCoreAssetsPathTxt->setText(CurrentConfig.v2AssetsPath);
    enableAPI->setChecked(CurrentConfig.apiConfig.enableAPI);
    statsPortBox->setValue(CurrentConfig.apiConfig.statsPort);
    //
    //
    bypassCNCb->setChecked(CurrentConfig.connectionConfig.bypassCN);
    proxyDefaultCb->setChecked(CurrentConfig.connectionConfig.enableProxy);
    //
    localDNSCb->setChecked(CurrentConfig.connectionConfig.withLocalDNS);
    //
    DNSListTxt->clear();

    for (auto dnsStr : CurrentConfig.connectionConfig.dnsList) {
        auto str = dnsStr.trimmed();

        if (!str.isEmpty()) {
            DNSListTxt->appendPlainText(str);
        }
    }

    //
    cancelIgnoreVersionBtn->setEnabled(CurrentConfig.ignoredVersion != "");
    ignoredNextVersion->setText(CurrentConfig.ignoredVersion);

    for (auto i = 0; i < CurrentConfig.toolBarConfig.Pages.size(); i++) {
        nsBarPagesList->addItem(tr("Page") + QSTRN(i + 1) + ": " + QSTRN(CurrentConfig.toolBarConfig.Pages[i].Lines.size()) + " " + tr("Item(s)"));
    }

    if (CurrentConfig.toolBarConfig.Pages.size() > 0) {
        nsBarPagesList->setCurrentRow(0);
        on_nsBarPagesList_currentRowChanged(0);
    } else {
        networkToolbarSettingsFrame->setEnabled(false);
        nsBarLinesList->setEnabled(false);
        nsBarLineDelBTN->setEnabled(false);
        nsBarLineAddBTN->setEnabled(false);
        nsBarPageYOffset->setEnabled(false);
    }

    CurrentBarPageId = 0;
    //
    // Empty for global config.
    //auto autoSub = CurrentConfig.autoStartConfig.subscriptionName;
    //auto autoCon = CurrentConfig.autoStartConfig.connectionName;
    //autoStartConnCombo->addItem("");
    QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");

    // TODO: Now use grouping, subscriptions are the special type of group
    //autoStartConnCombo->setCurrentText(autoCon);for (auto item : CurrentConfig.subscriptions.keys()) {
    //    autoStartSubsCombo->addItem(item);
    //}
    //
    //autoStartSubsCombo->setCurrentText(autoSub);
    //
    //if (CurrentConfig.autoStartConfig.subscriptionName.isEmpty()) {
    //    autoStartConnCombo->addItems(CurrentConfig.configs);
    //} else {
    //    auto list = GetSubscriptionConnection(autoSub);
    //    autoStartConnCombo->addItems(list.keys());
    //}
    //
    //

    // FP Settings
    if (CurrentConfig.connectionConfig.forwardProxyConfig.type.trimmed().isEmpty()) {
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
    pacListenAddrLabel->setText("http://" + (pacProxyTxt->text().isEmpty() ? "127.0.0.1" : pacProxyTxt->text()) + ":" + QSTRN(pacPortSB->value()) + "/pac");
    //
    finishedLoading = true;
}


QvMessageBusSlotImpl(PreferencesWindow)
{
    switch (msg) {
            QvMessageBusShowDefault
            QvMessageBusHideDefault
            QvMessageBusRetranslateDefault
    }
}

PreferencesWindow::~PreferencesWindow()
{
}

void PreferencesWindow::on_buttonBox_accepted()
{
    // Note:
    // A signal-slot connection from buttonbox_accpted to QDialog::accepted() has been removed.
    // To prevent closing this Dialog.
    QSet<int> ports;
    auto size = 0;

    if (CurrentConfig.inboundConfig.useHTTP) {
        size ++;
        ports << CurrentConfig.inboundConfig.http_port;
    }

    if (CurrentConfig.inboundConfig.useSocks) {
        size ++;
        ports << CurrentConfig.inboundConfig.socks_port;
    }

    if (CurrentConfig.inboundConfig.pacConfig.enablePAC) {
        size ++;
        ports << CurrentConfig.inboundConfig.pacConfig.port;
    }

    if (!StartupOption.noAPI) {
        size ++;
        ports << CurrentConfig.apiConfig.statsPort;
    }

    if (ports.size() != size) {
        // Duplicates detected.
        QvMessageBoxWarn(this, tr("Preferences"), tr("Duplicated port numbers detected, please check the port number settings."));
    } else if (CurrentConfig.inboundConfig.listenip.toLower() != "localhost" && !IsValidIPAddress(CurrentConfig.inboundConfig.listenip)) {
        QvMessageBoxWarn(this, tr("Preferences"), tr("Invalid inbound listening address."));;
    } else {
        if (CurrentConfig.uiConfig.language != GlobalConfig.uiConfig.language) {
            qApp->removeTranslator(Qv2rayTranslator.get());
            Qv2rayTranslator = std::move(QvTranslator(CurrentConfig.uiConfig.language).pTranslator);

            // Install translator
            if (!qApp->installTranslator(Qv2rayTranslator.get())) {
                LOG(MODULE_UI, "Failed to translate UI to: " + CurrentConfig.uiConfig.language)
            } else {
                messageBus.EmitGlobalSignal(QvMessage::RETRANSLATE);
                QApplication::processEvents();
            }
        }

        qApp->setStyle(QStyleFactory::create(CurrentConfig.uiConfig.theme));
        SaveGlobalConfig(CurrentConfig);
        emit s_reload_config(IsConnectionPropertyChanged);
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
    CurrentConfig.v2AssetsPath = arg1;
}

void PreferencesWindow::on_listenIPTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inboundConfig.listenip = arg1;

    if (IsValidIPAddress(arg1)) {
        BLACK(listenIPTxt)
    } else {
        RED(listenIPTxt)
    }

    //pacAccessPathTxt->setText("http://" + arg1 + ":" + QSTRN(pacPortSB->value()) + "/pac");
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

    if (!dir.isEmpty()) {
        vCoreAssetsPathTxt->setText(dir);
        on_vCoreAssetsPathTxt_textEdited(dir);
    }
}

void PreferencesWindow::on_selectVCoreBtn_clicked()
{
    QString core = QFileDialog::getOpenFileName(this, tr("Open V2ray core file"), QDir::currentPath());

    if (!core.isEmpty()) {
        vCorePathTxt->setText(core);
        on_vCorePathTxt_textEdited(core);
    }
}

void PreferencesWindow::on_vCorePathTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.v2CorePath = arg1;
}

void PreferencesWindow::on_DNSListTxt_textChanged()
{
    if (finishedLoading) {
        try {
            QStringList hosts = DNSListTxt->toPlainText().replace("\r", "").split("\n");
            CurrentConfig.connectionConfig.dnsList.clear();

            foreach (auto host, hosts) {
                if (host != "" && host != "\r") {
                    // Not empty, so we save.
                    CurrentConfig.connectionConfig.dnsList.push_back(host);
                    NEEDRESTART
                }
            }

            BLACK(DNSListTxt)
        } catch (...) {
            RED(DNSListTxt)
        }
    }
}

void PreferencesWindow::on_aboutQt_clicked()
{
    QApplication::aboutQt();
}

void PreferencesWindow::on_cancelIgnoreVersionBtn_clicked()
{
    CurrentConfig.ignoredVersion.clear();
    cancelIgnoreVersionBtn->setEnabled(false);
}

void PreferencesWindow::on_tProxyCheckBox_stateChanged(int arg1)
{
    LOADINGCHECK
#ifdef Q_OS_LINUX

    // Setting up tProxy for linux
    // Steps:
    // --> 1. Copy V2ray core files to the QV2RAY_TPROXY_VCORE_PATH and QV2RAY_TPROXY_VCTL_PATH dir.
    // --> 2. Change GlobalConfig.v2CorePath.
    // --> 3. Call `pkexec setcap CAP_NET_ADMIN,CAP_NET_RAW,CAP_NET_BIND_SERVICE=eip` on the V2ray core.
    if (arg1 == Qt::Checked) {
        // We enable it!
        if (QvMessageBoxAsk(this, tr("Enable tProxy Support"),
                            tr("This will append capabilities to the V2ray executable.")  + NEWLINE + NEWLINE +
                            tr("Qv2ray will copy your V2ray core to this path: ") + NEWLINE + QV2RAY_TPROXY_VCORE_PATH + NEWLINE + NEWLINE +
                            tr("If anything goes wrong after enabling this, please check issue #57 or the link below:") + NEWLINE +
                            " https://github.com/Qv2ray/Qv2ray/wiki/FAQ ") != QMessageBox::Yes) {
            tProxyCheckBox->setChecked(false);
            LOG(MODULE_UI, "Canceled enabling tProxy feature.")
        } else {
            LOG(MODULE_VCORE, "ENABLING tProxy Support")
            LOG(MODULE_FILEIO, " --> Origin V2ray core file is at: " + CurrentConfig.v2CorePath)
            auto v2ctlPath = QFileInfo(CurrentConfig.v2CorePath).absolutePath() + "/v2ctl";
            auto newPath = QFileInfo(QV2RAY_TPROXY_VCORE_PATH).absolutePath();
            QString mkPathResult = QDir().mkpath(newPath) ? "OK" : "FAILED";
            LOG(MODULE_FILEIO, " --> mkPath result: " + mkPathResult)
            //
            LOG(MODULE_FILEIO, " --> Origin v2ctl file is at: " + v2ctlPath)
            LOG(MODULE_FILEIO, " --> New V2ray files will be placed in: " + newPath)
            //
            LOG(MODULE_FILEIO, " --> Copying files....")

            if (QFileInfo(CurrentConfig.v2CorePath).absoluteFilePath() !=  QFileInfo(QV2RAY_TPROXY_VCORE_PATH).absoluteFilePath()) {
                // Only trying to remove file when they are not in the default dir.
                // (In other words...) Keep using the current files. <Because we don't know where else we can copy the file from...>
                //
                if (QFile(QV2RAY_TPROXY_VCORE_PATH).exists()) {
                    LOG(MODULE_FILEIO, QString(QV2RAY_TPROXY_VCORE_PATH) + ": File already exists.")
                    LOG(MODULE_FILEIO, QString(QV2RAY_TPROXY_VCORE_PATH) + ": Deleting file.")
                    QFile(QV2RAY_TPROXY_VCORE_PATH).remove();
                }

                if (QFile(QV2RAY_TPROXY_VCTL_PATH).exists()) {
                    LOG(MODULE_FILEIO, QV2RAY_TPROXY_VCTL_PATH + ": File already exists.")
                    LOG(MODULE_FILEIO, QV2RAY_TPROXY_VCTL_PATH + ": Deleting file.")
                    QFile(QV2RAY_TPROXY_VCTL_PATH).remove();
                }

                QString vCoreresult = QFile(CurrentConfig.v2CorePath).copy(QV2RAY_TPROXY_VCORE_PATH) ? "OK" : "FAILED";
                LOG(MODULE_FILEIO, " --> V2ray Core: " + vCoreresult)
                //
                QString vCtlresult = QFile(v2ctlPath).copy(QV2RAY_TPROXY_VCTL_PATH) ? "OK" : "FAILED";
                LOG(MODULE_FILEIO, " --> V2ray Ctl: " + vCtlresult)
                //

                if (vCoreresult == "OK" && vCtlresult == "OK") {
                    LOG(MODULE_VCORE, " --> Done copying files.")
                    on_vCorePathTxt_textEdited(QV2RAY_TPROXY_VCORE_PATH);
                } else {
                    LOG(MODULE_VCORE, "FAILED to copy V2ray files. Aborting.")
                    QvMessageBoxWarn(this, tr("Enable tProxy Support"),
                                     tr("Qv2ray cannot copy one or both V2ray files from: ") + NEWLINE + NEWLINE +
                                     CurrentConfig.v2CorePath + NEWLINE + v2ctlPath + NEWLINE + NEWLINE +
                                     tr("to this path: ") + NEWLINE + newPath);
                    return;
                }
            } else {
                LOG(MODULE_VCORE, "Skipped removing files since the current V2ray core is in the default path.")
                LOG(MODULE_VCORE, " --> Actually because we don't know where else to obtain the files.")
            }

            LOG(MODULE_UI, "Calling pkexec and setcap...")
            int ret = QProcess::execute("pkexec setcap CAP_NET_ADMIN,CAP_NET_RAW,CAP_NET_BIND_SERVICE=eip " + CurrentConfig.v2CorePath);

            if (ret != 0) {
                LOG(MODULE_UI, "WARN: setcap exits with code: " + QSTRN(ret))
                QvMessageBoxWarn(this, tr("Preferences"), tr("Failed to setcap onto V2ray executable. You may need to run `setcap` manually."));
            }

            CurrentConfig.tProxySupport = true;
            NEEDRESTART
        }
    } else {
        int ret = QProcess::execute("pkexec setcap -r " + CurrentConfig.v2CorePath);

        if (ret != 0) {
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

void PreferencesWindow::on_statsPortBox_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.apiConfig.statsPort = arg1;
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

    if (IsValidIPAddress(arg1)) {
        BLACK(socksUDPIP)
    } else {
        RED(socksUDPIP)
    }
}

// ------------------- NET SPEED PLUGIN OPERATIONS -----------------------------------------------------------------

#define CurrentBarPage CurrentConfig.toolBarConfig.Pages[this->CurrentBarPageId]
#define CurrentBarLine CurrentBarPage.Lines[this->CurrentBarLineId]
#define SET_LINE_LIST_TEXT nsBarLinesList->currentItem()->setText(GetBarLineDescription(CurrentBarLine));

void PreferencesWindow::on_nsBarPageAddBTN_clicked()
{
    QvBarPage page;
    CurrentConfig.toolBarConfig.Pages.push_back(page);
    CurrentBarPageId = CurrentConfig.toolBarConfig.Pages.size() - 1 ;
    // Add default line.
    QvBarLine line;
    CurrentBarPage.Lines.push_back(line);
    CurrentBarLineId = 0;
    nsBarPagesList->addItem(QSTRN(CurrentBarPageId));
    ShowLineParameters(CurrentBarLine);
    LOG(MODULE_UI, "Adding new page Id: " + QSTRN(CurrentBarPageId))
    nsBarPageDelBTN->setEnabled(true);
    nsBarLineAddBTN->setEnabled(true);
    nsBarLineDelBTN->setEnabled(true);
    nsBarLinesList->setEnabled(true);
    nsBarPageYOffset->setEnabled(true);
    on_nsBarPagesList_currentRowChanged(static_cast<int>(CurrentBarPageId));
    nsBarPagesList->setCurrentRow(static_cast<int>(CurrentBarPageId));
}

void PreferencesWindow::on_nsBarPageDelBTN_clicked()
{
    if (nsBarPagesList->currentRow() >= 0) {
        CurrentConfig.toolBarConfig.Pages.removeAt(nsBarPagesList->currentRow());
        nsBarPagesList->takeItem(nsBarPagesList->currentRow());

        if (nsBarPagesList->count() <= 0) {
            nsBarPageDelBTN->setEnabled(false);
            nsBarLineAddBTN->setEnabled(false);
            nsBarLineDelBTN->setEnabled(false);
            nsBarLinesList->setEnabled(false);
            networkToolbarSettingsFrame->setEnabled(false);
            nsBarPageYOffset->setEnabled(false);
            nsBarLinesList->clear();
        }
    }
}

void PreferencesWindow::on_nsBarPageYOffset_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarPage.OffsetYpx = arg1;
}

void PreferencesWindow::on_nsBarLineAddBTN_clicked()
{
    // WARNING Is it really just this simple?
    QvBarLine line;
    CurrentBarPage.Lines.push_back(line);
    CurrentBarLineId = CurrentBarPage.Lines.size() - 1;
    nsBarLinesList->addItem(QSTRN(CurrentBarLineId));
    ShowLineParameters(CurrentBarLine);
    nsBarLineDelBTN->setEnabled(true);
    LOG(MODULE_UI, "Adding new line Id: " + QSTRN(CurrentBarLineId))
    nsBarLinesList->setCurrentRow(static_cast<int>(CurrentBarPage.Lines.size() - 1));
}

void PreferencesWindow::on_nsBarLineDelBTN_clicked()
{
    if (nsBarLinesList->currentRow() >= 0) {
        CurrentBarPage.Lines.removeAt(nsBarLinesList->currentRow());
        nsBarLinesList->takeItem(nsBarLinesList->currentRow());
        CurrentBarLineId = 0;

        if (nsBarLinesList->count() <= 0) {
            networkToolbarSettingsFrame->setEnabled(false);
            nsBarLineDelBTN->setEnabled(false);
        }

        // TODO Disabling some UI;
    }
}

void PreferencesWindow::on_nsBarPagesList_currentRowChanged(int currentRow)
{
    if (currentRow < 0) return;

    // Change page.
    // We reload the lines
    // Set all parameters item to the property of the first line.
    CurrentBarPageId = currentRow;
    CurrentBarLineId = 0;
    nsBarPageYOffset->setValue(CurrentBarPage.OffsetYpx);
    nsBarLinesList->clear();

    if (!CurrentBarPage.Lines.empty()) {
        for (auto line : CurrentBarPage.Lines) {
            auto description = GetBarLineDescription(line);
            nsBarLinesList->addItem(description);
        }

        nsBarLinesList->setCurrentRow(0);
        ShowLineParameters(CurrentBarLine);
    } else {
        networkToolbarSettingsFrame->setEnabled(false);
    }
}

void PreferencesWindow::on_nsBarLinesList_currentRowChanged(int currentRow)
{
    if (currentRow < 0) return;

    CurrentBarLineId = currentRow;
    ShowLineParameters(CurrentBarLine);
}

void PreferencesWindow::on_fontComboBox_currentFontChanged(const QFont &f)
{
    LOADINGCHECK
    CurrentBarLine.Family = f.family();
    SET_LINE_LIST_TEXT
}

void PreferencesWindow::on_nsBarFontBoldCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.Bold = arg1 == Qt::Checked;
    SET_LINE_LIST_TEXT
}

void PreferencesWindow::on_nsBarFontItalicCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.Italic = arg1 == Qt::Checked;
    SET_LINE_LIST_TEXT
}

void PreferencesWindow::on_nsBarFontASB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.ColorA = arg1;
    ShowLineParameters(CurrentBarLine);
    SET_LINE_LIST_TEXT
}

void PreferencesWindow::on_nsBarFontRSB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.ColorR = arg1;
    ShowLineParameters(CurrentBarLine);
    SET_LINE_LIST_TEXT
}

void PreferencesWindow::on_nsBarFontGSB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.ColorG = arg1;
    ShowLineParameters(CurrentBarLine);
    SET_LINE_LIST_TEXT
}

void PreferencesWindow::on_nsBarFontBSB_valueChanged(int arg1)
{
    LOADINGCHECK
    CurrentBarLine.ColorB = arg1;
    ShowLineParameters(CurrentBarLine);
    SET_LINE_LIST_TEXT
}

void PreferencesWindow::on_nsBarFontSizeSB_valueChanged(double arg1)
{
    LOADINGCHECK
    CurrentBarLine.Size = arg1;
    SET_LINE_LIST_TEXT
}

QString PreferencesWindow::GetBarLineDescription(QvBarLine barLine)
{
    QString result = "Empty";
    result = NetSpeedPluginMessages[barLine.ContentType];

    if (barLine.ContentType == 0) {
        result +=  " (" + barLine.Message + ")";
    }

    result = result.append(barLine.Bold ?  ", " + tr("Bold") : "");
    result = result.append(barLine.Italic ? ", " + tr("Italic") : "");
    return result;
}

void PreferencesWindow::ShowLineParameters(QvBarLine &barLine)
{
    finishedLoading = false;

    if (!barLine.Family.isEmpty()) {
        fontComboBox->setCurrentFont(QFont(barLine.Family));
    }

    // Colors
    nsBarFontASB->setValue(barLine.ColorA);
    nsBarFontBSB->setValue(barLine.ColorB);
    nsBarFontGSB->setValue(barLine.ColorG);
    nsBarFontRSB->setValue(barLine.ColorR);
    //
    QColor color = QColor::fromRgb(barLine.ColorR, barLine.ColorG, barLine.ColorB, barLine.ColorA);
    QString s(QStringLiteral("background: #")
              + ((color.red() < 16) ? "0" : "") + QString::number(color.red(), 16)
              + ((color.green() < 16) ? "0" : "") + QString::number(color.green(), 16)
              + ((color.blue() < 16) ? "0" : "") + QString::number(color.blue(), 16) + ";");
    chooseColorBtn->setStyleSheet(s);
    nsBarFontSizeSB->setValue(barLine.Size);
    nsBarFontBoldCB->setChecked(barLine.Bold);
    nsBarFontItalicCB->setChecked(barLine.Italic);
    nsBarContentCombo->setCurrentText(NetSpeedPluginMessages[barLine.ContentType]);
    nsBarTagTxt->setText(barLine.Message);
    finishedLoading = true;
    networkToolbarSettingsFrame->setEnabled(true);
}

void PreferencesWindow::on_chooseColorBtn_clicked()
{
    LOADINGCHECK
    QColorDialog d(QColor::fromRgb(CurrentBarLine.ColorR, CurrentBarLine.ColorG, CurrentBarLine.ColorB, CurrentBarLine.ColorA), this);
    d.exec();

    if (d.result() == QDialog::DialogCode::Accepted) {
        d.selectedColor().getRgb(&CurrentBarLine.ColorR, &CurrentBarLine.ColorG, &CurrentBarLine.ColorB, &CurrentBarLine.ColorA);
        ShowLineParameters(CurrentBarLine);
        SET_LINE_LIST_TEXT
    }
}

void PreferencesWindow::on_nsBarTagTxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentBarLine.Message = arg1;
    SET_LINE_LIST_TEXT
}

void PreferencesWindow::on_nsBarContentCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    CurrentBarLine.ContentType = NetSpeedPluginMessages.key(arg1);
    SET_LINE_LIST_TEXT
}

void PreferencesWindow::on_applyNSBarSettingsBtn_clicked()
{
    if (QvMessageBoxAsk(this, tr("Apply network toolbar settings"), tr("All other modified settings will be applied as well after this object.") +
                        NEWLINE +
                        tr("Do you want to continue?")) == QMessageBox::Yes) {
        auto conf = GlobalConfig;
        conf.toolBarConfig = CurrentConfig.toolBarConfig;
        SaveGlobalConfig(conf);
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
    QvMessageBoxWarn(this, tr("Dark Mode"), tr("Please restart Qv2ray to fully apply this feature."));
#ifdef QV2RAY_USE_BUILTIN_DARKTHEME
    themeCombo->setEnabled(arg1 != Qt::Checked);

    if (arg1 == Qt::Checked) {
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

void PreferencesWindow::on_pacGoBtn_clicked()
{
    LOADINGCHECK
    QString gfwLocation;
    QString fileContent;
    pacGoBtn->setEnabled(false);
    gfwListCB->setEnabled(false);
    QvHttpRequestHelper request;
    LOG(MODULE_PROXY, "Downloading GFWList file.")
    bool withProxy = getGFWListWithProxyCB->isChecked();

    switch (gfwListCB->currentIndex()) {
        case 0:
            gfwLocation = "https://gitlab.com/gfwlist/gfwlist/raw/master/gfwlist.txt";
            fileContent = QString::fromUtf8(request.syncget(gfwLocation, withProxy));
            break;

        case 1:
            gfwLocation = "https://pagure.io/gfwlist/raw/master/f/gfwlist.txt";
            fileContent = QString::fromUtf8(request.syncget(gfwLocation, withProxy));
            break;

        case 2:
            gfwLocation = "http://repo.or.cz/gfwlist.git/blob_plain/HEAD:/gfwlist.txt";
            fileContent = QString::fromUtf8(request.syncget(gfwLocation, withProxy));
            break;

        case 3:
            gfwLocation = "https://bitbucket.org/gfwlist/gfwlist/raw/HEAD/gfwlist.txt";
            fileContent = QString::fromUtf8(request.syncget(gfwLocation, withProxy));
            break;

        case 4:
            gfwLocation = "https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt";
            fileContent = QString::fromUtf8(request.syncget(gfwLocation, withProxy));
            break;

        case 5:
            gfwLocation = "https://git.tuxfamily.org/gfwlist/gfwlist.git/plain/gfwlist.txt";
            fileContent = QString::fromUtf8(request.syncget(gfwLocation, withProxy));
            break;

        case 6:
            auto file = QFileDialog::getOpenFileName(this, tr("Select GFWList in base64"));

            if (file.isEmpty()) {
                QvMessageBoxWarn(this, tr("Download GFWList"), tr("Operation is cancelled."));
                return;
            }

            fileContent = StringFromFile(file);
            break;
    }

    LOG(MODULE_NETWORK, "Fetched: " + gfwLocation)
    QvMessageBoxInfo(this, tr("Download GFWList"), tr("Successfully downloaded GFWList."));
    pacGoBtn->setEnabled(true);
    gfwListCB->setEnabled(true);

    if (!QDir(QV2RAY_RULES_DIR).exists()) {
        QDir(QV2RAY_RULES_DIR).mkpath(QV2RAY_RULES_DIR);
    }

    QFile privateGFWListFile(QV2RAY_RULES_GFWLIST_PATH);
    StringToFile(&fileContent, &privateGFWListFile);
}

void PreferencesWindow::on_pacPortSB_valueChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.pacConfig.port = arg1;
    pacListenAddrLabel->setText("http://" + (pacProxyTxt->text().isEmpty() ? "127.0.0.1" : pacProxyTxt->text()) + ":" + QSTRN(pacPortSB->value()) + "/pac");
}

void PreferencesWindow::on_setSysProxyCB_stateChanged(int arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.setSystemProxy = arg1 == Qt::Checked;
}

void PreferencesWindow::on_pacProxyCB_currentIndexChanged(int index)
{
    LOADINGCHECK
    NEEDRESTART
    // 0 -> http
    // 1 -> socks
    CurrentConfig.inboundConfig.pacConfig.useSocksProxy = index == 1;
}

void PreferencesWindow::on_pushButton_clicked()
{
    LOADINGCHECK
    QDesktopServices::openUrl(QUrl::fromUserInput(QV2RAY_RULES_DIR));
}

void PreferencesWindow::on_pacProxyTxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.pacConfig.localIP = arg1;
}

void PreferencesWindow::on_autoStartSubsCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
    //CurrentConfig.autoStartConfig.subscriptionName = arg1;
    //autoStartConnCombo->clear();
    //
    //if (arg1.isEmpty()) {
    //    autoStartConnCombo->addItem("");
    //    autoStartConnCombo->addItems(CurrentConfig.configs);
    //} else {
    //    auto list = GetSubscriptionConnection(arg1);
    //    autoStartConnCombo->addItems(list.keys());
    //}
}

void PreferencesWindow::on_autoStartConnCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
    //CurrentConfig.autoStartConfig.connectionName = arg1;
}

void PreferencesWindow::on_startWithLoginCB_stateChanged(int arg1)
{
    bool isEnabled = arg1 == Qt::Checked;
    SetLaunchAtLoginStatus(isEnabled);

    if (GetLaunchAtLoginStatus() != isEnabled) {
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

    if (IsValidIPAddress(arg1)) {
        BLACK(fpAddressTx)
    } else {
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

void PreferencesWindow::on_pacProxyTxt_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    if (IsValidIPAddress(arg1)) {
        BLACK(pacProxyTxt)
    } else {
        RED(pacProxyTxt)
    }

    pacListenAddrLabel->setText("http://" + (pacProxyTxt->text().isEmpty() ? "127.0.0.1" : pacProxyTxt->text()) + ":" + QSTRN(pacPortSB->value()) + "/pac");
}

void PreferencesWindow::on_checkVCoreSettings_clicked()
{
    auto vcorePath = vCorePathTxt->text();
    auto vAssetsPath = vCoreAssetsPathTxt->text();
    QString result;

    if (!V2rayKernelInstance::ValidateKernel(vcorePath, vAssetsPath, &result)) {
        QvMessageBoxWarn(this, tr("V2ray Core Settings"), result);
    } else {
        QvMessageBoxInfo(this, tr("V2ray Core Settings"), tr("V2ray path configuration check passed.") + NEWLINE + NEWLINE +
                         tr("Current version of V2ray is: ") + NEWLINE + result);
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

void PreferencesWindow::on_pacGroupBox_clicked(bool checked)
{
    LOADINGCHECK
    NEEDRESTART
    CurrentConfig.inboundConfig.pacConfig.enablePAC = checked;
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
    CurrentConfig.apiConfig.enableAPI = arg1 == Qt::Checked;
}
