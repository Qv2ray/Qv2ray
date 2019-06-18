#ifndef DB_H
#define DB_H
#include <QSqlQuery>

class SQLiteDB
{
public:
    SQLiteDB();
    QSqlQuery myQuery;
    void DoQuery(QString queryString);

};

#endif // DB_H
