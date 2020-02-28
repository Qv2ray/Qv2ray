#include <QDebug>
#include "w_ImportConfig.hpp"

#include "3rdparty/qzxing/src/QZXing.h"
#include "core/CoreUtils.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/connection/Serialization.hpp"
#include "core/kernel/KernelInteractions.hpp"
#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_OutboundEditor.hpp"
#include "ui/editors/w_RoutesEditor.hpp"
#include "ui/w_SubscriptionManager.hpp"
#include "w_ScreenShot_Core.hpp"

#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

ImportConfigWindow::ImportConfigWindow(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    nameTxt->setText(QDateTime::currentDateTime().toString("MMdd_hhmm"));
    QvMessageBusConnect(ImportConfigWindow);
    RESTORE_RUNTIME_CONFIG(screenShotHideQv2ray, hideQv2rayCB->setChecked)
}

QvMessageBusSlotImpl(ImportConfigWindow)
{
    switch (msg)
    {
        MBShowDefaultImpl MBHideDefaultImpl MBRetranslateDefaultImpl
    }
}

ImportConfigWindow::~ImportConfigWindow()
{
}

QMap<QString, CONFIGROOT> ImportConfigWindow::OpenImport(bool partialImport)
{
    // partial import means only import as an outbound, will set keepImported to
    // false and disable the checkbox
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
    bool hideQv2ray = hideQv2rayCB->isChecked();

    if (hideQv2ray)
    {
        messageBus.EmitGlobalSignal(QvMBMessage::HIDE_WINDOWS);
    }

    QApplication::processEvents();
    QThread::msleep(static_cast<ulong>(doubleSpinBox->value() * 1000));
    ScreenShotWindow w;
    auto pix = w.DoScreenShot();
    auto _r = w.result();
    // Explicitly delete w to call UNREGISTER_WINDOW

    if (hideQv2ray)
    {
        messageBus.EmitGlobalSignal(QvMBMessage::SHOW_WINDOWS);
        // ShowAllGlobalWindow();
    }

    if (_r == QDialog::Accepted)
    {
        QZXing decoder;
        decoder.setDecoder(QZXing::DecoderFormat_QR_CODE | QZXing::DecoderFormat_EAN_13);
        auto str = decoder.decodeImage(pix);
        // auto str = QZXing().decodeImage(pix);

        if (str.trimmed().isEmpty())
        {
            LOG(MODULE_UI, "Cannot decode QR Code from an image, size: h=" + QSTRN(pix.width()) + ", v=" + QSTRN(pix.height()))
            QvMessageBoxWarn(this, tr("Capture QRCode"), tr("Cannot find a valid QRCode from this region."));
        }
        else
        {
            vmessConnectionStringTxt->appendPlainText(str.trimmed() + NEWLINE);
            // QvMessageBoxWarn(this, tr("Capture QRCode"), tr("Successfully
            // imported a QR code form the screen.")); this->show();
        }
    }
}

