#include "QvUtils.h"
#include "QvCoreInteractions.h"
#include "w_PrefrencesWindow.h"
#include <QFileDialog>

#ifndef _WIN32
#include <unistd.h>
#endif

PrefrencesWindow::PrefrencesWindow(QWidget *parent) : QDialog(parent),
    CurrentConfig(),
    ui(new Ui::PrefrencesWindow)
{
    ui->setupUi(this);
    CurrentConfig = GetGlobalConfig();
    //
    ui->languageComboBox->setCurrentText(QString::fromStdString(CurrentConfig.language));
    ui->runAsRootCheckBox->setChecked(CurrentConfig.runAsRoot);
    ui->logLevelComboBox->setCurrentIndex(CurrentConfig.logLevel);
    //
    //
    ui->httpCB->setChecked(CurrentConfig.inBoundSettings.http_port != 0);
    ui->httpPortLE->setText(QString::fromStdString(to_string(CurrentConfig.inBoundSettings.http_port)));
    ui->httpAuthCB->setChecked(CurrentConfig.inBoundSettings.http_useAuth);
    //
    ui->httpAuthUsernameTxt->setEnabled(CurrentConfig.inBoundSettings.http_useAuth);
    ui->httpAuthPasswordTxt->setEnabled(CurrentConfig.inBoundSettings.http_useAuth);
    ui->httpAuthUsernameTxt->setText(QString::fromStdString(CurrentConfig.inBoundSettings.httpAccount.user));
    ui->httpAuthPasswordTxt->setText(QString::fromStdString(CurrentConfig.inBoundSettings.httpAccount.pass));
    ui->httpPortLE->setValidator(new QIntValidator());
    //
    //
    ui->socksCB->setChecked(CurrentConfig.inBoundSettings.socks_port != 0);
    ui->socksPortLE->setText(QString::fromStdString(to_string(CurrentConfig.inBoundSettings.socks_port)));
    ui->socksAuthCB->setChecked(CurrentConfig.inBoundSettings.socks_useAuth);
    //
    ui->socksAuthUsernameTxt->setEnabled(CurrentConfig.inBoundSettings.socks_useAuth);
    ui->socksAuthPasswordTxt->setEnabled(CurrentConfig.inBoundSettings.socks_useAuth);
    ui->socksAuthUsernameTxt->setText(QString::fromStdString(CurrentConfig.inBoundSettings.socksAccount.user));
    ui->socksAuthPasswordTxt->setText(QString::fromStdString(CurrentConfig.inBoundSettings.socksAccount.pass));
    ui->socksPortLE->setValidator(new QIntValidator());
    //
    //
    ui->vCoreExePathTxt->setText(QString::fromStdString(CurrentConfig.v2CorePath));
    ui->vCoreAssetsPathTxt->setText(QString::fromStdString(CurrentConfig.v2AssetsPath));
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
        ui->DNSListTxt->appendPlainText(QString::fromStdString(dnsStr) + "\r\n");
    }

    finishedLoading = true;
}

PrefrencesWindow::~PrefrencesWindow()
{
    delete ui;
}

void PrefrencesWindow::on_buttonBox_accepted()
{
    if (ui->httpPortLE->text().toInt() == ui->socksPortLE->text().toInt()) {
        QvMessageBox(this, tr("Prefrences"), tr("PortNumbersCannotBeSame"));
        return;
    }

    SetGlobalConfig(CurrentConfig);
    emit s_reload_config();
}

void PrefrencesWindow::on_httpCB_stateChanged(int checked)
{
    ui->httpPortLE->setEnabled(checked == Qt::Checked);
    ui->httpAuthCB->setEnabled(checked == Qt::Checked);
    ui->httpAuthUsernameTxt->setEnabled(checked == Qt::Checked);
    ui->httpAuthPasswordTxt->setEnabled(checked == Qt::Checked);
    CurrentConfig.inBoundSettings.http_port = checked == Qt::Checked ? CurrentConfig.inBoundSettings.http_port : 0;

    if (checked != Qt::Checked) {
        ui->httpPortLE->setText("0");
    }
}

void PrefrencesWindow::on_socksCB_stateChanged(int checked)
{
    ui->socksPortLE->setEnabled(checked == Qt::Checked);
    ui->socksAuthCB->setEnabled(checked == Qt::Checked);
    ui->socksAuthUsernameTxt->setEnabled(checked == Qt::Checked);
    ui->socksAuthPasswordTxt->setEnabled(checked == Qt::Checked);
    CurrentConfig.inBoundSettings.socks_port = checked == Qt::Checked ? CurrentConfig.inBoundSettings.socks_port : 0;

    if (checked != Qt::Checked) {
        ui->socksPortLE->setText("0");
    }
}

