#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "QvUtils.hpp"
#include "QvCoreInteractions.hpp"
#include "QvCoreConfigOperations.hpp"

#include "w_OutboundEditor.hpp"
#include "w_ImportConfig.hpp"

#include "QZXing"

ImportConfigWindow::ImportConfigWindow(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    nameTxt->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-MM-ss.imported"));
}

ImportConfigWindow::~ImportConfigWindow()
{
}

void ImportConfigWindow::OpenImporter()
{
    this->exec();
}

void ImportConfigWindow::on_importSourceCombo_currentIndexChanged(int index)
{
    stackedWidget->setCurrentIndex(index);
}

void ImportConfigWindow::on_selectFileBtn_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Select file to import"), QDir::currentPath());
    fileLineTxt->setText(dir);
}

void ImportConfigWindow::on_buttonBox_accepted()
{
    QString alias = nameTxt->text();
    QJsonObject config;
    auto conf = GetGlobalConfig();
    auto needReload = false;

    if (importSourceCombo->currentIndex() == 0) {
        // From File...
        bool overrideInBound = !keepImportedInboundCheckBox->isChecked();
        auto fileName = fileLineTxt->text();

        if (!Qv2Instance::ValidateConfig(&fileName)) {
            QvMessageBox(this, tr("Import config file"), tr("Failed to check the validity of the config file."));
            return;
        }

        QString path = fileLineTxt->text();
        alias = alias != "" ? alias : QFileInfo(path).fileName();
        config = ConvertConfigFromFile(path, overrideInBound);
        //
        // We save first, "alias" may change to prevent override existing file.
        needReload = SaveConnectionConfig(config, &alias, false);
        conf.configs.push_back(alias.toStdString());
        //
        SetGlobalConfig(conf);
    } else {
        QString vmess = vmessConnectionStringTxt->toPlainText();
        //
        // We saperate the string into lines.
        QStringList vmessList = vmess.split(NEWLINE, QString::SplitBehavior::SkipEmptyParts);
        LOG(MODULE_CONNECTION_IMPORT, to_string(vmessList.count()) + " vmess connection found.")

        foreach (auto vmessString, vmessList) {
            int result = VerifyVMessProtocolString(vmess);

            switch (result) {
                case 0:
                    // This result code passes the validation check.
                    config = ConvertConfigFromVMessString(vmessConnectionStringTxt->toPlainText());
                    //
                    alias = alias.isEmpty() ? alias : config["QV2RAY_ALIAS"].toString();
                    config.remove("QV2RAY_ALIAS");
                    //
                    // Save first.
                    needReload = needReload || SaveConnectionConfig(config, &alias, false);
                    conf.configs.push_back(alias.toStdString());
                    break;

                case -1:
                    QvMessageBox(this, tr("VMess String Check"), tr("VMess string is not valid."));
                    done(0);
                    return;

                default:
                    QvMessageBox(this, tr("VMess String Check"), tr("VMess config is not valid."));
                    return;
            }
        }
    }

    SetGlobalConfig(conf);
    emit s_reload_config(needReload);
}

void ImportConfigWindow::on_qrFromScreenBtn_clicked()
{
    QScreen *screen = qApp->primaryScreen();

    if (const QWindow *window = windowHandle())
        screen = window->screen();

    if (!screen) {
        LOG(MODULE_UI, "Cannot even find a screen.")
        return;
    }

    auto pix = screen->grabWindow(0);
    imageLabel->setPixmap(pix.scaled(QSize(250, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    {
        QZXing x;
        auto str = x.decodeImage(pix.toImage());
        qRContentLabel->setText(str);

        if (str.isEmpty()) {
            QvMessageBox(this, tr("QRCode Scanning failed"), tr("Cannot find a qrcode from current primary screen"));
            return;
        }
    }
}
