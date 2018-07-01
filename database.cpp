#include "database.h"
#include "loratien.h"

extern Loratien *game;

Database::Database(QString databaseType, QString hostName, QString userName, QString userPassword, QString databaseName) :
    db_type(databaseType), sqlFile(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QStringLiteral("/village") + databaseType + QStringLiteral(".sql")),
    saveInDatabase(true), saveInFile(true) {
    db = QSqlDatabase::addDatabase(db_type);
    db.setHostName(hostName);
    db.setPort(3306);
    db.setUserName(userName);
    db.setPassword(userPassword);
    db.setDatabaseName(databaseName);
    if (!reset()) qDebug() << "Cannot reset Database.";
    if (!create()) qDebug() << "Cannot fill Database.";
}

void Database::createSequence(QString tableName) {
    QString query;
    QList<QString> querylist;
    querylist.append("CREATE SEQUENCE " + tableName + "_sequence START WITH 1");
    query  = "CREATE OR REPLACE TRIGGER " + tableName + "_on_insert ";
    query += "BEFORE INSERT ON " + tableName;
    query += "  FOR EACH ROW ";
    query += "BEGIN ";
    query += "  SELECT " + tableName + "_sequence.NEXTVAL ";
    query += "  INTO :new.id ";
    query += "  FROM dual; ";
    query += "END; ";
    query += "/ ";
    querylist.append(query);
    if (saveInDatabase) {
        QSqlQuery qry;
        foreach (QString query, querylist) if (!qry.exec(query)) qDebug() << qry.lastError();
    }
    if (saveInFile) {
        QFile sql(sqlFile);
        sql.open(QFile::WriteOnly | QFile::Text);
        QTextStream out(&sql);
        foreach (QString query, querylist) out << query << endl;
    }
}

void Database::createTable(QString tableName, QString primaryKeys) {
    QString query;
    QList<QString> querylist;
    QStringList keys = primaryKeys.split(",");
    if (db_type == "QOCI") createSequence(tableName);
    query  = QString("CREATE TABLE " + tableName + "(");
    foreach (QString key, keys) {
        if (db_type == "QMYSQL" && keys.size() == 1) query += QString(key + " INTEGER AUTO_INCREMENT, ");
        else query += QString(key + " INTEGER, ");
    }
    query += QString("CONSTRAINT PK_" + tableName + " PRIMARY KEY(" + primaryKeys + "))");
    querylist.append("");
    querylist.append(query);
    executeQuerylist(querylist);
}

void Database::dropTable(QString tableName) {
    if (tableName == "all") reset();
    else {
        QList<QString> querylist;
        querylist.append("DROP TABLE " + tableName);
        executeQuerylist(querylist);
    }
}

void Database::addColumn(QString tableName, QString columnName, QString dataType, bool notNull, QString defaultValue) {
    QList<QString> querylist;
    QString query = "ALTER TABLE " + tableName + " ADD " + columnName + " ";
    if (dataType == "BOOL") {
        if (db_type == "QMYSQL") query += QString("BOOL");
        else if (db_type == "QOCI") query += QString("NUMBER(1,0)");
        else query += QString("TINYINT");
    } else query += dataType;
    if (defaultValue != "") query += " DEFAULT " + defaultValue;
    if (notNull) query += " NOT NULL";
    querylist.append(query);
    executeQuerylist(querylist);
}

void Database::addForeignKey(QString tableName, QString columnName, QString linkedTable, QString linkedColumn, QString altName) {
    QList<QString> querylist;
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
        qry.exec("SELECT name FROM first_names_def WHERE gender = " + QString::number(gender));
        db.close();
        int randomName = rand()%qry.size();
        while (qry.next()) if (randomName == qry.at()) return qry.value(0).toString();
    } else qDebug() << "Failed to connect to database." << db.lastError().text();
    return "";
}

