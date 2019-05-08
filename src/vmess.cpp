#include "vmess.h"
#include "ui_vmess.h"
#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")
#include <QDebug>
#include "importconf.h"
#include <QFile>
#include "vinteract.h"
#include <QMessageBox>

vmess::vmess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::vmess)
{
    ui->setupUi(this);
}

vmess::~vmess()
{
    delete ui;
}

void vmess::on_buttonBox_accepted()
{
    QString vmess = ui->vmessTextEdit->toPlainText();
    QString alias = ui->aliasLineEdit->text();
    Py_Initialize();
    if ( !Py_IsInitialized() ) {
        qDebug() << "Python not initialized";
    }
    QString param = "--inbound socks:1080 " + vmess + " -o tmp.config.json";
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");
    PyObject *pModule = PyImport_ImportModule("vmess2json");
    PyObject *pFunc = PyObject_GetAttrString(pModule, "main");
    PyObject *arg = PyTuple_New(1);
    PyObject *arg1 = Py_BuildValue("s", param.toStdString().c_str());
    PyTuple_SetItem(arg, 0, arg1);
    PyObject_CallObject(pFunc, arg);
    Py_Finalize();
    if(QFile::exists(QCoreApplication::applicationDirPath() + "/tmp.config.json")) {
        importConf *im = new importConf(this->parentWidget());
        if (validationCheck(QCoreApplication::applicationDirPath() + "/tmp.config.json")) {
            im->savefromFile("tmp.config.json", alias);
        }
        QFile::remove("tmp.config.json");
    } else {
        QMessageBox::critical(0, "Error occured", "Config generation failed.", QMessageBox::Ok | QMessageBox::Default, 0);
    }
}
