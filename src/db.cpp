#include "db.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include "mainwindow.h"
#include <QDebug>

db::db()
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
        this->myQuery = QSqlQuery(database);
    }
}

void db::query(QString queryString)
{
    this->myQuery.prepare(queryString);
    this->myQuery.exec();
}