void Database::saveBuildingDefs() {
    QList<QString> querylist;
    querylist.append("INSERT INTO building_def(name, type, size_, max_people, max_worker) VALUES('hut',(SELECT id FROM building_type_def WHERE type='residence'),1,2,0)");
    querylist.append("INSERT INTO building_def(name, type, size_, max_people, max_worker) VALUES('house',(SELECT id FROM building_type_def WHERE type='residence'),2,4,0)");
    querylist.append("INSERT INTO building_def(name, type, size_, max_people, max_worker) VALUES('greathouse',(SELECT id FROM building_type_def WHERE type='residence'),3,8,0)");
    querylist.append("INSERT INTO building_def(name, type, size_, max_people, max_worker) VALUES('residence',(SELECT id FROM building_type_def WHERE type='residence'),4,12,0)");
    querylist.append("INSERT INTO building_def(name, type, size_, max_people, max_worker) VALUES('farmhouse',(SELECT id FROM building_type_def WHERE type='farm'),3,7,7)");
    querylist.append("INSERT INTO building_def(name, type, size_, max_people, max_worker) VALUES('farm',(SELECT id FROM building_type_def WHERE type='farm'),4,12,12)");
    querylist.append("INSERT INTO building_def(name, type, size_, max_people, max_worker) VALUES('shrine',(SELECT id FROM building_type_def WHERE type='temple'),1,0,1)");
    querylist.append("INSERT INTO building_def(name, type, size_, max_people, max_worker) VALUES('temple',(SELECT id FROM building_type_def WHERE type='temple'),2,2,4)");
    querylist.append("INSERT INTO building_def(name, type, size_, max_people, max_worker) VALUES('greattemple',(SELECT id FROM building_type_def WHERE type='temple'),3,4,8)");
    executeQuerylist(querylist);
}

void Database::saveBuildingTypeDefs() {
    QList<QString> querylist;
    querylist.append("INSERT INTO building_type_def(type) VALUES('residence')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('farm')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('barn')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('stable')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('pigsty')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('cowshed')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('henhouse')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('tavern')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('hunting lodge')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('forge')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('temple')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('well')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('lumberjack hut')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('forester hut')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('quarry')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('weaving')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('mill')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('prison')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('market')");
    querylist.append("INSERT INTO building_type_def(type) VALUES('town hall')");
    executeQuerylist(querylist);
}

