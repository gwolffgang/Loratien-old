#ifndef DATABASE_H
#define DATABASE_H

#include <QFile>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

class Database {

private:
    QSqlDatabase db;

public:
    // constructor
    Database(QString hostName = "localhost", QString userName = "Loratien", QString userPassword = "loratien", QString databaseName = "Loratien");

    // selects
    QString getRandomFirstName(int gender);

    // inputs

    // updates

    // methods
    bool create();
};

#endif // DATABASE_H
