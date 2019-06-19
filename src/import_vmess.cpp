#include "import_vmess.h"
#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")
#include <QDebug>
#include <QFile>

#include "vinteract.h"
#include "utils.h"
#include "importconf.h"
#include "ui_import_vmess.h"

import_vmess::import_vmess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::import_vmess)
{
    ui->setupUi(this);
}

import_vmess::~import_vmess()
{
    delete ui;
}

void import_vmess::on_buttonBox_accepted()
{
    QString vmess = ui->vmessTextEdit->toPlainText();
    QString alias = ui->aliasLineEdit->text();
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
        importConf *im = new importConf(this->parentWidget());
        if (validationCheck(QCoreApplication::applicationDirPath() + "/config.json.tmp")) {
            im->savefromFile("config.json.tmp", alias);
        }
        QFile::remove("config.json.tmp");
    } else {
        alterMessage("Error occured", "Failed to generate config file.");
    }
}