void Database::saveCharacter(Char *c) {
    if (db.open()) {
        QSqlQuery qry_name;
        QList<QString> querylist;
        int name_id;
        qry_name.exec("SELECT id FROM first_names_def WHERE name='"+c->getFirstName()+"'");
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

void Database::saveFieldDefs() {
    QList<QString> querylist;
    querylist.append("INSERT INTO fields_def(type, passable) VALUES('plain', 1)");
    querylist.append("INSERT INTO fields_def(type, passable) VALUES('field', 1)");
    querylist.append("INSERT INTO fields_def(type, passable) VALUES('grass', 1)");
    querylist.append("INSERT INTO fields_def(type, passable) VALUES('building', 1)");
    querylist.append("INSERT INTO fields_def(type, passable) VALUES('forest', 1)");
    querylist.append("INSERT INTO fields_def(type, passable) VALUES('swamp', 1)");
    querylist.append("INSERT INTO fields_def(type, passable) VALUES('bridge', 1)");
    querylist.append("INSERT INTO fields_def(type, passable) VALUES('river', 0)");
    querylist.append("INSERT INTO fields_def(type, passable) VALUES('lake', 0)");
    executeQuerylist(querylist);
}

void Database::saveNames(int gender) {
    QFile names;
    if (gender == 1) names.setFileName(":/data/lists/names_male.list");
    else names.setFileName(":/data/lists/names_female.list");
    if (names.open(QIODevice::ReadOnly)) {
        QTextStream in(&names);
        QStringList list = in.readAll().split('\n');
        QList<QString> querylist;
        foreach(QString name, list) {
            if (name != "")
                querylist.append("INSERT INTO first_names_def(name, gender) VALUES('" + name + "', " + QString::number(gender) + ")");
        }
        names.close();
        executeQuerylist(querylist);
    }
}

void Database::saveProduction() {
    QString query;
    QList<QString> querylist;
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'farmhouse'),(SELECT id FROM production_type_def WHERE name = 'food'), 5, 0, 1, 0, 0, 0, 0, 0, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'farmhouse'),(SELECT id FROM production_type_def WHERE name = 'food'), 5, 0, 1, 0, 0, 0, 0, 0, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'shrine'),(SELECT id FROM production_type_def WHERE name = 'belief'), 5, 0, 0, 0, 0, 0, 1, 0, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'shrine'),(SELECT id FROM production_type_def WHERE name = 'belief'), 7, 0, 0, 0, 0, 0, 1, 0, 1)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'shrine'),(SELECT id FROM production_type_def WHERE name = 'belief'), 7, 0, 0, 0, 0, 0, 1, 1, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'shrine'),(SELECT id FROM production_type_def WHERE name = 'belief'), 9, 0, 0, 0, 0, 0, 1, 1, 1)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'shrine'),(SELECT id FROM production_type_def WHERE name = 'influence'), 1, 0, 0, 0, 2, 0, 0, 0, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'shrine'),(SELECT id FROM production_type_def WHERE name = 'influence'), 2, 0, 0, 0, 1, 0, 0, 0, 1)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'shrine'),(SELECT id FROM production_type_def WHERE name = 'influence'), 2, 0, 0, 0, 1, 0, 0, 1, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'shrine'),(SELECT id FROM production_type_def WHERE name = 'influence'), 3, 0, 0, 0, 1, 0, 0, 1, 1)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'temple'),(SELECT id FROM production_type_def WHERE name = 'belief'), 7, 0, 0, 0, 0, 0, 1, 0, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'temple'),(SELECT id FROM production_type_def WHERE name = 'belief'), 9, 0, 0, 0, 0, 0, 1, 0, 1)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'temple'),(SELECT id FROM production_type_def WHERE name = 'belief'), 9, 0, 0, 0, 0, 0, 1, 1, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'temple'),(SELECT id FROM production_type_def WHERE name = 'belief'), 11, 0, 0, 0, 0, 0, 1, 1, 1)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'temple'),(SELECT id FROM production_type_def WHERE name = 'influence'), 2, 0, 0, 0, 2, 0, 0, 0, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'temple'),(SELECT id FROM production_type_def WHERE name = 'influence'), 3, 0, 0, 0, 1, 0, 0, 0, 1)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'temple'),(SELECT id FROM production_type_def WHERE name = 'influence'), 3, 0, 0, 0, 1, 0, 0, 1, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'temple'),(SELECT id FROM production_type_def WHERE name = 'influence'), 5, 0, 0, 0, 1, 0, 0, 1, 1)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'greattemple'),(SELECT id FROM production_type_def WHERE name = 'belief'), 9, 0, 0, 0, 0, 0, 1, 0, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'greattemple'),(SELECT id FROM production_type_def WHERE name = 'belief'), 11, 0, 0, 0, 0, 0, 1, 0, 1)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'greattemple'),(SELECT id FROM production_type_def WHERE name = 'belief'), 11, 0, 0, 0, 0, 0, 1, 1, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'greattemple'),(SELECT id FROM production_type_def WHERE name = 'belief'), 14, 0, 0, 0, 0, 0, 1, 1, 1)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'greattemple'),(SELECT id FROM production_type_def WHERE name = 'influence'), 3, 0, 0, 0, 2, 0, 0, 0, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'greattemple'),(SELECT id FROM production_type_def WHERE name = 'influence'), 4, 0, 0, 0, 1, 0, 0, 0, 1)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'greattemple'),(SELECT id FROM production_type_def WHERE name = 'influence'), 4, 0, 0, 0, 1, 0, 0, 1, 0)";
    querylist.append(query);
    query  = "INSERT INTO production(building, type, amount, academics, crafts, medicine, politics, science, spirituality, persuation, subterfuge)";
    query += "VALUES((SELECT id FROM building_def WHERE name = 'greattemple'),(SELECT id FROM production_type_def WHERE name = 'influence'), 6, 0, 0, 0, 1, 0, 0, 1, 1)";
    querylist.append(query);
    executeQuerylist(querylist);
}

