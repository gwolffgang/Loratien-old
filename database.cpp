#include "database.h"
#include "loratien.h"

extern Loratien *game;

Database::Database(QString databaseType, QString hostName, QString userName, QString userPassword, QString databaseName) : db_type(databaseType),
    sqlFile(""), sqlFileFormats("QMYSQL,QOCI") {
    db = QSqlDatabase::addDatabase(db_type);
    db.setHostName(hostName);
    db.setPort(3306);
    db.setUserName(userName);
    db.setPassword(userPassword);
    db.setDatabaseName(databaseName);
    if (!reset()) qDebug() << "Cannot clear Database.";
    if (!create()) qDebug() << "Cannot fill Database.";
}

void Database::createSequence(QString tableName) {
    QString query;
    QList<QString> querylist;
    querylist.append("DROP SEQUENCE SQ_" + tableName + "|QOCI");
    querylist.append("CREATE SEQUENCE SQ_" + tableName + " START WITH 1|QOCI");
    querylist.append("CREATE OR REPLACE TRIGGER T_" + tableName + " BEFORE INSERT ON " + tableName + " FOR EACH ROW BEGIN SELECT SQ_" + tableName + ".NEXTVAL INTO :new.id FROM dual; END|QOCI");
    querylist.append("/|QOCI");
    executeQuerylist(querylist);
}

void Database::createTable(QString tableName, QString primaryKeys) {
    QString query;
    QList<QString> querylist;
    querylist.append("");
    QStringList keys = primaryKeys.split(",");
    QStringList formats = sqlFileFormats.split(",");
    foreach (QString format, formats){
        query  = QString("CREATE TABLE " + tableName + "(");
        foreach (QString key, keys) {
            if (format == "QMYSQL" && keys.size() == 1) query += QString(key + " INTEGER AUTO_INCREMENT, ");
            else query += QString(key + " INTEGER, ");
        }
        query += QString("CONSTRAINT PK_" + tableName + " PRIMARY KEY(" + primaryKeys + "))");
        query += "|" + format;
        querylist.append(query);
    }
    query += QString("CONSTRAINT PK_" + tableName + " PRIMARY KEY(" + primaryKeys + "))");
    querylist.append(query);
    executeQuerylist(querylist);
    foreach (QString format, formats){
        if (format == "QOCI" && keys.size() == 1) createSequence(tableName);
    }
}

void Database::dropTable(QString tableName) {
    QList<QString> querylist;
    if (tableName == "all") reset();
    else {
        querylist.append("DROP TABLE " + tableName);
        executeQuerylist(querylist);
    }
}

void Database::addColumn(QString tableName, QString columnName, QString dataType, bool notNull, QString defaultValue) {
    QList<QString> querylist;
    tableName.replace(" ", "");
    columnName.replace(" ", "");
    QString query = "ALTER TABLE " + tableName + " ADD " + columnName + " " + dataType;
    if (defaultValue != "") query += " DEFAULT " + defaultValue;
    if (notNull) query += " NOT NULL";
    querylist.append(query);
    executeQuerylist(querylist);
}

void Database::addForeignKey(QString tableName, QString columnName, QString linkedTable, QString linkedColumn, QString altName) {
    QList<QString> querylist;
    columnName.replace(" ", "");
    QString query = "ALTER TABLE " + tableName + " ADD CONSTRAINT ";
    if (altName != "") query += "FK_" + altName;
    else query += "FK_" + tableName + "_" + columnName;
    query += " FOREIGN KEY (" + columnName + ") REFERENCES " + linkedTable + " (" + linkedColumn + ")";
    querylist.append(query);
    executeQuerylist(querylist);
}

QString Database::getRandomFirstName(int gender) {
    if (db.open()) {
        QSqlQuery qry;
        qry.exec("SELECT name FROM first_name_def WHERE gender = " + QString::number(gender));
        db.close();
        int randomName = rand()%qry.size();
        while (qry.next()) if (randomName == qry.at()) return qry.value(0).toString();
    } else qDebug() << "Failed to connect to database." << db.lastError().text();
    return "";
}

