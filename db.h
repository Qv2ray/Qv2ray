#ifndef DB_H
#define DB_H
#include <QSqlQuery>

class db
{
public:
    db();
    QSqlQuery myQuery;
    void query(QString queryString);

};

#endif // DB_H
