#ifndef LORATIEN_H
#define LORATIEN_H

#include <math.h>
#include <time.h>

#include "mainwindow.h"
#include "group.h"
#include "gui.h"
#include "hex.h"
#include "lake.h"
#include "river.h"

class Loratien {

private:
    // attributes
    MainWindow *window;
    int hexSize, worldWidth, worldHeight;
    bool worldEarthStyle;
    int worldAltMax, worldAltMin, worldRivers, worldTectonicPlates;
    QList<QList<Hex*>> *worldTectonicPlatesList;
    QList<QList<Hex*>> *worldMap;
    QList<River*> *rivers;
    QList<Lake*> *lakes;
    //QList<Bridge*> *bridges;
    //QList<City*>*cities;
    //QList<Village*> *villages;
    Group *group;

public:
    // constructors
    explicit Loratien();

    // getter
    inline int getHexSize() {return hexSize;}
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
    inline QList<QList<Hex*>> *getWorldTectonicPlatesList() {return worldTectonicPlatesList;}
    inline int getWorldWidth() {return worldWidth;}

    // methods
    void setupGame();
};

#endif // LORATIEN_H
