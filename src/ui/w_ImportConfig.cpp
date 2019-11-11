#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include "QZXing"

#include "QvUtils.hpp"
#include "QvCoreInteractions.hpp"
#include "QvCoreConfigOperations.hpp"

#include "w_OutboundEditor.hpp"
#include "w_ImportConfig.hpp"


ImportConfigWindow::ImportConfigWindow(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    nameTxt->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-MM-ss") + "_.imported");
}

void ImportConfigWindow::on_importSourceCombo_currentIndexChanged(int index)
{
    stackedWidget->setCurrentIndex(index);
}

void ImportConfigWindow::on_selectFileBtn_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Select file to import"));
    fileLineTxt->setText(dir);
}

void ImportConfigWindow::on_qrFromScreenBtn_clicked()
{
    LOG(MODULE_UI, "We currently only support the primary screen.")
    QThread::msleep(static_cast<ulong>(doubleSpinBox->value() * 1000));
    QScreen *screen = qApp->primaryScreen();

    if (const QWindow *window = windowHandle()) {
        screen = window->screen();
    }

    if (!screen) {
        LOG(MODULE_UI, "Cannot even find a screen.")
        QvMessageBox(this, tr("Screenshot failed"), tr("Cannot find a valid screen, it's rare."));
        return;
    }

    auto pix = screen->grabWindow(0);
    auto str = QZXing().decodeImage(pix.toImage());

    if (str.isEmpty()) {
        QvMessageBox(this, tr("QRCode scanning failed"), tr("Cannot find any QRCode from the primary screen."));
        return;
    } else {
        vmessConnectionStringTxt->appendPlainText(str.trimmed() + NEWLINE);
    }
}

void ImportConfigWindow::on_beginImportBtn_clicked()
{
    QString aliasPrefix = nameTxt->text();
    QJsonObject config;
    auto conf = GetGlobalConfig();

    switch (importSourceCombo->currentIndex()) {
        case 0: {
            // From File...
            bool overrideInBound = !keepImportedInboundCheckBox->isChecked();
            QString path = fileLineTxt->text();
            aliasPrefix = aliasPrefix.isEmpty() ? aliasPrefix : QFileInfo(path).fileName();
            config = ConvertConfigFromFile(path, overrideInBound);

            if (config.isEmpty()) {
                QvMessageBox(this, tr("Import config file"), tr("Import from file failed, for more information, please check the log file."));
                return;
            } else if (!Qv2Instance::ValidateConfig(&path)) {
                QvMessageBox(this, tr("Import config file"), tr("Failed to check the validity of the config file."));
                return;
            } else {
                // We save first, "alias" may change to prevent override existing file.
                bool alwaysFalse = SaveConnectionConfig(config, &aliasPrefix, false);

                if (alwaysFalse) {
                    QvMessageBox(this, tr("Assertion Failed"), tr("Assertion failed: ::SaveConnectionConfig should returns false."));
                }

                conf.configs.push_back(aliasPrefix.toStdString());
                break;
            }
        }

        case 1: {
            QStringList vmessList = SplitLines(vmessConnectionStringTxt->toPlainText());
            //
            // Clear UI and error lists
            vmessErrors.clear();
            vmessConnectionStringTxt->clear();
            errorsList->clear();
            //
            LOG(MODULE_IMPORT, to_string(vmessList.count()) + " vmess connection found.")

            while (!vmessList.isEmpty()) {
                auto vmess = vmessList.takeFirst();
                QString errMessage;
                config = ConvertConfigFromVMessString(vmess, &aliasPrefix, &errMessage);

                // If the config is empty or we have any err messages.
                if (config.isEmpty() || !errMessage.isEmpty()) {
                    // To prevent duplicated values.
                    vmessErrors[vmess] = QString::number(vmessErrors.count() + 1) + ": " + errMessage;
                    continue;
                } else {
                    bool alwaysFalse = SaveConnectionConfig(config, &aliasPrefix, false);

                    if (alwaysFalse) {
                        QvMessageBox(this, tr("Assertion Failed"), "Assertion failed: ::SaveConnectionConfig should returns false.");
                    }

                    conf.configs.push_back(aliasPrefix.toStdString());
                }
            }

            if (!vmessErrors.isEmpty()) {
                // TODO Show in UI
                for (auto item : vmessErrors) {
                    vmessConnectionStringTxt->appendPlainText(vmessErrors.key(item));
                    errorsList->addItem(item);
                }

                // Don't quit;
                return;
            }

            break;
        }

        case 2: {
            // Subscription link.
            break;
        }
    }

    SetGlobalConfig(conf);
    // Never restart current connection after import.
    emit s_reload_config(false);
    close();
}

void ImportConfigWindow::on_selectImageBtn_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Select an image to import"));
    imageFileEdit->setText(dir);
}

void ImportConfigWindow::on_errorsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    if (current == nullptr) {
        return;
    }

    auto currentErrorText = current->text();
    auto vmessEntry = vmessErrors.key(currentErrorText);
    //
    auto startPos = vmessConnectionStringTxt->toPlainText().indexOf(vmessEntry);
    auto endPos = startPos + vmessEntry.length();

    if (startPos < 0) {
        return;
    }

    //
    QTextCursor c = vmessConnectionStringTxt->textCursor();
    c.setPosition(startPos);
    c.setPosition(endPos, QTextCursor::KeepAnchor);
    vmessConnectionStringTxt->setTextCursor(c);
}
