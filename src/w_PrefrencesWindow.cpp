#include "QvUtils.h"
#include "QvCoreInteractions.h"
#include "w_PrefrencesWindow.h"
#include <QFileDialog>

#include <iostream>

#ifdef __linux
#include <unistd.h>
#endif

#define NEEDRESTART if(finishedLoading) IsConnectionPropertyChanged = true;

PrefrencesWindow::PrefrencesWindow(QWidget *parent) : QDialog(parent),
    CurrentConfig(),
    ui(new Ui::PrefrencesWindow)
{
    ui->setupUi(this);
    // We add locales
    ui->languageComboBox->clear();
    QDirIterator it(":/translations");

    while (it.hasNext()) {
        ui->languageComboBox->addItem(it.next().split("/").last().split(".").first());
    }

    //
    ui->qvVersion->setText(QV2RAY_VERSION_STRING);
    CurrentConfig = GetGlobalConfig();
    //
    ui->languageComboBox->setCurrentText(QSTRING(CurrentConfig.language));
    ui->logLevelComboBox->setCurrentIndex(CurrentConfig.logLevel);
    ui->tProxyCheckBox->setChecked(CurrentConfig.runAsRoot);
    //
    //
    ui->listenIPTxt->setText(QSTRING(CurrentConfig.inBoundSettings.listenip));
    //
    bool have_http = CurrentConfig.inBoundSettings.http_port != 0;
    ui->httpCB->setChecked(have_http);
    ui->httpPortLE->setText(QSTRING(to_string(CurrentConfig.inBoundSettings.http_port)));
    ui->httpAuthCB->setChecked(CurrentConfig.inBoundSettings.http_useAuth);
    //
    ui->httpAuthCB->setEnabled(have_http);
    ui->httpAuthCB->setChecked(CurrentConfig.inBoundSettings.http_useAuth);
    ui->httpAuthUsernameTxt->setEnabled(have_http && CurrentConfig.inBoundSettings.http_useAuth);
    ui->httpAuthPasswordTxt->setEnabled(have_http && CurrentConfig.inBoundSettings.http_useAuth);
    ui->httpAuthUsernameTxt->setText(QSTRING(CurrentConfig.inBoundSettings.httpAccount.user));
    ui->httpAuthPasswordTxt->setText(QSTRING(CurrentConfig.inBoundSettings.httpAccount.pass));
    ui->httpPortLE->setValidator(new QIntValidator());
    //
    //
    bool have_socks = CurrentConfig.inBoundSettings.socks_port != 0;
    ui->socksCB->setChecked(have_socks);
    ui->socksPortLE->setText(QSTRING(to_string(CurrentConfig.inBoundSettings.socks_port)));
    ui->socksAuthCB->setChecked(CurrentConfig.inBoundSettings.socks_useAuth);
    //
    ui->socksAuthCB->setEnabled(have_socks);
    ui->socksAuthCB->setChecked(CurrentConfig.inBoundSettings.socks_useAuth);
    ui->socksAuthUsernameTxt->setEnabled(have_socks && CurrentConfig.inBoundSettings.socks_useAuth);
    ui->socksAuthPasswordTxt->setEnabled(have_socks && CurrentConfig.inBoundSettings.socks_useAuth);
    ui->socksAuthUsernameTxt->setText(QSTRING(CurrentConfig.inBoundSettings.socksAccount.user));
    ui->socksAuthPasswordTxt->setText(QSTRING(CurrentConfig.inBoundSettings.socksAccount.pass));
    ui->socksPortLE->setValidator(new QIntValidator());
    //
    //
    ui->vCoreExePathTxt->setText(QSTRING(CurrentConfig.v2CorePath));
    ui->vCoreAssetsPathTxt->setText(QSTRING(CurrentConfig.v2AssetsPath));
    //
    //
    ui->muxEnabledCB->setChecked(CurrentConfig.mux.enabled);
    ui->muxConcurrencyTxt->setValue(CurrentConfig.mux.concurrency);
    //
    ui->proxyCNCb->setChecked(CurrentConfig.proxyCN);
    ui->proxyDefaultCb->setChecked(CurrentConfig.proxyDefault);
    ui->localDNSCb->setChecked(CurrentConfig.withLocalDNS);
    //
    ui->DNSListTxt->clear();

    foreach (auto dnsStr, CurrentConfig.dnsList) {
        auto str = QString::fromStdString(dnsStr).trimmed();

        if (!str.isEmpty()) {
            ui->DNSListTxt->appendPlainText(str);
        }
    }

    foreach (auto connection, CurrentConfig.configs) {
        ui->autoStartCombo->addItem(QSTRING(connection));
    }

    ui->autoStartCombo->setCurrentText(QSTRING(CurrentConfig.autoStartConfig));
    ui->cancelIgnoreVersionBtn->setEnabled(CurrentConfig.ignoredVersion != "");
    ui->ignoredNextVersion->setText(QSTRING(CurrentConfig.ignoredVersion));
    finishedLoading = true;
}

