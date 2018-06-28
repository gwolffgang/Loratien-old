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
#include "river.h"

class Loratien {

private:
    // attributes
    MainWindow *window;
    Database *database;
    int worldWidth, worldHeight, hexesRadius, hexSize;
    bool worldEarthStyle;
    double percentOcean, percentMountain, percentMountainPeak;
    int worldAltMax, worldAltMin, worldRivers, worldTectonicPlates;
    QList<QList<Hex*>> *worldMap;
    QList<River*> *rivers;
    QList<Lake*> *lakes;
    QList<Hex*> *springs;
    //QList<City*>*cities;
    //QList<Village*> *villages;
    Group *group;
    Char *player;
    QList<Char*> *npcs;
    int amountNPCs;


public:
    // constructors
    explicit Loratien();

    // getter
    inline Database *getDatabase() {return database;}
    inline Group *getGroup() {return group;}
    inline int getHexSize() {return hexSize;}
    inline int getHexesRadius() {return hexesRadius;}
    inline QList<Lake*> *getLakes() {return lakes;}
    inline QList<Char*> *getNpcs() {return npcs;}
    double getPercentAlt(double percent);
    inline Char *getPlayer() {return player;}
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
};

#endif // LORATIEN_H
