#include "w_ExportConfig.hpp"
#include "common/QvHelpers.hpp"
#include "core/connection/Serialization.hpp"
#include <QFileDialog>

// Private initialiser
ConfigExporter::ConfigExporter(QWidget *parent) :
    QDialog(parent),
    qzxing(this)
{
    setupUi(this);
    REGISTER_WINDOW
}

ConfigExporter::~ConfigExporter()
{
    UNREGISTER_WINDOW
}

ConfigExporter::ConfigExporter(const CONFIGROOT &root, const ConnectionIdentifier &alias, QWidget *parent) : ConfigExporter(parent)
{
    message = ConvertConfigToString(root, alias.IdentifierString());
    //
    QZXingEncoderConfig conf;
    conf.border = true;
    conf.imageSize = QSize(400, 400);
    auto img = qzxing.encodeData(message, conf);
    image = img.copy();
}

void ConfigExporter::OpenExport()
{
    imageLabel->setPixmap(QPixmap::fromImage(image));
    messageTxt->setPlainText(message);
    this->exec();
}

void ConfigExporter::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);

    switch (e->type()) {
        case QEvent::LanguageChange:
            retranslateUi(this);
            break;

        case QEvent::Resize:
            imageLabel->setPixmap(QPixmap::fromImage(image));
            break;

        default:
            break;
    }
}

void ConfigExporter::on_closeBtn_clicked()
{
    // OK
    close();
}

void ConfigExporter::on_saveBtn_clicked()
{
    // Save
    auto filePath = QFileDialog().getSaveFileName(this, tr("Save Image"), "", "Images (*.png)");
    auto result = image.save(filePath);
    QDesktopServices::openUrl(QUrl::fromUserInput(filePath));
    LOG(FILEIO, "Saving an image to: " + filePath + " result: " + (result ? "OK" : "Failed"))
}

void ConfigExporter::on_copyImageBtn_clicked()
{
    QGuiApplication::clipboard()->setImage(image);
    QvMessageBoxInfo(this, tr("Share Connection"), tr("Image has been copied to the clipboard."));
}

void ConfigExporter::on_copyVMessBtn_clicked()
{
    QGuiApplication::clipboard()->setText(message);
    QvMessageBoxInfo(this, tr("Share Connection"), tr("VMess string has been copied to the clipboard."));
}