void Database::saveCharacter(Char *c) {
    if (db.open()) {
        QSqlQuery qry_name;
        QList<QString> querylist;
        querylist.append("");
        int name_id;
        qry_name.exec("SELECT id FROM first_name_def WHERE name='"+c->getFirstName()+"'");
        while (qry_name.next()) name_id = qry_name.value(0).toInt();
        QString query = "INSERT INTO people(x, y, last_name, first_name, gender, age, birthday, day_of_death, concept, virtue, vice, size_, ";
        query += "defense, speed, initiative, health, health_current, mana, mana_current, willpower, willpower_current, ";
        query += "intelligence, wits, resolve, strength, dexterity, stamina, presence, manipulation, composure, ";
        query += "academics, crafts, investigation, magic, medicine, politics, science, spirituality, ";
        query += "athletics, brawl, riding, ranged_combat, larceny, stealth, survival, weaponry, ";
        query += "animal_ken, empathy, expression, intimidation, persuation, socialize, streetwise, subterfuge) ";
        query += "VALUES("+QString::number(c->getLocation()->getCol())+", "+QString::number(c->getLocation()->getRow())+", '";
        query += c->getLastName()+"', "+QString::number(name_id)+", "+QString::number(c->getGender())+", "+QString::number(c->getAge())+", '";
        query += c->getBirthday()+"', '"+c->getDayOfDeath()+"', '"+c->getConcept()+"', '"+c->getVirtue()+"', '"+c->getVice()+"', ";
        query += QString::number(c->getSize())+", "+QString::number(c->getDefense())+", "+QString::number(c->getSpeed())+", ";
        query += QString::number(c->getInitiative())+", "+QString::number(c->getHealth())+", "+QString::number(c->getHealthCurrent())+", ";
        query += QString::number(c->getMana())+", "+QString::number(c->getManaCurrent())+", ";
        query += QString::number(c->getWillpower())+", "+QString::number(c->getWillpowerCurrent())+", ";
        query += QString::number(c->getIntelligence())+", "+QString::number(c->getWits())+", "+QString::number(c->getResolve())+", ";
        query += QString::number(c->getStrength())+", "+QString::number(c->getDexterity())+", "+QString::number(c->getStamina())+", ";
        query += QString::number(c->getPresence())+", "+QString::number(c->getManipulation())+", "+QString::number(c->getComposure())+", ";
        query += QString::number(c->getAcademics())+", "+QString::number(c->getCrafts())+", "+QString::number(c->getInvestigation())+", ";
        query += QString::number(c->getMagic())+", "+QString::number(c->getMedicine())+", "+QString::number(c->getPolitics())+", ";
        query += QString::number(c->getScience())+", "+QString::number(c->getSpirituality())+", "+QString::number(c->getAthletics())+", ";
        query += QString::number(c->getBrawl())+", "+QString::number(c->getRiding())+", "+QString::number(c->getRangedCombat())+", ";
        query += QString::number(c->getLarceny())+", "+QString::number(c->getStealth())+", "+QString::number(c->getSurvival())+", ";
        query += QString::number(c->getWeaponry())+", "+QString::number(c->getAnimalKen())+", "+QString::number(c->getEmpathy())+", ";
        query += QString::number(c->getExpression())+", "+QString::number(c->getIntimidation())+", "+QString::number(c->getPersuation())+", ";
        query += QString::number(c->getSocialize())+", "+QString::number(c->getStreetwise())+", "+QString::number(c->getSubterfuge())+")";
        querylist.append(query);
        executeQuerylist(querylist);
    } else qDebug() << "Failed to connect to database." << db.lastError().text();
}

void Database::saveLists(QString lists) {
    QDirIterator sources(lists, QDirIterator::Subdirectories);
    QList<QString> list;
    while (sources.hasNext()) list.append(sources.next());
    std::stable_sort(list.begin(), list.end());
    foreach (QString currentFilename, list) {
        QStringList parts = currentFilename.split("/");
        QString tableName = parts.last();
        tableName = tableName.split(".").first();
        createTable(tableName);
        QFile source(currentFilename);
        if (source.open(QIODevice::ReadOnly)) {
            QTextStream in(&source);
            QStringList rows = in.readAll().split("\n");
            QList<QString> querylist;
            QString columnNames = rows.at(0);
            QStringList columns = columnNames.split(",");
            QString valuesRow = rows.at(3);
            QStringList values = valuesRow.split(",");
            for (int i = 0; i < values.size(); i++) {
                QString value = values.at(i);
                if (isNumeric(value)) addColumn(tableName, columns.at(i), "INTEGER", true);
                else if (value.toUpper().contains("SELECT")) {
                    QStringList parts = value.toLower().split(" from ");
                    QString keyId = parts.first();
                    QString keyTable = parts.last();
                    parts = keyId.split("select ");
                    keyId = parts.last();
                    parts = keyTable.split(" where");
                    keyTable = parts.first();
                    addColumn(tableName, columns.at(i), "INTEGER", true);
                    addForeignKey(tableName, columns.at(i), keyTable, keyId);
                } else addColumn(tableName, columns.at(i), "VARCHAR(20)", true);
            }
            for (int k = 1; k < rows.size(); k++) {
                QString elem = rows.at(k);
                if (elem != ""){
                    parts = elem.split(",");
                    QString query = "INSERT INTO " + tableName + "("+ columnNames + ") VALUES(";
                    for (int l = 0; l < parts.size(); l++) {
                        if (0 < l) query += ",";
                        QString string = parts.at(l);
                        if (isNumeric(string) || string.contains("SELECT")) query += string;
                        else query += "'" + string + "'";
                    }
                    query += ")";
                    querylist.append(query);
                }
            }
            source.close();
            executeQuerylist(querylist);
        }
    }
}

