#include "confedit.h"
#include "ui_confedit.h"
#include <iostream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "mainwindow.h"
#include <QDebug>
#include <QSqlError>
#include <QFile>
#include "db.h"
#include <QIntValidator>

ConfEdit::ConfEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfEdit)
{
    ui->setupUi(this);
    connect(this, SIGNAL(updateConfTable()), parentWidget(), SLOT(updateConfTable()));
    ui->portLineEdit->setValidator(new QIntValidator());
    ui->alterLineEdit->setValidator(new QIntValidator());
}

ConfEdit::~ConfEdit()
{
    delete ui;
}
vConfig *vConfig::query(int id)
{
    QString selectQuery = "select * from confs where id = " + QString::number(id) + ";";
    db myDb = db();
    myDb.query(selectQuery);
    myDb.myQuery.first();
    this->host = myDb.myQuery.value(1).toString();
    this->port = myDb.myQuery.value(2).toString();
    this->alias = myDb.myQuery.value(3).toString();
    this->uuid = myDb.myQuery.value(4).toString();
    this->alterid = myDb.myQuery.value(5).toString();
    this->security = myDb.myQuery.value(6).toString();
    this->isCustom = myDb.myQuery.value(7).toInt();
    return this;
}
int vConfig::save()
{
    QSqlDatabase database;
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        database = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName(confDatabase);
    }
    if (!database.open()) {
        qDebug() << "Failed to open database while saving.";
        return -1;
    }
    QSqlQuery myQuery(database);
    myQuery.prepare("insert into confs (host, port, alias, uuid, alterid, security, isCustom, selected) values(:host, :port, :alias, :uuid, :alterid, :security, :isCustom, :selected)");
    myQuery.bindValue(":host", this->host);
    myQuery.bindValue(":port", this->port);
    myQuery.bindValue(":alias", this->alias);
    myQuery.bindValue(":uuid", this->uuid);
    myQuery.bindValue(":alterid", this->alterid);
    myQuery.bindValue(":security", this->security);
    myQuery.bindValue(":isCustom", this->isCustom);
    myQuery.bindValue(":selected", 0);
    myQuery.exec();
    myQuery.exec("select last_insert_rowid();");
    myQuery.first();
    return myQuery.value(0).toInt();
}

void vConfig::getConfigFromDialog(Ui::ConfEdit *ui)
{
    this->host = ui->ipLineEdit->text();
    this->port = ui->portLineEdit->text();
    this->alias = ui->aliasLineEdit->text();
    this->uuid = ui->idLineEdit->text();
    this->alterid = ui->alterLineEdit->text();
    this->security = ui->securityCombo->currentText();
    this->isCustom = 0;
}
void ConfEdit::on_ConfEdit_accepted()
{
    vConfig newConf;
    newConf.getConfigFromDialog(this->ui);
    newConf.save();
    emit updateConfTable();
}
