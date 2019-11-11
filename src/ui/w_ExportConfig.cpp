#include "w_ExportConfig.hpp"
#include "QvUtils.hpp"

// Private initialiser
ConfigExporter::ConfigExporter(QWidget *parent) :
    QDialog(parent),
    qzxing(this)
{
    setupUi(this);
}

ConfigExporter::ConfigExporter(const QImage &img, QWidget *parent): ConfigExporter(parent)
{
    image = img;
    message = tr("Empty");
}
ConfigExporter::ConfigExporter(const QString &data, QWidget *parent): ConfigExporter(parent)
{
    QZXingEncoderConfig conf;
    conf.border = true;
    conf.imageSize = QSize(400, 400);
    auto img = qzxing.encodeData(data, conf);
    image = img.copy();
    message = data;
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
    if (!QDir(QV2RAY_QRCODE_DIR).exists()) {
        QDir().mkpath(QV2RAY_QRCODE_DIR);
    }

    auto filePath = QV2RAY_QRCODE_DIR + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss-z.png");
    auto result = image.save(filePath);
    QDesktopServices::openUrl(QUrl(QV2RAY_QRCODE_DIR));
    LOG(MODULE_FILE, "Saving an image to: " + filePath.toStdString() + " result: " + (result ? "OK" : "Failed"))
    // If succeed, we disable future save.
    saveBtn->setEnabled(result);
}