void Database::saveVillage() {
    QList<QString> querylist;
    for (int x = 0; x < game->getWorldMapWidth(); x++) {
        for (int y = 0; y < game->getWorldMapHeight(); y++) {
            querylist.append("INSERT INTO village(x, y) VALUES("+ QString::number(x) + ", " + QString::number(y) +")");
        }
    }
    executeQuerylist(querylist);
}

bool Database::create() {
    if (db.open()) {
        saveLists(":/data/lists/type_def");
        saveLists(":/data/lists/def");

        createTable("village", "x,y");
        addColumn("village", "type", "INTEGER", true, "1");
        addForeignKey("village", "type", "field_def", "id");
        saveVillage();

        createTable("people");
        addColumn("people", "x", "INTEGER", true);
        addColumn("people", "y", "INTEGER", true);
        addForeignKey("people", "x,y", "village", "x,y", "people_location");
        /*addColumn("people", "last_name", "INTEGER", true);
        addForeignKey("people", "last_name", "first_name_def", "id");*/
        addColumn("people", "last_name", "VARCHAR(20)", false);
        addColumn("people", "first_name", "INTEGER", true);
        addForeignKey("people", "first_name", "first_name_def", "id");
        addColumn("people", "gender", "SMALLINT", true, "1");
        addColumn("people", "age", "SMALLINT", true);
        addColumn("people", "birthday", "VARCHAR(11)", true);
        addColumn("people", "day_of_death", "VARCHAR(11)", false, "NULL");
        addColumn("people", "concept", "VARCHAR(10)", false, "NULL");
        addColumn("people", "virtue", "VARCHAR(10)", false, "NULL");
        addColumn("people", "vice", "VARCHAR(10)", false, "NULL");
        addColumn("people", "size_", "SMALLINT", true, "5");
        addColumn("people", "weight", "SMALLINT", true, "70");
        addColumn("people", "defense", "SMALLINT", true, "2");
        addColumn("people", "speed", "SMALLINT", true, "7");
        addColumn("people", "initiative", "SMALLINT", true, "2");
        addColumn("people", "health", "SMALLINT", true, "6");
        addColumn("people", "health_current", "SMALLINT", true, "6");
        addColumn("people", "mana", "SMALLINT", false, "NULL");
        addColumn("people", "mana_current", "SMALLINT", false, "NULL");
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

        createTable("buildings");
        addColumn("buildings", "x", "INTEGER", true);
        addColumn("buildings", "y", "INTEGER", true);
        addForeignKey("buildings", "x,y", "village", "x,y", "buildings_location");
        addColumn("buildings", "building", "INTEGER", true);
        addForeignKey("buildings", "building", "building_def", "id");
        addColumn("buildings", "condition_", "SMALLINT", true, "100");

        createTable("building_own");
        addColumn("building_own", "building", "INTEGER", true);
        addForeignKey("building_own", "building", "buildings", "id");
        addColumn("building_own", "owner", "INTEGER", true);
        addForeignKey("building_own", "owner", "people", "id");

        createTable("production");
        addColumn("production", "building", "INTEGER", true);
        addForeignKey("production", "building", "building_def", "id");
        addColumn("production", "type", "INTEGER", true);
        addForeignKey("production", "type", "production_type_def", "id");
        addColumn("production", "amount", "SMALLINT", true, "0");
        addColumn("production", "academics", "SMALLINT", true, "0");
        addColumn("production", "crafts", "SMALLINT", true, "0");
        addColumn("production", "medicine", "SMALLINT", true, "0");
        addColumn("production", "politics", "SMALLINT", true, "0");
        addColumn("production", "science", "SMALLINT", true, "0");
        addColumn("production", "spirituality", "SMALLINT", true, "0");
        addColumn("production", "persuation", "SMALLINT", true, "0");
        addColumn("production", "subterfuge", "SMALLINT", true, "0");

        createTable("animal_def");
        addColumn("animal_def", "type", "INTEGER", true);
        addForeignKey("animal_def", "type", "animal_type_def", "id");
        addColumn("animal_def","size_","SMALLINT", true);
        addColumn("animal_def", "weight", "SMALLINT", true);
        addColumn("animal_def","damage", "SMALLINT", true);
        addColumn("animal_def","defense", "SMALLINT", true);
        addColumn("animal_def", "speed", "SMALLINT", true);
        addColumn("animal_def", "initiative", "SMALLINT", true);
        addColumn("animal_def","health", "SMALLINT", true);
        addColumn("animal_def","mana", "SMALLINT", false, "NULL");
        addColumn("animal_def", "intelligence", "SMALLINT", true, "1");
        addColumn("animal_def", "wits", "SMALLINT", true, "1");
        addColumn("animal_def", "resolve", "SMALLINT", true, "1");
        addColumn("animal_def", "strength", "SMALLINT", true, "1");
        addColumn("animal_def", "dexterity", "SMALLINT", true, "1");
        addColumn("animal_def", "stamina", "SMALLINT", true, "1");
        addColumn("animal_def", "presence", "SMALLINT", true, "1");
        addColumn("animal_def", "manipulation", "SMALLINT", true, "1");
        addColumn("animal_def", "composure", "SMALLINT", true, "1");
        addColumn("animal_def", "athletics", "SMALLINT", true, "-1");
        addColumn("animal_def", "brawl", "SMALLINT", true, "-1");
        addColumn("animal_def", "stealth", "SMALLINT", true, "-1");
        addColumn("animal_def", "survival", "SMALLINT", true, "-1");
        addColumn("animal_def", "intimidation", "SMALLINT", true, "-1");

        createTable("animals");
        addColumn("animals", "x", "INTEGER", true);
        addColumn("animals", "y", "INTEGER", true);
        addForeignKey("animals", "x,y", "village", "x,y", "animals_location");
        addColumn("animals", "type", "INTEGER", true);
        addForeignKey("animals", "type", "animal_def", "id");
        addColumn("animals", "name", "VARCHAR(20)", true, "''");
        addColumn("animals", "birthday", "VARCHAR(11)", true);
        addColumn("animals", "day_of_death", "VARCHAR(11)", false, "NULL");
        addColumn("animals","health_current", "SMALLINT", true);
        addColumn("animals","mana_current", "SMALLINT", false, "NULL");
        addColumn("animals","size_","SMALLINT", false, "NULL");
        addColumn("animals", "weight", "SMALLINT", false, "NULL");
        addColumn("animals","damage", "SMALLINT", false, "NULL");
        addColumn("animals","defense", "SMALLINT", false, "NULL");
        addColumn("animals", "speed", "SMALLINT", false, "NULL");
        addColumn("animals", "initiative", "SMALLINT", false, "NULL");
        addColumn("animals","health", "SMALLINT", false, "NULL");
        addColumn("animals","mana", "SMALLINT", false, "NULL");
        addColumn("animals", "intelligence", "SMALLINT", false, "NULL");
        addColumn("animals", "wits", "SMALLINT", false, "NULL");
        addColumn("animals", "resolve", "SMALLINT", false, "NULL");
        addColumn("animals", "strength", "SMALLINT", false, "NULL");
        addColumn("animals", "dexterity", "SMALLINT", false, "NULL");
        addColumn("animals", "stamina", "SMALLINT", false, "NULL");
        addColumn("animals", "presence", "SMALLINT", false, "NULL");
        addColumn("animals", "manipulation", "SMALLINT", false, "NULL");
        addColumn("animals", "composure", "SMALLINT", false, "NULL");
        addColumn("animals", "athletics", "SMALLINT", false, "NULL");
        addColumn("animals", "brawl", "SMALLINT", false, "NULL");
        addColumn("animals", "stealth", "SMALLINT", false, "NULL");
        addColumn("animals", "survival", "SMALLINT", false, "NULL");
        addColumn("animals", "intimidation", "SMALLINT", false, "NULL");

        createTable("animal_own");
        addColumn("animal_own", "animal", "INTEGER", true);
        addForeignKey("animal_own", "animal", "animals", "id");
        addColumn("animal_own", "owner", "INTEGER", true);
        addForeignKey("animal_own", "owner", "people", "id");

        createTable("animal_inv");
        addColumn("animal_inv", "animal", "INTEGER", true);
        addForeignKey("animal_inv", "animal", "animals", "id");
        addColumn("animal_inv", "person", "INTEGER", true);
        addForeignKey("animal_inv", "person", "people", "id");

        createTable("cloth_def");
        addColumn("cloth_def", "type", "INTEGER", true);
        addForeignKey("cloth_def", "type", "cloth_type_def", "id");
        addColumn("cloth_def", "weight", "SMALLINT", true);
        addColumn("cloth_def", "condition_", "SMALLINT", true, "10");
        addColumn("cloth_def", "defense", "SMALLINT", true, "0");
        addColumn("cloth_def", "speed", "SMALLINT", true, "0");
        addColumn("cloth_def", "strength", "SMALLINT", true, "0");

        createTable("cloths");
        addColumn("cloths", "x", "INTEGER", true);
        addColumn("cloths", "y", "INTEGER", true);
        addForeignKey("cloths", "x,y", "village", "x,y", "cloths_location");
        addColumn("cloths", "type", "INTEGER", true);
        addForeignKey("cloths", "type", "cloth_def", "id");
        addColumn("cloths", "weight", "SMALLINT", true);
        addColumn("cloths", "name", "VARCHAR(20)", true, "''");
        addColumn("cloths", "condition_", "SMALLINT", true, "10");
        addColumn("cloths", "defense", "SMALLINT", true, "0");
        addColumn("cloths", "speed", "SMALLINT", true, "0");
        addColumn("cloths", "strength", "SMALLINT", true, "0");

        createTable("cloth_own");
        addColumn("cloth_own", "object", "INTEGER", true);
        addForeignKey("cloth_own", "object", "cloths", "id");
        addColumn("cloth_own", "owner", "INTEGER", true);
        addForeignKey("cloth_own", "owner", "people", "id");

        createTable("cloth_inv");
        addColumn("cloth_inv", "object", "INTEGER", true);
        addForeignKey("cloth_inv", "object", "cloths", "id");
        addColumn("cloth_inv", "person", "INTEGER", true);
        addForeignKey("cloth_inv", "person", "people", "id");

        createTable("food_def");
        addColumn("food_def", "type", "INTEGER", true);
        addForeignKey("food_def", "type", "food_type_def", "id");
        addColumn("food_def", "weight", "SMALLINT", true);
        addColumn("food_def", "health", "SMALLINT", true, "0");
        addColumn("food_def", "mana", "SMALLINT", true, "0");

        createTable("food");
        addColumn("food", "x", "INTEGER", true);
        addColumn("food", "y", "INTEGER", true);
        addForeignKey("food", "x,y", "village", "x,y", "food_location");
        addColumn("food", "type", "INTEGER", true);
        addForeignKey("food", "type", "food_def", "id");
        addColumn("food", "name", "VARCHAR(20)", false, "NULL");
        addColumn("food", "amount", "INTEGER", true, "1");
        addColumn("food", "condition_", "SMALLINT", true, "10");
        addColumn("food", "weight", "SMALLINT", false, "NULL");
        addColumn("food", "health", "SMALLINT", false, "NULL");
        addColumn("food", "mana", "SMALLINT", false, "NULL");

        createTable("food_own");
        addColumn("food_own", "object", "INTEGER", true);
        addForeignKey("food_own", "object", "food", "id");
        addColumn("food_own", "owner", "INTEGER", true);
        addForeignKey("food_own", "owner", "people", "id");

        createTable("food_inv");
        addColumn("food_inv", "object", "INTEGER", true);
        addForeignKey("food_inv", "object", "food", "id");
        addColumn("food_inv", "person", "INTEGER", true);
        addForeignKey("food_inv", "person", "people", "id");

        createTable("resource_def");
        addColumn("resource_def", "type", "VARCHAR(20)", true);

        createTable("resources");
        addColumn("resources", "x", "INTEGER", true);
        addColumn("resources", "y", "INTEGER", true);
        addForeignKey("resources", "x,y", "village", "x,y", "resource_location");
        addColumn("resources", "type", "INTEGER", true);
        addForeignKey("resources", "type", "resource_def", "id");
        addColumn("resources", "weight", "SMALLINT", true);
        addColumn("resources", "amount", "INTEGER", true, "1");

        createTable("resource_own");
        addColumn("resource_own", "object", "INTEGER", true);
        addForeignKey("resource_own", "object", "resources", "id");
        addColumn("resource_own", "owner", "INTEGER", true);
        addForeignKey("resource_own", "owner", "people", "id");

        createTable("resource_inv");
        addColumn("resource_inv", "object", "INTEGER", true);
        addForeignKey("resource_inv", "object", "resources", "id");
        addColumn("resource_inv", "person", "INTEGER", true);
        addForeignKey("resource_inv", "person", "people", "id");

        createTable("stuff_def");
        addColumn("stuff_def", "type", "VARCHAR(20)", true);

        createTable("stuff");
        addColumn("stuff", "x", "INTEGER", true);
        addColumn("stuff", "y", "INTEGER", true);
        addForeignKey("stuff", "x,y", "village", "x,y", "stuff_location");
        addColumn("stuff", "type", "INTEGER", true);
        addForeignKey("stuff", "type", "stuff_def", "id");
        addColumn("stuff", "weight", "SMALLINT", true);
        addColumn("stuff", "amount", "INTEGER", true, "1");

        createTable("stuff_own");
        addColumn("stuff_own", "object", "INTEGER", true);
        addForeignKey("stuff_own", "object", "stuff", "id");
        addColumn("stuff_own", "owner", "INTEGER", true);
        addForeignKey("stuff_own", "owner", "people", "id");

        createTable("stuff_inv");
        addColumn("stuff_inv", "object", "INTEGER", true);
        addForeignKey("stuff_inv", "object", "stuff", "id");
        addColumn("stuff_inv", "person", "INTEGER", true);
        addForeignKey("stuff_inv", "person", "people", "id");

        createTable("tool_def");
        addColumn("tool_def", "type", "VARCHAR(20)", true);

        createTable("tools");
        addColumn("tools", "x", "INTEGER", true);
        addColumn("tools", "y", "INTEGER", true);
        addForeignKey("tools", "x,y", "village", "x,y", "tools_location");
        addColumn("tools", "type", "INTEGER", true);
        addForeignKey("tools", "type", "tool_def", "id");
        addColumn("tools", "weight", "SMALLINT", true);
        addColumn("tools", "condition_", "SMALLINT", true, "10");

        createTable("tool_own");
        addColumn("tool_own", "object", "INTEGER", true);
        addForeignKey("tool_own", "object", "tools", "id");
        addColumn("tool_own", "owner", "INTEGER", true);
        addForeignKey("tool_own", "owner", "people", "id");

        createTable("tool_inv");
        addColumn("tool_inv", "object", "INTEGER", true);
        addForeignKey("tool_inv", "object", "tools", "id");
        addColumn("tool_inv", "person", "INTEGER", true);
        addForeignKey("tool_inv", "person", "people", "id");

        createTable("weapon_melee_def");
        addColumn("weapon_melee_def", "type", "INTEGER", true);
        addForeignKey("weapon_melee_def", "type", "weapon_melee_type_def", "id");
        addColumn("weapon_melee_def", "weight", "SMALLINT", true);
        addColumn("weapon_melee_def", "condition_", "SMALLINT", true, "10");
        addColumn("weapon_melee_def", "damage", "SMALLINT", true, "0");
        addColumn("weapon_melee_def", "strength", "SMALLINT", true, "0");

        createTable("weapons_melee");
        addColumn("weapons_melee", "x", "INTEGER", true);
        addColumn("weapons_melee", "y", "INTEGER", true);
        addForeignKey("weapons_melee", "x,y", "village", "x,y", "weapon_melee_location");
        addColumn("weapons_melee", "type", "INTEGER", true);
        addForeignKey("weapons_melee", "type", "weapon_melee_def", "id");
        addColumn("weapons_melee", "weight", "SMALLINT", true);
        addColumn("weapons_melee", "condition_", "SMALLINT", true, "10");

        createTable("weapon_melee_own");
        addColumn("weapon_melee_own", "object", "INTEGER", true);
        addForeignKey("weapon_melee_own", "object", "weapons_melee", "id");
        addColumn("weapon_melee_own", "owner", "INTEGER", true);
        addForeignKey("weapon_melee_own", "owner", "people", "id");

        createTable("weapon_melee_inv");
        addColumn("weapon_melee_inv", "object", "INTEGER", true);
        addForeignKey("weapon_melee_inv", "object", "weapons_melee", "id");
        addColumn("weapon_melee_inv", "person", "INTEGER", true);
        addForeignKey("weapon_melee_inv", "person", "people", "id");

        createTable("weapons_ranged");
        addColumn("weapons_ranged", "x", "INTEGER", true);
        addColumn("weapons_ranged", "y", "INTEGER", true);
        addForeignKey("weapons_ranged", "x,y", "village", "x,y", "weapon_ranged_location");
        addColumn("weapons_ranged", "type", "INTEGER", true);
        addForeignKey("weapons_ranged", "type", "weapon_ranged_def", "id");
        addColumn("weapons_ranged", "weight", "SMALLINT", true);
        addColumn("weapons_ranged", "condition_", "SMALLINT", true, "10");

        createTable("weapon_ranged_own");
        addColumn("weapon_ranged_own", "object", "INTEGER", true);
        addForeignKey("weapon_ranged_own", "object", "weapons_ranged", "id");
        addColumn("weapon_ranged_own", "owner", "INTEGER", true);
        addForeignKey("weapon_ranged_own", "owner", "people", "id");

        createTable("weapon_ranged_inv");
        addColumn("weapon_ranged_inv", "object", "INTEGER", true);
        addForeignKey("weapon_ranged_inv", "object", "weapons_ranged", "id");
        addColumn("weapon_ranged_inv", "person", "INTEGER", true);
        addForeignKey("weapon_ranged_inv", "person", "people", "id");

        createTable("relationships");
        addColumn("relationships", "person1", "INTEGER", true);
        addForeignKey("relationships", "person1", "people", "id");
        addColumn("relationships", "person2", "INTEGER", true);
        addForeignKey("relationships", "person2", "people", "id");
        addColumn("relationships", "relation", "INTEGER", true);
        addForeignKey("relationships", "relation", "relationship_type_def", "id");
        db.close();
    } else {
        qDebug() << "Failed to connect to database." << db.lastError().text();
        return false;
    }
    return true;
}

