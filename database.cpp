#include "database.h"

Database::Database(QString hostName, QString userName, QString userPassword, QString databaseName) {
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(hostName);
    db.setPort(3306);
    db.setUserName(userName);
    db.setPassword(userPassword);
    db.setDatabaseName(databaseName);
    create();
}

QString Database::getRandomFirstName(int gender) {
    if (db.open()) {
        QSqlQuery qry;
        qry.exec("SELECT name FROM first_names_def WHERE gender = " + QString::number(gender));
        db.close();
        int randomName = rand()%qry.size();
        while (qry.next()) if (randomName == qry.at()) return qry.value(0).toString();
    } else qDebug() << "Failed to connect to database." << db.lastError().text();
    return "";
}

bool Database::create() {
    if (db.open()) {
        QSqlQuery qry1, qry2;
        QString query = "";
        // DROP ALL TABLES
        qry1.exec("SHOW TABLES");
        while (qry1.next()) {
            qry2.exec("DROP TABLE " + qry1.value(0).toString());
        }

        // CREATE THE TABLES
        query = "CREATE TABLE first_names_def ("
                +QString("id INTEGER AUTO_INCREMENT PRIMARY KEY, ")
                +QString("name VARCHAR(20) NOT NULL, ")
                +QString("gender BOOL DEFAULT 1 NOT NULL)");
        qry1.exec(query);
        query = "CREATE TABLE people ("
                +QString("id INTEGER AUTO_INCREMENT PRIMARY KEY, ")
                +QString("name VARCHAR(20) NOT NULL, ")
                +QString("gender BOOL DEFAULT 1 NOT NULL)");
        qry1.exec(query);

        // FILL THE TABLES WITH VALUES
        QFile maleNames(":/data/lists/names_male.list");
        if (maleNames.open(QIODevice::ReadOnly)) {
           QTextStream in(&maleNames);
           QStringList list = in.readAll().split('\n');
           foreach(QString name, list)
               if (name != "")
                   qry1.exec("INSERT INTO first_names_def(name, gender) VALUES('"+name+"', 1)");
           maleNames.close();
        }
        QFile femaleNames(":/data/lists/names_female.list");
        if (femaleNames.open(QIODevice::ReadOnly)) {
           QTextStream in(&femaleNames);
           QStringList list = in.readAll().split('\n');
           foreach(QString name, list)
               if (name != "")
                   qry1.exec("INSERT INTO first_names_def(name, gender) VALUES('"+name+"', 0)");
           femaleNames.close();
        }
        db.close();
    } else {
        qDebug() << "Failed to connect to database." << db.lastError().text();
        return false;
    }
    return true;
}
