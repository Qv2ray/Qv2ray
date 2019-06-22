#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")

#include "ConnectionEditWindow.h"
#include "vinteract.h"
#include "utils.h"
#include "ImportConfig.h"
#include "ui_ImportConfig.h"

ImportConfig::ImportConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportConfig)
{
    ui->setupUi(this);
    connect(this, SIGNAL(updateConfTable()), parentWidget(), SLOT(updateConfTable()));
}

ImportConfig::~ImportConfig()
{
    delete ui;
}

void ImportConfig::on_pushButton_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Open Config File"), "~/");
    ui->fileLineTxt->setText(dir);
}

void ImportConfig::savefromFile(QString path, QString alias)
{
    vConfig newConf;
    newConf.alias = alias;
    QFile configFile(path);
    if(!configFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Couldn't open config json";
        return;
    }
    QByteArray allData = configFile.readAll();
    configFile.close();
    QJsonDocument v2conf(QJsonDocument::fromJson(allData));
    QJsonObject rootobj = v2conf.object();
    QJsonObject outbound;
    if(rootobj.contains("outbounds")) {
        outbound = rootobj.value("outbounds").toArray().first().toObject();
    } else {
        outbound = rootobj.value("outbound").toObject();
    }
    QJsonObject vnext = switchJsonArrayObject(outbound.value("settings").toObject(), "vnext");
    QJsonObject user = switchJsonArrayObject(vnext, "users");
    newConf.host = vnext.value("address").toString();
    newConf.port = QString::number(vnext.value("port").toInt());
    newConf.alterid = QString::number(user.value("alterId").toInt());
    newConf.uuid = user.value("id").toString();
    newConf.security = user.value("security").toString();
    if (newConf.security.isNull()) {
        newConf.security = "auto";
    }
    newConf.isCustom = 1;
    int id = newConf.save();
    if(id < 0)
    {
        showWarnMessageBox(this, "Database Error", "Failed to open database while saving");
        return;
    }
    emit updateConfTable();
    QString newFile = "conf/" + QString::number(id) + ".conf";
    if(!QFile::copy(path, newFile)) {
        showWarnMessageBox(this, "Copy error", "Failed to copy custom config file.");
    }
}

void ImportConfig::on_buttonBox_accepted()
{
    QString alias = ui->nameTxt->text();
    if(ui->importSourceCombo->currentIndex() == 0) // From File...
    {
        QString path = ui->fileLineTxt->text();
        bool isValid = validationCheck(path);
        if(isValid) {
            savefromFile(path, alias);
        }
    }
    else
    {
        QString vmess = ui->vmessConnectionStringTxt->toPlainText();
        Py_Initialize();
        if ( !Py_IsInitialized() ) {
            qDebug() << "Python is not initialized";
        }
        QString param = "--inbound socks:1080 " + vmess + " -o config.json.tmp";
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append('./utils')");
        PyObject *pModule = PyImport_ImportModule("vmess2json");
        PyObject *pFunc = PyObject_GetAttrString(pModule, "main");
        PyObject *arg = PyTuple_New(1);
        PyObject *arg1 = Py_BuildValue("s", param.toStdString().c_str());
        PyTuple_SetItem(arg, 0, arg1);
        PyObject_CallObject(pFunc, arg);
        Py_Finalize();
        if(QFile::exists(QCoreApplication::applicationDirPath() + "/config.json.tmp")) {
            ImportConfig *im = new ImportConfig(this->parentWidget());
            if (validationCheck(QCoreApplication::applicationDirPath() + "/config.json.tmp")) {
                im->savefromFile("config.json.tmp", alias);
            }
            QFile::remove("config.json.tmp");
        } else {
            showWarnMessageBox(this, "Error occured", "Failed to generate config file.");
        }
    }

    if(ui->useCurrentSettingRidBtn->isChecked())
    {
        // TODO: Use Current Settings...
    }
    else
    {
        // TODO: Override Inbound....
    }
}