PrefrencesWindow::~PrefrencesWindow()
{
    delete ui;
}

void PrefrencesWindow::on_buttonBox_accepted()
{
    int sp = ui->socksPortLE->text().toInt();
    int hp = ui->httpPortLE->text().toInt() ;

    if (!(sp == 0 || hp == 0) && sp == hp) {
        QvMessageBox(this, tr("Prefrences"), tr("Port numbers cannot be the same"));
        return;
    }

    SetGlobalConfig(CurrentConfig);
    emit s_reload_config(IsConnectionPropertyChanged);
}

void PrefrencesWindow::on_httpCB_stateChanged(int checked)
{
    NEEDRESTART
    ui->httpPortLE->setEnabled(checked == Qt::Checked);
    ui->httpAuthCB->setEnabled(checked == Qt::Checked);
    ui->httpAuthUsernameTxt->setEnabled(checked == Qt::Checked && ui->httpAuthCB->isChecked());
    ui->httpAuthPasswordTxt->setEnabled(checked == Qt::Checked && ui->httpAuthCB->isChecked());
    CurrentConfig.inBoundSettings.http_port = checked == Qt::Checked ? CurrentConfig.inBoundSettings.http_port : 0;

    if (checked != Qt::Checked) {
        ui->httpPortLE->setText("0");
    }
}

void PrefrencesWindow::on_socksCB_stateChanged(int checked)
{
    NEEDRESTART
    ui->socksPortLE->setEnabled(checked == Qt::Checked);
    ui->socksAuthCB->setEnabled(checked == Qt::Checked);
    ui->socksAuthUsernameTxt->setEnabled(checked == Qt::Checked && ui->socksAuthCB->isChecked());
    ui->socksAuthPasswordTxt->setEnabled(checked == Qt::Checked && ui->socksAuthCB->isChecked());
    CurrentConfig.inBoundSettings.socks_port = checked == Qt::Checked ? CurrentConfig.inBoundSettings.socks_port : 0;

    if (checked != Qt::Checked) {
        ui->socksPortLE->setText("0");
    }
}

void PrefrencesWindow::on_httpAuthCB_stateChanged(int checked)
{
    NEEDRESTART
    ui->httpAuthUsernameTxt->setEnabled(checked == Qt::Checked);
    ui->httpAuthPasswordTxt->setEnabled(checked == Qt::Checked);
    CurrentConfig.inBoundSettings.http_useAuth = checked == Qt::Checked;
}

void PrefrencesWindow::on_socksAuthCB_stateChanged(int checked)
{
    NEEDRESTART
    ui->socksAuthUsernameTxt->setEnabled(checked == Qt::Checked);
    ui->socksAuthPasswordTxt->setEnabled(checked == Qt::Checked);
    CurrentConfig.inBoundSettings.socks_useAuth = checked == Qt::Checked;
}

void PrefrencesWindow::on_languageComboBox_currentTextChanged(const QString &arg1)
{
    CurrentConfig.language = arg1.toStdString();
    //
    // A strange bug prevents us to change the UI language online
    //    https://github.com/lhy0403/Qv2ray/issues/34
    //
    //if (QApplication::installTranslator(getTranslator(&arg1))) {
    //    LOG(MODULE_UI, "Loaded translations " + arg1.toStdString())
    //    ui->retranslateUi(this);
    //} else {
    //    QvMessageBox(this, tr("#Prefrences"), tr("#SwitchTranslationError"));
    //}
}

void PrefrencesWindow::on_logLevelComboBox_currentIndexChanged(int index)
{
    NEEDRESTART
    CurrentConfig.logLevel = index;
}

void PrefrencesWindow::on_vCoreExePathTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.v2CorePath = arg1.toStdString();
}

void PrefrencesWindow::on_vCoreAssetsPathTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.v2AssetsPath = arg1.toStdString();
}

void PrefrencesWindow::on_muxEnabledCB_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.mux.enabled = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_muxConcurrencyTxt_valueChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.mux.concurrency = arg1;
}

void PrefrencesWindow::on_listenIPTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.listenip = arg1.toStdString();
}

void PrefrencesWindow::on_socksPortLE_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.socks_port = stoi(arg1.toStdString());
}

