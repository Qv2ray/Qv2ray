#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include "qzxing/src/QZXing.h"

#include "QvUtils.hpp"
#include "QvKernelInteractions.hpp"
#include "QvCoreConfigOperations.hpp"

#include "w_ScreenShot_Core.hpp"
#include "w_OutboundEditor.hpp"
#include "w_JsonEditor.hpp"
#include "w_ImportConfig.hpp"
#include "w_SubscriptionEditor.hpp"
#include "w_RoutesEditor.hpp"


ImportConfigWindow::ImportConfigWindow(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    nameTxt->setText(QDateTime::currentDateTime().toString("MMdd_hhmm"));
}

QMap<QString, CONFIGROOT> ImportConfigWindow::OpenImport(bool partialImport)
{
    // partial import means only import as an outbound, will set keepImported to false and disable the checkbox
    // keepImportedInboundCheckBox->setChecked(!outboundsOnly);
    keepImportedInboundCheckBox->setEnabled(!partialImport);
    routeEditBtn->setEnabled(!partialImport);
    this->exec();
    return this->result() == QDialog::Accepted ? connections : QMap<QString, CONFIGROOT>();
}

void ImportConfigWindow::on_selectFileBtn_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Select file to import"));
    fileLineTxt->setText(dir);
}

void ImportConfigWindow::on_qrFromScreenBtn_clicked()
{
    QThread::msleep(static_cast<ulong>(doubleSpinBox->value() * 1000));
    ScreenShotWindow w;
    auto pix = w.DoScreenShot();

    if (w.result() == QDialog::Accepted) {
        auto str = QZXing().decodeImage(pix);

        if (str.trimmed().isEmpty()) {
            LOG(MODULE_UI, "Cannot decode QR Code from an image, size: h=" + QSTRN(pix.width()) + ", v=" + QSTRN(pix.height()))
            QvMessageBoxWarn(this, tr("Capture QRCode"), tr("Cannot find a valid QRCode from this region."));
        } else {
            vmessConnectionStringTxt->appendPlainText(str.trimmed() + NEWLINE);
        }
    }
}

