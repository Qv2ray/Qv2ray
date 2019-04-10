#include "confedit.h"
#include "ui_confedit.h"
#include <iostream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <mainwindow.h>
#include <QDebug>
#include <QSqlError>
#include <QFile>

ConfEdit::ConfEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfEdit)
{
    ui->setupUi(this);
}

ConfEdit::~ConfEdit()
{
    delete ui;
}
vConfig *vConfig::query(int id)
{
    QSqlDatabase database;
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        database = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName(confDatabase);
    }
    if (!database.open()) {
        qDebug() << "Failed to open database while querying.";
    } else {
        QSqlQuery myQuery(database);
        QString selectQuery = "select * from confs where id = " + QString::number(id) + ";";
        myQuery.exec(selectQuery);
        myQuery.first();
        this->host = myQuery.value(1).toString();
        this->port = myQuery.value(2).toString();
        this->alias = myQuery.value(3).toString();
        this->uuid = myQuery.value(4).toString();
        this->alterid = myQuery.value(5).toString();
        this->security = myQuery.value(6).toString();
        this->isCustom = myQuery.value(6).toInt();
        return this;
    }
}
void *vConfig::save()
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
    } else {
        QSqlQuery myQuery(database);
        myQuery.prepare("insert into confs (host, port, alias, uuid, alterid, security, isCustom) values(:host, :port, :alias, :uuid, :alterid, :security, :isCustom)");
        myQuery.bindValue(":host", this->host);
        myQuery.bindValue(":port", this->port);
        myQuery.bindValue(":alias", this->alias);
        myQuery.bindValue(":uuid", this->uuid);
        myQuery.bindValue(":alterid", this->alterid);
        myQuery.bindValue(":security", this->security);
        myQuery.bindValue(":isCustom", this->isCustom);
        myQuery.exec();
        return this;
    }
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
void vConfig::getConfigFromCustom(QString path)
{
    QFile file(path);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << " Could not open the file for reading";
        return;
    }
    return;
}
void ConfEdit::on_ConfEdit_accepted()
{
    vConfig newConf;
    newConf.getConfigFromDialog(this->ui);
    newConf.save();
}
