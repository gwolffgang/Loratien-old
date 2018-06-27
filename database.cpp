#include "database.h"
#include "loratien.h"

extern Loratien *game;

Database::Database(QString databaseType, QString hostName, QString userName, QString userPassword, QString databaseName) : db_type(databaseType){
    db = QSqlDatabase::addDatabase(db_type);
    db.setHostName(hostName);
    db.setPort(3306);
    db.setUserName(userName);
    db.setPassword(userPassword);
    db.setDatabaseName(databaseName);
    create();
}

void Database::createSequence(QString tableName) {
    QSqlQuery qry;
    QString query = "";
    qry.exec("DROP SEQUENCE " + tableName + "_sequence");
    qry.exec("CREATE SEQUENCE " + tableName + "_sequence START WITH 1");
    query  = "CREATE OR REPLACE TRIGGER " + tableName + "_on_insert ";
    query += "BEFORE INSERT ON " + tableName + " ";
    query += "  FOR EACH ROW ";
    query += "BEGIN ";
    query += "  SELECT " + tableName + "_sequence.NEXTVAL ";
    query += "  INTO :new.id ";
    query += "  FROM dual; ";
    query += "END; ";
    query += "/ ";
    if (!qry.exec(query)) qDebug() << qry.lastError();
}

void Database::createTable(QString tableName, QString primaryKeys) {
    QSqlQuery qry;
    QString query;
    QStringList keys = primaryKeys.split(",");
    query  = QString("CREATE TABLE " + tableName + "(");
    foreach (QString key, keys) {
        if (db_type == "QMYSQL" && keys.size() == 1) query += QString(key + " INTEGER AUTO_INCREMENT, ");
        else query += QString(key + " INTEGER, ");
    }
    query += QString("CONSTRAINT PK_" + tableName + " PRIMARY KEY(" + primaryKeys + ") ");
    query += QString(")");
    qry.exec(query);
    if (db_type == "QOCI") createSequence(tableName);
}

void Database::dropTable(QString tableName) {
    QSqlQuery qry1, qry2;
    if (tableName == "all") {
        qry1.exec("SHOW TABLES");
        while (qry1.size() > 0) {
            while (qry1.next()) {
                qry2.exec("DROP TABLE " + qry1.value(0).toString());
            }
            qry1.exec("SHOW TABLES");
        }
    } else qry1.exec("DROP TABLE " + tableName);
}

void Database::addColumn(QString tableName, QString columnName, QString dataType, bool notNull, QString defaultValue) {
    QSqlQuery qry;
    QString query = "ALTER TABLE " + tableName + " ADD " + columnName + " ";
    if (dataType == "BOOL") {
        if (db_type == "QMYSQL") query += QString("BOOL");
        else if (db_type == "QOCI") query += QString("NUMBER(1,0)");
        else query += QString("SHORTINT");
    } else query += dataType;
    if (defaultValue != "") query += " DEFAULT " + defaultValue;
    if (notNull) query += " NOT NULL";
    if (!qry.exec(query)) qDebug() << qry.lastError();
}

