#ifndef DATABASE_H
#define DATABASE_H

#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include "char.h"

class Database {

private:
    QSqlDatabase db;
    QString db_type, sqlFile, sqlFileFormats;

public:
    // constructor
    Database(QString databaseType = "QMYSQL", QString hostName = "localhost", QString userName = "Loratien", QString userPassword = "loratien", QString databaseName = "Loratien");

    // creates
    void createSequence(QString tableName);
    void createTable(QString tableName, QString primaryKeys = "id");

    // drops
    void dropTable(QString tableName);

    // alters
    void addColumn(QString tableName, QString columnName, QString dataType, bool notNull = 0, QString defaultValue = "");
    void addForeignKey(QString tableName, QString columnName, QString linkedTable, QString linkedColumn, QString altName = "");

    // selects
    QString getRandomFirstName(int gender);

    // inserts
    void saveCharacter(Char *c);
    void saveLists(QString lists);
    void saveVillage();

    // updates

    // methods
    bool create();
    void executeQuerylist(QList<QString> querylist, bool suppressErrors = false);
    inline bool isNumeric(QString string) { string.replace(" ", ""); for (int i = 0; i < string.size(); i++) if (!string[i].isDigit()) return false; return true; }
    bool reset();
};

#endif // DATABASE_H
