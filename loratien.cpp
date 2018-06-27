#include "loratien.h"
#include "mainwindow.h"

extern Loratien *game;

Loratien::Loratien() : window(new MainWindow), database(NULL), worldWidth(200), worldHeight(200), worldEarthStyle(true),
    worldAltMax(10), worldAltMin(-10), worldRivers(worldWidth*worldHeight * 0.0005),
    worldTectonicPlates(10), worldMap(new QList<QList<Hex*>>),
    rivers(new QList<River*>), lakes(new QList<Lake*>), npcs(new QList<Char*>), group(new Group) /*,
    cities(new QList<City*>), villages(new QList<Village*>)*/ {

    if (worldEarthStyle && 1 == worldWidth%2) worldWidth++;

    // new seed for randomizer
    unsigned int seed = 23;
    if (seed != 0) srand(seed);
    else srand(unsigned(time(NULL)));

    // window settings
    window->show();
}

QList<River*> Loratien::getRivers(Hex *hex) {
    QList<River*> list;
    for (int i = 0; i < game->getRivers()->size(); i++) {
        River *current = game->getRivers()->at(i);
        for (int k = 0; k < current->getWatercourse()->size(); k++) {
            Hex *part = current->getWatercourse()->at(k);
            if (part->getCol() == hex->getCol() && part->getRow() == hex->getRow()) list.append(current);
        }
    }
    return list;
}

void Loratien::setupGame() {
    window->constructWorldMap();
    window->setupWorldMap();
    window->setupGui();
}