void Database::executeQuerylist(QList<QString> querylist, bool suppressErrors) {
    QSqlQuery qry;
    for (int k = 0; k < querylist.size(); k++) {
        QString query = querylist.at(k);
        if (query != "") {
            QStringList parts = query.split("|");
            if (parts.size() == 1 || parts.last() == "" || parts.last() == db_type){
                if (!qry.exec(parts.first())) {
                    if (!suppressErrors) qDebug() << qry.lastError() << query;
                    querylist.removeAt(k);
                    k--;
                }
            }
        }
    }
    if (sqlFileFormats != "") {
        QStringList formats = sqlFileFormats.split(",");
        foreach (QString format, formats){
            sqlFile = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QStringLiteral("/village") + format + QStringLiteral(".sql");
            QFile sql(sqlFile);
            sql.open(QFile::WriteOnly | QIODevice::Append);
            QTextStream out(&sql);
            foreach (QString query, querylist) {
                if (query != "") {
                    QStringList parts = query.split("|");
                    if (parts.size() == 1 || parts.last() == "" || parts.last() == format) {
                        if (parts.first() != "/") out << parts.first() << ";" << endl;
                        else out << parts.first() << endl;
                    }
                }
                else out << endl;
            }
        }
    }
}

bool Database::reset() {
    if (db.open()) {
        if (sqlFileFormats != "") {
            QStringList formats = sqlFileFormats.split(",");
            foreach (QString format, formats){
                sqlFile = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QStringLiteral("/village") + format + QStringLiteral(".sql");
                QFile sql(sqlFile); sql.open(QFile::WriteOnly | QFile::Text);
            }
        }
        QList<QString> querylist;
        QSqlQuery qry;
        qry.exec("SHOW TABLES");
        do {
            querylist.clear();
            while (qry.next()) querylist.append("DROP TABLE " + qry.value(0).toString());
            executeQuerylist(querylist, true);
            qry.exec("SHOW TABLES");
        } while(qry.size() > 0);
    } else {
        qDebug() << "Failed to connect to database." << db.lastError().text();
        return false;
    }
    return true;
}
