#include "loratien.h"
#include "mainwindow.h"

extern Loratien *game;

Loratien::Loratien() : window(new MainWindow), worldWidth(90), worldHeight(60), worldEarthStyle(true),
    worldAltMax(5), worldAltMin(-4),
    worldMountains(30), worldOceans(10), worldRivers(worldMountains * 3),
    worldPropPeak(1), worldPropMountainHigh(2), worldPropMountainLow(3),
    worldPropPlainHigh(10), worldPropPlain(13), worldPropPlainLow(1),
    worldPropWaterCoast(1), worldPropWaterDeep(4), worldPropOceanLow(10), worldPropOceanDeep(55),
    worldAltitudesList(new QList<int>), worldMap(new QList<QList<Hex*>>),
    rivers(new QList<River*>), lakes(new QList<Lake*>), group(new Group) /*,
    bridges(new QList<Bridge*>), cities(new QList<City*>), villages(new QList<Village*>)*/ {

    // new seed for randomizer
    unsigned int seed = 0;
    if (seed != 0) srand(seed);
    else srand(unsigned(time(NULL)));

    // window settings
    window->show();

    fillAltitudesList();
}

QList<River*> Loratien::getRivers(int hexCol, int hexRow) {
    QList<River*> list;
    for (int i = 0; i < game->getRivers()->size(); i++) {
        River *current = game->getRivers()->at(i);
        for (int k = 0; k < current->getWatercourse()->size(); k++) {
            Hex *part = current->getWatercourse()->at(k);
            if (part->getCol() == hexCol && part->getRow() == hexRow) list.append(current);
        }
    }
    return list;
}

void Loratien::fillAltitudesList() {
    int worldPropAll = worldPropPeak + worldPropMountainHigh + worldPropMountainLow + worldPropPlainHigh + worldPropPlainLow + worldPropWaterCoast + worldPropWaterDeep + worldPropOceanLow + worldPropOceanDeep;
    if (worldPropAll == 0) worldPropAll = worldPropPlainLow = 1;
    double worldHexes = (double) worldHeight * (double) worldWidth / (double) worldPropAll;
    for (int i = 0; i < worldHexes * worldPropPeak; i++) worldAltitudesList->append(5);
    for (int i = 0; i < worldHexes * worldPropMountainHigh; i++) worldAltitudesList->append(4);
    for (int i = 0; i < worldHexes * worldPropMountainLow; i++) worldAltitudesList->append(3);
    for (int i = 0; i < worldHexes * worldPropPlainHigh; i++) worldAltitudesList->append(2);
    for (int i = 0; i < worldHexes * worldPropPlain; i++) worldAltitudesList->append(1);
    for (int i = 0; i < worldHexes * worldPropPlainLow; i++) worldAltitudesList->append(0);
    for (int i = 0; i < worldHexes * worldPropWaterCoast; i++) worldAltitudesList->append(-1);
    for (int i = 0; i < worldHexes * worldPropWaterDeep; i++) worldAltitudesList->append(-2);
    for (int i = 0; i < worldHexes * worldPropOceanLow; i++) worldAltitudesList->append(-3);
    for (int i = 0; i < worldHexes * worldPropOceanDeep; i++) worldAltitudesList->append(-4);
    for (int i = 0; i < worldWidth * worldHeight - worldAltitudesList->size(); i++) worldAltitudesList->append(-4);
}

void Loratien::setupGame() {
    window->constructWorldMap();
    window->setupWorldMap();
    //window->setupHUD();
}