void ImportConfigWindow::on_beginImportBtn_clicked()
{
    QString aliasPrefix = nameTxt->text();
    //auto conf = GetGlobalConfig();

    switch (tabWidget->currentIndex()) {
        case 0: {
            // From File...
            bool keepInBound = keepImportedInboundCheckBox->isChecked();
            QString path = fileLineTxt->text();

            if (!V2rayKernelInstance::ValidateConfig(path)) {
                QvMessageBoxWarn(this, tr("Import config file"), tr("Failed to check the validity of the config file."));
                return;
            }

            aliasPrefix += "_" + QFileInfo(path).fileName();
            CONFIGROOT config = ConvertConfigFromFile(path, keepInBound);
            connections[aliasPrefix] = config;
            break;
        }

        case 1: {
            QStringList vmessList = SplitLines(vmessConnectionStringTxt->toPlainText());
            //
            // Clear UI and error lists
            vmessErrors.clear();
            vmessConnectionStringTxt->clear();
            errorsList->clear();
            //
            LOG(MODULE_IMPORT, QSTRN(vmessList.count()) + " string found in vmess box.")

            while (!vmessList.isEmpty()) {
                aliasPrefix = nameTxt->text();
                auto vmess = vmessList.takeFirst();
                QString errMessage;
                CONFIGROOT config = ConvertConfigFromVMessString(vmess, &aliasPrefix, &errMessage);

                // If the config is empty or we have any err messages.
                if (config.isEmpty() || !errMessage.isEmpty()) {
                    // To prevent duplicated values.
                    vmessErrors[vmess] = QSTRN(vmessErrors.count() + 1) + ": " + errMessage;
                    continue;
                } else {
                    connections[aliasPrefix] = config;
                }
            }

            if (!vmessErrors.isEmpty()) {
                for (auto item : vmessErrors) {
                    vmessConnectionStringTxt->appendPlainText(vmessErrors.key(item));
                    errorsList->addItem(item);
                }

                vmessConnectionStringTxt->setLineWidth(errorsList->lineWidth());
                errorsList->sortItems();
                return;
            }

            break;
        }
    }

    accept();
}
void ImportConfigWindow::on_selectImageBtn_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Select an image to import"));
    imageFileEdit->setText(dir);
    //
    QFile file(dir);
    file.open(QFile::OpenModeFlag::ReadOnly);
    auto buf = file.readAll();
    file.close();
    //
    auto str = QZXing().decodeImage(QImage::fromData(buf));

    if (str.isEmpty()) {
        QvMessageBoxWarn(this, tr("QRCode scanning failed"), tr("Cannot find any QRCode from the image."));
        return;
    } else {
        vmessConnectionStringTxt->appendPlainText(str.trimmed() + NEWLINE);
    }
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

    // Select vmess string that is invalid.
    QTextCursor c = vmessConnectionStringTxt->textCursor();
    c.setPosition(startPos);
    c.setPosition(endPos, QTextCursor::KeepAnchor);
    vmessConnectionStringTxt->setTextCursor(c);
}
void ImportConfigWindow::on_editFileBtn_clicked()
{
    QFile file(fileLineTxt->text());

    if (!file.exists()) {
        QvMessageBoxWarn(this, tr("Edit file as JSON"), tr("Provided file not found: ")  +  fileLineTxt->text());
        return;
    }

    auto jsonString = StringFromFile(&file);
    auto jsonCheckingError = VerifyJsonString(jsonString);

    if (!jsonCheckingError.isEmpty()) {
        LOG(MODULE_FILE, "Currupted JSON file detected")

        if (QvMessageBoxAsk(this, tr("Edit file as JSON"), tr("The file you selected has json syntax error. Continue editing may make you lose data. Would you like to continue?") + NEWLINE + jsonCheckingError) != QMessageBox::Yes) {
            return;
        } else {
            LOG(MODULE_FILE, "Continue editing curruped json file, data loss is expected.")
        }
    }

    auto json  = JsonFromString(jsonString);
    auto editor = new JsonEditor(json, this);
    json = editor->OpenEditor();

    if (editor->result() == QDialog::Accepted) {
        auto str = JsonToString(json);
        bool result = StringToFile(&str, &file);

        if (!result) {
            QvMessageBoxWarn(this, tr("Edit file as JSON"), tr("Failed to save file, please check if you have proper permissions"));
        }
    } else {
        LOG(MODULE_FILE, "Canceled saving a file.")
    }
}

void ImportConfigWindow::on_connectionEditBtn_clicked()
{
    OutboundEditor *w = new OutboundEditor(this);
    auto outboundEntry = w->OpenEditor();
    bool isChanged = w->result() == QDialog::Accepted;
    QString alias = w->GetFriendlyName();
    delete w;

    if (isChanged) {
        OUTBOUNDS outboundsList;
        outboundsList.push_back(outboundEntry);
        CONFIGROOT root;
        root.insert("outbounds", outboundsList);
        //
        connections[alias] = root;
        accept();
    } else {
        return;
    }
}

void ImportConfigWindow::on_cancelImportBtn_clicked()
{
    reject();
}

void ImportConfigWindow::on_subscriptionButton_clicked()
{
    hide();
    SubscribeEditor w;
    w.exec();
    auto importToComplex = !keepImportedInboundCheckBox->isEnabled();
    connections.clear();

    if (importToComplex) {
        auto _result = w.GetSelectedConfig();
        connections[_result.first] = _result.second;
    }

    accept();
}

void ImportConfigWindow::on_routeEditBtn_clicked()
{
    RouteEditor w(QJsonObject(), this);
    auto result = w.OpenEditor();
    bool isChanged = w.result() == QDialog::Accepted;
    QString alias = nameTxt->text();

    if (isChanged) {
        connections[alias] = result;
        accept();
    } else {
        return;
    }
}
