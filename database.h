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
    QString db_type, sqlFile;
    bool saveInDatabase, saveInFile;

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
    void saveBuildingDefs();
    void saveBuildingTypeDefs();
    void saveCharacter(Char *c);
    void saveFieldDefs();
    void saveNames(int gender);
    void saveProduction();
    void saveProductionTypeDefs();
    void saveVillageMap();

    // updates

    // methods
    bool create();
    void executeQuerylist(QList<QString> querylist);
    bool reset();
};

#endif // DATABASE_H