void PrefrencesWindow::on_httpPortLE_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.http_port = stoi(arg1.toStdString());
}

void PrefrencesWindow::on_httpAuthUsernameTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.httpAccount.user = arg1.toStdString();
}

void PrefrencesWindow::on_httpAuthPasswordTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.httpAccount.pass = arg1.toStdString();
}

void PrefrencesWindow::on_socksAuthUsernameTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.socksAccount.user = arg1.toStdString();
}

void PrefrencesWindow::on_socksAuthPasswordTxt_textEdited(const QString &arg1)
{
    NEEDRESTART
    CurrentConfig.inBoundSettings.socksAccount.pass = arg1.toStdString();
}

void PrefrencesWindow::on_proxyCNCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.proxyCN = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_proxyDefaultCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.proxyDefault = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_localDNSCb_stateChanged(int arg1)
{
    NEEDRESTART
    CurrentConfig.withLocalDNS = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_selectVCoreBtn_clicked()
{
    NEEDRESTART
    QString path = QFileDialog::getOpenFileName(this, tr("Open v2ray core file"), QDir::currentPath());
    ui->vCoreExePathTxt->setText(path);
    on_vCoreExePathTxt_textEdited(path);

    // If we enabled tProxy feature... then not to change this automatically
    if (CurrentConfig.runAsRoot) {
        LOG(MODULE_CONFIG, "Not to automatically update v2ray assets path, because tProxy feature is enabled.")
    } else {
        auto dir = QFileInfo(path).dir().path();
        ui->vCoreAssetsPathTxt->setText(dir);
        on_vCoreAssetsPathTxt_textEdited(dir);
    }
}

void PrefrencesWindow::on_selectVAssetBtn_clicked()
{
    NEEDRESTART
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open v2ray assets folder"), QDir::currentPath());
    ui->vCoreAssetsPathTxt->setText(dir);
    on_vCoreAssetsPathTxt_textEdited(dir);
}

void PrefrencesWindow::on_DNSListTxt_textChanged()
{
    if (finishedLoading) {
        try {
            QStringList hosts = ui->DNSListTxt->toPlainText().replace("\r", "").split("\n");
            CurrentConfig.dnsList.clear();

            foreach (auto host, hosts) {
                if (host != "" && host != "\r") {
                    // Not empty, so we save.
                    CurrentConfig.dnsList.push_back(host.toStdString());
                    NEEDRESTART
                }
            }

            BLACK(DNSListTxt)
        } catch (...) {
            RED(DNSListTxt)
        }
    }
}

void PrefrencesWindow::on_autoStartCombo_currentTextChanged(const QString &arg1)
{
    CurrentConfig.autoStartConfig = arg1.toStdString();
}

void PrefrencesWindow::on_aboutQt_clicked()
{
    QApplication::aboutQt();
}

void PrefrencesWindow::on_cancelIgnoreVersionBtn_clicked()
{
    CurrentConfig.ignoredVersion.clear();
    ui->cancelIgnoreVersionBtn->setEnabled(false);
}

void PrefrencesWindow::on_tProxyCheckBox_stateChanged(int arg1)
{
#ifdef __linux
    LOG(MODULE_UI, "WARN: This feature is on development.")
    // Set UID and GID for linux
    // Steps:
    // --> 1. Copy v2ray core files to the #CONFIG_DIR#/vcore/ dir.
    // --> 2. Change GlobalConfig.v2CorePath.
    // --> 3. Call `pkexec setcap SOMECAP` on the v2ray core.
    QString vCorePath = QString::fromStdString(CurrentConfig.v2CorePath);
    QFileInfo v2rayCoreExeFile(vCorePath);

    if (arg1 == Qt::Checked && v2rayCoreExeFile.ownerId() != 0) {
        QProcess::execute("pkexec", QStringList() << "bash"
                          << "-c"
                          << "chown root:root " + vCorePath + " && "
                          << "chmod +s " + vCorePath);
        CurrentConfig.runAsRoot = true;
    } else if (arg1 != Qt::Checked && v2rayCoreExeFile.ownerId() == 0) {
        uid_t uid = getuid();
        gid_t gid = getgid();
        QProcess::execute("pkexec", QStringList()
                          << "chown" << QString::number(uid) + ":" + QString::number(gid)
                          << vCorePath);
        CurrentConfig.runAsRoot = false;
    }

    NEEDRESTART
#else
    Q_UNUSED(arg1)
    ui->tProxyCheckBox->setChecked(false);
    // No such uid gid thing on Windows and MacOS is in TODO ....
    QvMessageBox(this, tr("tProxy"), tr("tProxy is not supported on MacOS and Windows"));
#endif
}
