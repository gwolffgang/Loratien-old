#ifndef LORATIEN_H
#define LORATIEN_H

#include <QtSql>
#include <QtDebug>
#include <QPainter>
#include <QMouseEvent>
#include <math.h>
#include <time.h>

#include "mainwindow.h"
#include "database.h"
#include "char.h"
#include "group.h"
#include "gui.h"
#include "hex.h"
#include "lake.h"
#include "river.h"

class Loratien {

private:
    // attributes
    MainWindow *window;
    Database *database;
    int worldWidth, worldHeight;
    bool worldEarthStyle;
    int worldAltMax, worldAltMin, worldRivers, worldTectonicPlates;
    QList<QList<Hex*>> *worldMap;
    QList<River*> *rivers;
    QList<Lake*> *lakes;
    //QList<City*>*cities;
    //QList<Village*> *villages;
    QList<Char*> *npcs;
    Group *group;

public:
    // constructors
    explicit Loratien();

    // getter
    inline Database *getDatabase() {return database;}
    inline QList<Lake*> *getLakes() {return lakes;}
    inline QList<Char*> *getNpcs() {return npcs;}
    inline QList<River*> *getRivers() {return rivers;}
    QList<River*> getRivers(Hex *hex);
    inline MainWindow *getWindow() {return window;}
    inline int getWorldAltMax() {return worldAltMax;}
    inline int getWorldAltMin() {return worldAltMin;}
    inline bool getWorldEarthStyle() {return worldEarthStyle;}
    inline int getWorldHeight() {return worldHeight;}
    inline QList<QList<Hex*>> *getWorldMap() {return worldMap;}
    inline int getWorldRivers() {return worldRivers;}
    inline int getWorldTectonicPlates() {return worldTectonicPlates;}
    inline int getWorldWidth() {return worldWidth;}

    // setter
    inline void setDatabase(Database *newDatabase) {database = newDatabase;}

    // methods
    void setupGame();
};

#endif // LORATIEN_H
