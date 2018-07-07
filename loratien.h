#ifndef LORATIEN_H
#define LORATIEN_H

#include <QDesktopWidget>
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
#include "region.h"
#include "river.h"
#include "sector.h"

class Loratien {

public:
    // constructors
    explicit Loratien();

    // getter
    inline Database *getDatabase() {return database;}
    inline Group *getGroup() {return group;}
    inline int getHexSize() {return hexSize;}
    inline QList<Lake*> *getLakes() {return lakes;}
    inline QList<Char*> *getNpcs() {return npcs;}
    double getPercentAlt(double percent);
    inline Char *getPlayer() {return player;}
    inline int getPlayzoneRadius() {return playzoneRadius;}
    inline int getRegionHeight() {return regionHeight;}
    inline int getRegionRadius() {return regionRadius;}
    inline int getRegionWidth() {return regionWidth;}
    inline QList<River*> *getRivers() {return rivers;}
    QList<River*> getRivers(Hex *hex);
    inline int getSectorHeight() {return sectorHeight;}
    inline int getSectorRadius() {return sectorRadius;}
    inline int getSectorWidth() {return sectorWidth;}
    inline MainWindow *getWindow() {return window;}
    inline int getWorldAltMax() {return worldAltMax;}
    inline int getWorldAltMin() {return worldAltMin;}
    inline bool getWorldEarthStyle() {return worldEarthStyle;}
    inline int getWorldHeight() {return worldHeight;}
    inline QList<QList<Hex*>> *getWorldMap() {return worldMap;}
    inline int getWorldMapHeight() {return worldMapHeight;}
    inline int getWorldMapWidth() {return worldMapWidth;}
    inline QList<QList<Region*>> *getWorldRegions() {return worldRegions;}
    inline int getWorldRivers() {return worldRivers;}
    inline int getWorldTectonicPlates() {return worldTectonicPlates;}
    inline int getWorldWidth() {return worldWidth;}

    // setter
    inline void setDatabase(Database *newDatabase) {database = newDatabase;}
    inline void setHexSize(int newSize) {hexSize = newSize;}

    // methods
    void colorizePlates();
    void colorizeWorldMap();
    void constructWorldMap();
    void createNPCs();
    void createPlayerChar();
    void evaluateHexes();
    void generateWorldMap();
    void newGame();
    void placeCities();
    void placeRivers();
    void polishWorldMap();
    void setupWorldMap();
    void simulateTectonicMovement(int range = 2);
    void translateValuesToWorldMap();

private:
    // attributes
    MainWindow *window;
    Database *database;
    int playzoneRadius, regionRadius, sectorRadius, hexSize;
    int worldWidth, worldHeight, sectorWidth, sectorHeight, regionWidth, regionHeight;
    int worldMapWidth, worldMapHeight;
    bool worldEarthStyle;
    double percentOcean, percentMountain, percentMountainPeak;
    int worldAltMax, worldAltMin, worldRivers, worldTectonicPlates;
    QList<QList<Hex*>> *worldMap;
    QList<QList<Region*>> *worldRegions;
    QList<River*> *rivers;
    QList<Lake*> *lakes;
    QList<Hex*> *springs;
    //QList<City*>*cities;
    //QList<Village*> *villages;
    Group *group;
    Char *player;
    QList<Char*> *npcs;
    int amountNPCs;
};

#endif // LORATIEN_H