void PrefrencesWindow::on_httpAuthCB_stateChanged(int checked)
{
    ui->httpAuthUsernameTxt->setEnabled(checked == Qt::Checked);
    ui->httpAuthPasswordTxt->setEnabled(checked == Qt::Checked);
    CurrentConfig.inBoundSettings.http_useAuth = checked == Qt::Checked;
}

void PrefrencesWindow::on_runAsRootCheckBox_stateChanged(int arg1)
{
#ifdef __linux
    // Set UID and GID in *nix
    // The file is actually not here
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

#else
    // No such uid gid thing on Windows and MacOS is in TODO ....
    QvMessageBox(this, tr("Prefrences"), tr("RunAsRootNotOnWindows"));
#endif
}

void PrefrencesWindow::on_socksAuthCB_stateChanged(int checked)
{
    ui->socksAuthUsernameTxt->setEnabled(checked == Qt::Checked);
    ui->socksAuthPasswordTxt->setEnabled(checked == Qt::Checked);
    CurrentConfig.inBoundSettings.socks_useAuth = checked == Qt::Checked;
}

void PrefrencesWindow::on_languageComboBox_currentTextChanged(const QString &arg1)
{
    CurrentConfig.language = arg1.toStdString();
}

void PrefrencesWindow::on_logLevelComboBox_currentIndexChanged(int index)
{
    CurrentConfig.logLevel = index;
}

void PrefrencesWindow::on_vCoreExePathTxt_textEdited(const QString &arg1)
{
    CurrentConfig.v2CorePath = arg1.toStdString();
}

void PrefrencesWindow::on_vCoreAssetsPathTxt_textEdited(const QString &arg1)
{
    CurrentConfig.v2AssetsPath = arg1.toStdString();
}

void PrefrencesWindow::on_muxEnabledCB_stateChanged(int arg1)
{
    CurrentConfig.mux.enabled = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_muxConcurrencyTxt_valueChanged(int arg1)
{
    CurrentConfig.mux.concurrency = arg1;
}

void PrefrencesWindow::on_listenIPTxt_textEdited(const QString &arg1)
{
    CurrentConfig.inBoundSettings.listenip = arg1.toStdString();
}

void PrefrencesWindow::on_socksPortLE_textEdited(const QString &arg1)
{
    CurrentConfig.inBoundSettings.socks_port = stoi(arg1.toStdString());
}

void PrefrencesWindow::on_httpPortLE_textEdited(const QString &arg1)
{
    CurrentConfig.inBoundSettings.http_port = stoi(arg1.toStdString());
}

void PrefrencesWindow::on_httpAuthUsernameTxt_textEdited(const QString &arg1)
{
    CurrentConfig.inBoundSettings.httpAccount.user = arg1.toStdString();
}

void PrefrencesWindow::on_httpAuthPasswordTxt_textEdited(const QString &arg1)
{
    CurrentConfig.inBoundSettings.httpAccount.pass = arg1.toStdString();
}

void PrefrencesWindow::on_socksAuthUsernameTxt_textEdited(const QString &arg1)
{
    CurrentConfig.inBoundSettings.socksAccount.user = arg1.toStdString();
}

void PrefrencesWindow::on_socksAuthPasswordTxt_textEdited(const QString &arg1)
{
    CurrentConfig.inBoundSettings.socksAccount.pass = arg1.toStdString();
}

void PrefrencesWindow::on_proxyCNCb_stateChanged(int arg1)
{
    CurrentConfig.proxyCN = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_proxyDefaultCb_stateChanged(int arg1)
{
    CurrentConfig.proxyDefault = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_localDNSCb_stateChanged(int arg1)
{
    CurrentConfig.withLocalDNS = arg1 == Qt::Checked;
}

void PrefrencesWindow::on_selectVCoreBtn_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("#OpenVCoreFile"), QDir::homePath());
    ui->vCoreExePathTxt->setText(dir);
    on_vCoreExePathTxt_textEdited(dir);
}

void PrefrencesWindow::on_selectVAssetBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("OpenVAssetsDir"), QDir::homePath());
    ui->vCoreAssetsPathTxt->setText(dir);
    on_vCoreAssetsPathTxt_textChanged(dir);
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
                }
            }

            WHITE(DNSListTxt)
        } catch (...) {
            RED(DNSListTxt)
        }
    }
}
void PrefrencesWindow::on_vCoreAssetsPathTxt_textChanged(const QString &arg1)
{
    CurrentConfig.v2AssetsPath = arg1.toStdString();
}