void Database::addForeignKey(QString tableName, QString columnName, QString linkedTable, QString linkedColumn, QString altName) {
    QSqlQuery qry;
    QString query = "ALTER TABLE " + tableName + " ADD CONSTRAINT ";
    if (altName != "") query += "FK_" + altName;
    else query += "FK_" + tableName + "_" + columnName;
    query += " FOREIGN KEY (" + columnName + ") REFERENCES " + linkedTable + " (" + linkedColumn + ")";
    if (!qry.exec(query)) qDebug() << query << qry.lastError().text();
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

void Database::saveCharacter(Char *c) {
    if (db.open()) {
        QSqlQuery qry;
        QString query = "";
        query  = "INSERT INTO people (x,y,last_name,first_name,gender,age,birthday,day_of_death,concept,virtue,vice,size_,";
        query += "defense, speed, initiative, health, health_current, mana, mana_current, willpower, willpower_current,";
        query += "intelligence, wits, resolve, strength, dexterity, stamina, presence, manipulation, composure,";
        query += "academics, crafts, investigation, magic, medicine, politics, science, spirituality,";
        query += "athletics, brawl, riding, ranged_combat, larceny, stealth, survival, weaponry,";
        query += "animal_ken, empathy, expression, intimidation, persuation, socialize, streetwise, subterfuge)";
        query += "VALUES ("+QString::number(c->getLocation()->getCol())+","+QString::number(c->getLocation()->getRow())+",'";
        query += c->getLastName()+"','"+c->getFirstName()+"',"+QString::number(c->getGender())+",'"+QString::number(c->getAge())+"',";
        query += c->getBirthday()+",'"+c->getDayOfDeath()+"','"+c->getConcept()+"','"+c->getVirtue()+"','"+c->getVice()+"',";
        query += QString::number(c->getSize())+","+QString::number(c->getDefense())+","+QString::number(c->getSpeed())+",";
        query += QString::number(c->getInitiative())+","+QString::number(c->getHealth())+","+QString::number(c->getHealthCurrent())+",";
        query += QString::number(c->getMana())+","+QString::number(c->getManaCurrent())+",";
        query += QString::number(c->getWillpower())+","+QString::number(c->getWillpowerCurrent())+",";
        query += QString::number(c->getIntelligence())+","+QString::number(c->getWits())+","+QString::number(c->getResolve())+",";
        query += QString::number(c->getStrength())+","+QString::number(c->getDexterity())+","+QString::number(c->getStamina())+",";
        query += QString::number(c->getPresence())+","+QString::number(c->getManipulation())+","+QString::number(c->getComposure())+",";
        query += QString::number(c->getAcademics())+","+QString::number(c->getCrafts())+","+QString::number(c->getInvestigation())+",";
        query += QString::number(c->getMagic())+","+QString::number(c->getMedicine())+","+QString::number(c->getPolitics())+",";
        query += QString::number(c->getScience())+","+QString::number(c->getSpirituality())+","+QString::number(c->getAthletics())+",";
        query += QString::number(c->getBrawl())+","+QString::number(c->getRiding())+","+QString::number(c->getRangedCombat())+",";
        query += QString::number(c->getLarceny())+","+QString::number(c->getStealth())+","+QString::number(c->getSurvival())+",";
        query += QString::number(c->getWeaponry())+","+QString::number(c->getAnimalKen())+","+QString::number(c->getEmpathy())+",";
        query += QString::number(c->getExpression())+","+QString::number(c->getIntimidation())+","+QString::number(c->getPersuation())+",";
        query += QString::number(c->getSocialize())+","+QString::number(c->getStreetwise())+","+QString::number(c->getSubterfuge())+")";
        if (!qry.exec(query)) qDebug() << qry.lastError().text();
    } else qDebug() << "Failed to connect to database." << db.lastError().text();
}

void Database::saveFieldDefs() {
    QSqlQuery qry;
    if (!qry.exec("INSERT INTO fields_def (type, passable) VALUES ('plain', 1)")) qDebug() << qry.lastError().text();
}

void Database::saveNames(int gender) {
    QFile names;
    if (gender == 1) names.setFileName(":/data/lists/names_male.list");
    else names.setFileName(":/data/lists/names_female.list");
    if (names.open(QIODevice::ReadOnly)) {
        QTextStream in(&names);
        QStringList list = in.readAll().split('\n');
        QSqlQuery qry;
        foreach(QString name, list) {
            if (name != "") {
                if (!qry.exec("INSERT INTO first_names_def(name, gender) VALUES('" + name + "', " + QString::number(gender) + ")"))
                    qDebug() << qry.lastError().text();
            }
        }
        names.close();
    }
}

void Database::saveVillageMap() {
    QSqlQuery qry;
    for (int x = 0; x < game->getWorldWidth(); x++) {
        for (int y = 0; y < game->getWorldHeight(); y++) {
            if (!qry.exec("INSERT INTO village_map (x,y) VALUES ("+ QString::number(x) + "," + QString::number(y) +")"))
                qDebug() << qry.lastError().text();
        }
    }
}

bool Database::create() {
    if (db.open()) {
        dropTable("all");

        createTable("fields_def", "id");
        addColumn("fields_def", "type", "VARCHAR(10)", true);
        addColumn("fields_def", "passable", "BOOL", true, "1");
        saveFieldDefs();

        createTable("village_map", "x,y");
        addColumn("village_map", "fieldtype", "INTEGER", true, "1");
        addForeignKey("village_map", "fieldtype", "fields_def", "id");
        saveVillageMap();

        createTable("first_names_def", "id");
        addColumn("first_names_def", "name", "VARCHAR(20)", true);
        addColumn("first_names_def", "gender", "BOOL", true, "1");
        saveNames(1);
        saveNames(0);

        createTable("people", "id");
        addColumn("people", "x", "INTEGER", true);
        addColumn("people", "y", "INTEGER", true);
        addForeignKey("people", "x,y", "village_map", "x,y", "people_location");
        /*addColumn("people", "last_name", "INTEGER", true);
        addForeignKey("people", "last_name", "first_names_def", "id");
        addColumn("people", "first_name", "INTEGER", true);
        addForeignKey("people", "first_name", "first_names_def", "id");*/
        addColumn("people", "last_name", "VARCHAR(20)", true);
        addColumn("people", "first_name", "VARCHAR(20)", true);
        addColumn("people", "gender", "BOOL", true, "1");
        addColumn("people", "age", "SMALLINT", true);
        addColumn("people", "birthday", "VARCHAR(11)", true);
        addColumn("people", "day_of_death", "VARCHAR(11)", false, "NULL");
        addColumn("people", "concept", "VARCHAR(10)", false, "NULL");
        addColumn("people", "virtue", "VARCHAR(10)", false, "NULL");
        addColumn("people", "vice", "VARCHAR(10)", false, "NULL");
        addColumn("people", "size_", "SMALLINT", true, "5");
        addColumn("people", "defense", "SMALLINT", true, "2");
        addColumn("people", "speed", "SMALLINT", true, "7");
        addColumn("people", "initiative", "SMALLINT", true, "2");
        addColumn("people", "health", "SMALLINT", true, "6");
        addColumn("people", "health_current", "SMALLINT", true, "6");
        addColumn("people", "mana", "SMALLINT", true, "3");
        addColumn("people", "mana_current", "SMALLINT", true, "3");
        addColumn("people", "willpower", "SMALLINT", true, "2");
        addColumn("people", "willpower_current", "SMALLINT", true, "2");
        addColumn("people", "intelligence", "SMALLINT", true, "1");
        addColumn("people", "wits", "SMALLINT", true, "1");
        addColumn("people", "resolve", "SMALLINT", true, "1");
        addColumn("people", "strength", "SMALLINT", true, "1");
        addColumn("people", "dexterity", "SMALLINT", true, "1");
        addColumn("people", "stamina", "SMALLINT", true, "1");
        addColumn("people", "presence", "SMALLINT", true, "1");
        addColumn("people", "manipulation", "SMALLINT", true, "1");
        addColumn("people", "composure", "SMALLINT", true, "1");
        addColumn("people", "academics", "SMALLINT", true, "-3");
        addColumn("people", "crafts", "SMALLINT", true, "-3");
        addColumn("people", "investigation", "SMALLINT", true, "-3");
        addColumn("people", "magic", "SMALLINT", true, "-3");
        addColumn("people", "medicine", "SMALLINT", true, "-3");
        addColumn("people", "politics", "SMALLINT", true, "-3");
        addColumn("people", "science", "SMALLINT", true, "-3");
        addColumn("people", "spirituality", "SMALLINT", true, "-3");
        addColumn("people", "athletics", "SMALLINT", true, "-1");
        addColumn("people", "brawl", "SMALLINT", true, "-1");
        addColumn("people", "riding", "SMALLINT", true, "-1");
        addColumn("people", "ranged_combat", "SMALLINT", true, "-1");
        addColumn("people", "larceny", "SMALLINT", true, "-1");
        addColumn("people", "stealth", "SMALLINT", true, "-1");
        addColumn("people", "survival", "SMALLINT", true, "-1");
        addColumn("people", "weaponry", "SMALLINT", true, "-1");
        addColumn("people", "animal_ken", "SMALLINT", true, "-1");
        addColumn("people", "empathy", "SMALLINT", true, "-1");
        addColumn("people", "expression", "SMALLINT", true, "-1");
        addColumn("people", "intimidation", "SMALLINT", true, "-1");
        addColumn("people", "persuation", "SMALLINT", true, "-1");
        addColumn("people", "socialize", "SMALLINT", true, "-1");
        addColumn("people", "streetwise", "SMALLINT", true, "-1");
        addColumn("people", "subterfuge", "SMALLINT", true, "-1");
        db.close();
    } else {
        qDebug() << "Failed to connect to database." << db.lastError().text();
        return false;
    }
    return true;
}