void ImportConfigWindow::on_beginImportBtn_clicked()
{
    QString aliasPrefix = nameTxt->text();
    // auto conf = GetGlobalConfig();

    switch (tabWidget->currentIndex())
    {
        case 0:
        {
            //// From File...
            // bool ImportAsComplex = keepImportedInboundCheckBox->isChecked();
            // QString path = fileLineTxt->text();
            //
            // if (!V2rayKernelInstance::ValidateConfig(path)) {
            //    QvMessageBoxWarn(this, tr("Import config file"), tr("Failed to
            //    check the validity of the config file.")); return;
            //}
            //
            // aliasPrefix += "_" + QFileInfo(path).fileName();
            ////CONFIGROOT config = ConvertConfigFromFile(path,
            /// ImportAsComplex);
            // connections[aliasPrefix] = config;
            // break;
        }

        case 1:
        {
            QStringList linkList = SplitLines(vmessConnectionStringTxt->toPlainText());
            //
            // Clear UI and error lists
            linkErrors.clear();
            vmessConnectionStringTxt->clear();
            errorsList->clear();
            //
            LOG(MODULE_IMPORT, QSTRN(linkList.count()) + " string found in vmess box.")

            while (!linkList.isEmpty())
            {
                aliasPrefix = nameTxt->text();
                auto link = linkList.takeFirst();
                QString errMessage;
                CONFIGROOT config = ConvertConfigFromString(link, &aliasPrefix, &errMessage);

                // If the config is empty or we have any err messages.
                if (config.isEmpty() || !errMessage.isEmpty())
                {
                    // To prevent duplicated values.
                    linkErrors[link] = QSTRN(linkErrors.count() + 1) + ": " + errMessage;
                    continue;
                }
                else
                {
                    connections[aliasPrefix] = config;
                }
            }

            if (!linkErrors.isEmpty())
            {
                for (auto item : linkErrors)
                {
                    vmessConnectionStringTxt->appendPlainText(linkErrors.key(item));
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
    QZXing decoder;
    decoder.setDecoder(QZXing::DecoderFormat_QR_CODE | QZXing::DecoderFormat_EAN_13);
    auto str = decoder.decodeImage(QImage::fromData(buf));

    if (str.isEmpty())
    {
        QvMessageBoxWarn(this, tr("QRCode scanning failed"), tr("Cannot find any QRCode from the image."));
        return;
    }
    else
    {
        vmessConnectionStringTxt->appendPlainText(str.trimmed() + NEWLINE);
    }
}
void ImportConfigWindow::on_errorsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    if (current == nullptr)
    {
        return;
    }

    auto currentErrorText = current->text();
    auto vmessEntry = linkErrors.key(currentErrorText);
    //
    auto startPos = vmessConnectionStringTxt->toPlainText().indexOf(vmessEntry);
    auto endPos = startPos + vmessEntry.length();

    if (startPos < 0)
    {
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

    if (!file.exists())
    {
        QvMessageBoxWarn(this, tr("Edit file as JSON"), tr("Provided file not found: ") + fileLineTxt->text());
        return;
    }

    auto jsonString = StringFromFile(&file);
    auto jsonCheckingError = VerifyJsonString(jsonString);

    if (!jsonCheckingError.isEmpty())
    {
        LOG(MODULE_FILEIO, "Currupted JSON file detected")

        if (QvMessageBoxAsk(
                this, tr("Edit file as JSON"),
                tr("The file you selected has json syntax error. Continue editing may make you lose data. Would you like to continue?") +
                    NEWLINE + jsonCheckingError) != QMessageBox::Yes)
        {
            return;
        }
        else
        {
            LOG(MODULE_FILEIO, "Continue editing curruped json file, data loss is expected.")
        }
    }

    auto json = JsonFromString(jsonString);
    JsonEditor editor(json, this);
    json = editor.OpenEditor();

    if (editor.result() == QDialog::Accepted)
    {
        auto str = JsonToString(json);
        bool result = StringToFile(str, file);

        if (!result)
        {
            QvMessageBoxWarn(this, tr("Edit file as JSON"), tr("Failed to save file, please check if you have proper permissions"));
        }
    }
    else
    {
        LOG(MODULE_FILEIO, "Canceled saving a file.")
    }
}

void ImportConfigWindow::on_connectionEditBtn_clicked()
{
    OutboundEditor w(this);
    auto outboundEntry = w.OpenEditor();
    bool isChanged = w.result() == QDialog::Accepted;
    QString alias = w.GetFriendlyName();

    if (isChanged)
    {
        OUTBOUNDS outboundsList;
        outboundsList.push_back(outboundEntry);
        CONFIGROOT root;
        root.insert("outbounds", outboundsList);
        //
        connections[alias] = root;
        accept();
    }
    else
    {
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

    if (importToComplex)
    {
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

    if (isChanged)
    {
        connections[alias] = result;
        accept();
    }
    else
    {
        return;
    }
}

void ImportConfigWindow::on_hideQv2rayCB_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    SET_RUNTIME_CONFIG(screenShotHideQv2ray, hideQv2rayCB->isChecked)
}