void Database::saveProductionTypeDefs() {
    QList<QString> querylist;
    querylist.append("INSERT INTO production_type_def (name) VALUES ('food')");
    querylist.append("INSERT INTO production_type_def (name) VALUES ('drink')");
    querylist.append("INSERT INTO production_type_def (name) VALUES ('alcohol')");
    querylist.append("INSERT INTO production_type_def (name) VALUES ('influence')");
    querylist.append("INSERT INTO production_type_def (name) VALUES ('belief')");
    querylist.append("INSERT INTO production_type_def (name) VALUES ('tool')");
    querylist.append("INSERT INTO production_type_def (name) VALUES ('melee weapon')");
    querylist.append("INSERT INTO production_type_def (name) VALUES ('ranged weapon')");
    querylist.append("INSERT INTO production_type_def (name) VALUES ('wood')");
    querylist.append("INSERT INTO production_type_def (name) VALUES ('stone')");
    executeQuerylist(querylist);
}

void Database::saveVillageMap() {
    QList<QString> querylist;
    for (int x = 0; x < game->getWorldWidth(); x++) {
        for (int y = 0; y < game->getWorldHeight(); y++) {
            querylist.append("INSERT INTO village_map(x, y) VALUES("+ QString::number(x) + ", " + QString::number(y) +")");
        }
    }
    executeQuerylist(querylist);
}

