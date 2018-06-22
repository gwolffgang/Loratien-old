#ifndef LORATIEN_H
#define LORATIEN_H

#include <QtSql>
#include <QtDebug>
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
    //QList<Bridge*> *bridges;
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
    inline QList<River*> *getRivers() {return rivers;}
    QList<River*> getRivers(int hexCol, int hexRow);
    inline MainWindow *getWindow() {return window;}
    inline int getWorldAltMax() {return worldAltMax;}
    inline int getWorldAltMin() {return worldAltMin;}
    inline bool getWorldEarthStyle() {return worldEarthStyle;}
    inline int getWorldHeight() {return worldHeight;}
    inline QList<QList<Hex*>> *getWorldMap() {return worldMap;}
    inline int getWorldRivers() {return worldRivers;}
    inline int getWorldTectonicPlates() {return worldTectonicPlates;}
    inline int getWorldWidth() {return worldWidth;}

    // methods
    void setupGame();
};

#endif // LORATIEN_H