bool Database::create() {
    if (db.open()) {
        reset();

        createTable("fields_def");
        addColumn("fields_def", "type", "VARCHAR(10)", true);
        addColumn("fields_def", "passable", "BOOL", true, "1");
        saveFieldDefs();

        createTable("village_map", "x,y");
        addColumn("village_map", "fieldtype", "INTEGER", true, "1");
        addForeignKey("village_map", "fieldtype", "fields_def", "id");
        saveVillageMap();

        createTable("first_names_def");
        addColumn("first_names_def", "name", "VARCHAR(20)", true);
        addColumn("first_names_def", "gender", "BOOL", true, "1");
        saveNames(1);
        saveNames(0);

        createTable("people");
        addColumn("people", "x", "INTEGER", true);
        addColumn("people", "y", "INTEGER", true);
        addForeignKey("people", "x,y", "village_map", "x,y", "people_location");
        /*addColumn("people", "last_name", "INTEGER", true);
        addForeignKey("people", "last_name", "first_names_def", "id");*/
        addColumn("people", "last_name", "VARCHAR(20)", true);
        addColumn("people", "first_name", "INTEGER", true);
        addForeignKey("people", "first_name", "first_names_def", "id");
        addColumn("people", "gender", "BOOL", true, "1");
        addColumn("people", "age", "TINYINT", true);
        addColumn("people", "birthday", "VARCHAR(11)", true);
        addColumn("people", "day_of_death", "VARCHAR(11)", false, "NULL");
        addColumn("people", "concept", "VARCHAR(10)", false, "NULL");
        addColumn("people", "virtue", "VARCHAR(10)", false, "NULL");
        addColumn("people", "vice", "VARCHAR(10)", false, "NULL");
        addColumn("people", "size_", "TINYINT", true, "5");
        addColumn("people", "weight", "SMALLINT", true, "70");
        addColumn("people", "defense", "TINYINT", true, "2");
        addColumn("people", "speed", "TINYINT", true, "7");
        addColumn("people", "initiative", "TINYINT", true, "2");
        addColumn("people", "health", "TINYINT", true, "6");
        addColumn("people", "health_current", "TINYINT", true, "6");
        addColumn("people", "mana", "TINYINT", false, "NULL");
        addColumn("people", "mana_current", "TINYINT", false, "NULL");
        addColumn("people", "willpower", "TINYINT", true, "2");
        addColumn("people", "willpower_current", "TINYINT", true, "2");
        addColumn("people", "intelligence", "TINYINT", true, "1");
        addColumn("people", "wits", "TINYINT", true, "1");
        addColumn("people", "resolve", "TINYINT", true, "1");
        addColumn("people", "strength", "TINYINT", true, "1");
        addColumn("people", "dexterity", "TINYINT", true, "1");
        addColumn("people", "stamina", "TINYINT", true, "1");
        addColumn("people", "presence", "TINYINT", true, "1");
        addColumn("people", "manipulation", "TINYINT", true, "1");
        addColumn("people", "composure", "TINYINT", true, "1");
        addColumn("people", "academics", "TINYINT", true, "-3");
        addColumn("people", "crafts", "TINYINT", true, "-3");
        addColumn("people", "investigation", "TINYINT", true, "-3");
        addColumn("people", "magic", "TINYINT", true, "-3");
        addColumn("people", "medicine", "TINYINT", true, "-3");
        addColumn("people", "politics", "TINYINT", true, "-3");
        addColumn("people", "science", "TINYINT", true, "-3");
        addColumn("people", "spirituality", "TINYINT", true, "-3");
        addColumn("people", "athletics", "TINYINT", true, "-1");
        addColumn("people", "brawl", "TINYINT", true, "-1");
        addColumn("people", "riding", "TINYINT", true, "-1");
        addColumn("people", "ranged_combat", "TINYINT", true, "-1");
        addColumn("people", "larceny", "TINYINT", true, "-1");
        addColumn("people", "stealth", "TINYINT", true, "-1");
        addColumn("people", "survival", "TINYINT", true, "-1");
        addColumn("people", "weaponry", "TINYINT", true, "-1");
        addColumn("people", "animal_ken", "TINYINT", true, "-1");
        addColumn("people", "empathy", "TINYINT", true, "-1");
        addColumn("people", "expression", "TINYINT", true, "-1");
        addColumn("people", "intimidation", "TINYINT", true, "-1");
        addColumn("people", "persuation", "TINYINT", true, "-1");
        addColumn("people", "socialize", "TINYINT", true, "-1");
        addColumn("people", "streetwise", "TINYINT", true, "-1");
        addColumn("people", "subterfuge", "TINYINT", true, "-1");

        createTable("building_type_def");
        addColumn("building_type_def", "type", "VARCHAR(20)", 1);
        saveBuildingTypeDefs();

        createTable("building_def");
        addColumn("building_def", "name", "VARCHAR(20)", true);
        addColumn("building_def", "type", "INTEGER", true, "1");
        addForeignKey("building_def", "type", "building_type_def", "id");
        addColumn("building_def", "size_", "TINYINT", true, "2");
        addColumn("building_def", "max_people", "TINYINT", true, "0");
        addColumn("building_def", "max_worker", "TINYINT", true, "0");
        saveBuildingDefs();

        createTable("buildings");
        addColumn("buildings", "x", "INTEGER", true);
        addColumn("buildings", "y", "INTEGER", true);
        addForeignKey("buildings", "x,y", "village_map", "x,y", "buildings_location");
        addColumn("buildings", "building", "INTEGER", true);
        addForeignKey("buildings", "building", "building_def", "id");
        addColumn("buildings", "condition_", "TINYINT", true, "100");

        createTable("building_own");
        addColumn("building_own", "building", "INTEGER", true);
        addForeignKey("building_own", "building", "buildings", "id");
        addColumn("building_own", "owner", "INTEGER", true);
        addForeignKey("building_own", "owner", "people", "id");

        createTable("production_type_def");
        addColumn("production_type_def", "name", "VARCHAR(20)", true);
        saveProductionTypeDefs();

        createTable("production");
        addColumn("production", "building", "INTEGER", true);
        addForeignKey("production", "building", "building_def", "id");
        addColumn("production", "type", "INTEGER", true);
        addForeignKey("production", "type", "production_type_def", "id");
        addColumn("production", "amount", "TINYINT", true, "0");
        addColumn("production", "academics", "TINYINT", true, "0");
        addColumn("production", "crafts", "TINYINT", true, "0");
        addColumn("production", "medicine", "TINYINT", true, "0");
        addColumn("production", "politics", "TINYINT", true, "0");
        addColumn("production", "science", "TINYINT", true, "0");
        addColumn("production", "spirituality", "TINYINT", true, "0");
        addColumn("production", "persuation", "TINYINT", true, "0");
        addColumn("production", "subterfuge", "TINYINT", true, "0");
        saveProduction();

        createTable("animal_def");
        addColumn("animal_def","type","VARCHAR(20)",true);

        createTable("animals");
        addColumn("animals", "x", "INTEGER", true);
        addColumn("animals", "y", "INTEGER", true);
        addForeignKey("animals", "x,y", "village_map", "x,y", "animals_location");
        addColumn("animals", "type", "INTEGER", true);
        addForeignKey("animals", "type", "animal_def", "id");
        addColumn("animals", "name", "VARCHAR(20)", true, "''");
        addColumn("animals", "birthday", "VARCHAR(11)", true);
        addColumn("animals", "day_of_death", "VARCHAR(11)", false, "NULL");
        addColumn("animals","size_","TINYINT",true);
        addColumn("animals", "weight", "SMALLINT", true);
        addColumn("animals","defense", "TINYINT", true);
        addColumn("animals", "speed", "TINYINT", true);
        addColumn("animals", "initiative", "TINYINT", true);
        addColumn("animals","health", "TINYINT", true);
        addColumn("animals","health_current", "TINYINT", true);
        addColumn("animals","mana", "TINYINT", false, "NULL");
        addColumn("animals","mana_current", "TINYINT", false, "NULL");
        addColumn("animals", "intelligence", "TINYINT", true, "1");
        addColumn("animals", "wits", "TINYINT", true, "1");
        addColumn("animals", "resolve", "TINYINT", true, "1");
        addColumn("animals", "strength", "TINYINT", true, "1");
        addColumn("animals", "dexterity", "TINYINT", true, "1");
        addColumn("animals", "stamina", "TINYINT", true, "1");
        addColumn("animals", "presence", "TINYINT", true, "1");
        addColumn("animals", "manipulation", "TINYINT", true, "1");
        addColumn("animals", "composure", "TINYINT", true, "1");
        addColumn("animals", "athletics", "TINYINT", true, "-1");
        addColumn("animals", "brawl", "TINYINT", true, "-1");
        addColumn("animals", "stealth", "TINYINT", true, "-1");
        addColumn("animals", "survival", "TINYINT", true, "-1");
        addColumn("animals", "intimidation", "TINYINT", true, "-1");

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

        createTable("cloth_type_def");
        addColumn("cloth_type_def", "type", "VARCHAR(20)", true);

        createTable("cloth_def");
        addColumn("cloth_def", "type", "INTEGER", true);
        addForeignKey("cloth_def", "type", "cloth_type_def", "id");
        addColumn("cloth_def", "weight", "SMALLINT", true);
        addColumn("cloth_def", "condition_", "TINYINT", true, "10");
        addColumn("cloth_def", "defense", "TINYINT", true, "0");
        addColumn("cloth_def", "speed", "TINYINT", true, "0");
        addColumn("cloth_def", "strength", "TINYINT", true, "0");

        createTable("cloths");
        addColumn("cloths", "x", "INTEGER", true);
        addColumn("cloths", "y", "INTEGER", true);
        addForeignKey("cloths", "x,y", "village_map", "x,y", "cloths_location");
        addColumn("cloths", "type", "INTEGER", true);
        addForeignKey("cloths", "type", "cloth_def", "id");
        addColumn("cloths", "weight", "SMALLINT", true);
        addColumn("cloths", "name", "VARCHAR(20)", true, "''");
        addColumn("cloths", "condition_", "TINYINT", true, "10");
        addColumn("cloths", "defense", "TINYINT", true, "0");
        addColumn("cloths", "speed", "TINYINT", true, "0");
        addColumn("cloths", "strength", "TINYINT", true, "0");

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
        addColumn("food_def", "type", "VARCHAR(20)", true);

        createTable("food");
        addColumn("food", "x", "INTEGER", true);
        addColumn("food", "y", "INTEGER", true);
        addForeignKey("food", "x,y", "village_map", "x,y", "food_location");
        addColumn("food", "type", "INTEGER", true);
        addForeignKey("food", "type", "food_def", "id");
        addColumn("food", "weight", "SMALLINT", true);
        addColumn("food", "name", "VARCHAR(20)", true, "''");
        addColumn("food", "amount", "INTEGER", true, "1");
        addColumn("food", "condition_", "TINYINT", true, "10");
        addColumn("food", "health", "TINYINT", true, "0");
        addColumn("food", "mana", "TINYINT", true, "0");

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
        addForeignKey("resources", "x,y", "village_map", "x,y", "resource_location");
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
        addForeignKey("stuff", "x,y", "village_map", "x,y", "stuff_location");
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
        addForeignKey("tools", "x,y", "village_map", "x,y", "tools_location");
        addColumn("tools", "type", "INTEGER", true);
        addForeignKey("tools", "type", "tool_def", "id");
        addColumn("tools", "weight", "SMALLINT", true);
        addColumn("tools", "condition_", "TINYINT", true, "10");

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

        createTable("weapon_melee_type_def");
        addColumn("weapon_melee_type_def", "type", "VARCHAR(20)", true);

        createTable("weapon_melee_def");
        addColumn("weapon_melee_def", "type", "INTEGER", true);
        addForeignKey("weapon_melee_def", "type", "weapon_melee_type_def", "id");
        addColumn("weapon_melee_def", "weight", "SMALLINT", true);
        addColumn("weapon_melee_def", "condition_", "TINYINT", true, "10");
        addColumn("weapon_melee_def", "damage", "TINYINT", true, "0");
        addColumn("weapon_melee_def", "strength", "TINYINT", true, "0");

        createTable("weapons_melee");
        addColumn("weapons_melee", "x", "INTEGER", true);
        addColumn("weapons_melee", "y", "INTEGER", true);
        addForeignKey("weapons_melee", "x,y", "village_map", "x,y", "weapon_melee_location");
        addColumn("weapons_melee", "type", "INTEGER", true);
        addForeignKey("weapons_melee", "type", "weapon_melee_def", "id");
        addColumn("weapons_melee", "weight", "SMALLINT", true);
        addColumn("weapons_melee", "condition_", "TINYINT", true, "10");

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

        createTable("weapon_ranged_type_def");
        addColumn("weapon_ranged_type_def", "type", "VARCHAR(20)", true);

        createTable("weapon_ranged_def");
        addColumn("weapon_ranged_def", "type", "INTEGER", true);
        addForeignKey("weapon_ranged_def", "type", "weapon_ranged_type_def", "id");
        addColumn("weapon_ranged_def", "range_", "SMALLINT", true);
        addColumn("weapon_ranged_def", "weight", "SMALLINT", true);
        addColumn("weapon_ranged_def", "condition_", "TINYINT", true, "10");
        addColumn("weapon_ranged_def", "damage", "TINYINT", true, "0");
        addColumn("weapon_ranged_def", "strength", "TINYINT", true, "0");

        createTable("weapons_ranged");
        addColumn("weapons_ranged", "x", "INTEGER", true);
        addColumn("weapons_ranged", "y", "INTEGER", true);
        addForeignKey("weapons_ranged", "x,y", "village_map", "x,y", "weapon_ranged_location");
        addColumn("weapons_ranged", "type", "INTEGER", true);
        addForeignKey("weapons_ranged", "type", "weapon_ranged_def", "id");
        addColumn("weapons_ranged", "weight", "SMALLINT", true);
        addColumn("weapons_ranged", "condition_", "TINYINT", true, "10");

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

        createTable("relationship_type_def");
        addColumn("relationship_type_def", "type", "VARCHAR(20)", true);

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

void Database::executeQuerylist(QList<QString> querylist) {
    if (saveInDatabase) {
        QSqlQuery qry;
        foreach (QString query, querylist) {
            if (query != "") {
                if (!qry.exec(query)) qDebug() << qry.lastError();
            }
        }
    }
    if (saveInFile) {
        QFile sql(sqlFile);
        sql.open(QFile::WriteOnly | QIODevice::Append);
        QTextStream out(&sql);
        foreach (QString query, querylist) {
            if (query != "") out << query << ";" << endl;
            else out << endl;
        }
    }
}

bool Database::reset() {
    if (db.open()) {
        QList<QString> querylist;
        querylist.append("DROP DATABASE Loratien");
        querylist.append("CREATE DATABASE Loratien");
        querylist.append("USE Loratien");
        if (saveInDatabase) {
            QSqlQuery qry;
            foreach (QString query, querylist) if (!qry.exec(query)) qDebug() << qry.lastError();
        }
        if (saveInFile) {
            QFile sql(sqlFile);
            sql.open(QFile::WriteOnly | QFile::Text);
            QTextStream out(&sql);
            foreach (QString query, querylist) out << query << ";" << endl;
        }
    } else {
        qDebug() << "Failed to connect to database." << db.lastError().text();
        return false;
    }
    return true;
}
