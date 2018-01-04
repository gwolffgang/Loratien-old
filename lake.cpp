#include "lake.h"
#include "loratien.h"

extern Loratien *game;

Lake::Lake(Hex *hex) : name("Lake"), dimension(new QList<Hex*>) {
    hex->setLake(true);
    hex->removeRivers();
    dimension->append(hex);
    expandLake();
}

Hex *Lake::checkRiver(Hex *lowestHex) {
    Hex *currentHex = NULL;
    Hex *rivercheck = NULL;
    int hexCol = lowestHex->getCol();
    int hexRow = lowestHex->getRow();
    QList<Hex*> neighbors = game->getWindow()->getHexNeighbors(hexCol, hexRow);
    for (int k = 0; k < neighbors.size(); k++) {
        currentHex = neighbors.at(k);
        if (!currentHex->getLake() && !currentHex->getRiver() &&
            (!rivercheck || (currentHex->getAltitude() < rivercheck->getAltitude())))
            rivercheck = currentHex;
    }
    return rivercheck;
}

void Lake::expandLake() {
    bool ended = false;
    Hex *previousHex = NULL;
    do {
        Hex *currentHex = NULL;
        Hex *lowestHex = NULL;
        for (int i = 0; i < dimension->size(); i++) {
            int hexCol = dimension->at(i)->getCol();
            int hexRow = dimension->at(i)->getRow();
            QList<Hex*> neighbors = game->getWindow()->getHexNeighbors(hexCol, hexRow);
            for (int k = 0; k < neighbors.size(); k++) {
                currentHex = neighbors.at(k);
                if (!currentHex->getLake() && (!lowestHex || (currentHex->getAltitude() < lowestHex->getAltitude()))) {
                    lowestHex = currentHex;
                    previousHex = dimension->at(i);
                }
            }
        }
        Hex *rivercheck = checkRiver(lowestHex);
        if (rivercheck) {
            if (lowestHex->getAltitude() <= rivercheck->getAltitude()) {
                dimension->append(lowestHex);
                lowestHex->setLake(true);
                lowestHex->removeRivers();
            } else {
                River *newRiver;
                int lastCol = previousHex->getCol(), lastRow = previousHex->getRow();
                int nextCol = lowestHex->getCol(), nextRow = lowestHex->getRow();
                if (lastCol%2==1) {
                    if (nextCol < lastCol) {
                        if (nextRow == lastRow) newRiver = new River(previousHex, 6);
                        else newRiver = new River(previousHex, 5);
                    } else if (nextCol == lastCol) {
                        if (nextRow < lastRow) newRiver = new River(previousHex, 1);
                        else newRiver = new River(previousHex, 4);
                    } else {
                        if (nextRow == lastRow) newRiver = new River(previousHex, 2);
                        else newRiver = new River(previousHex, 3);
                    }
                } else {
                    if (nextCol < lastCol) {
                        if (nextRow < lastRow) newRiver = new River(previousHex, 6);
                        else newRiver = new River(previousHex, 5);
                    } else if (nextCol == lastCol) {
                        if (nextRow < lastRow) newRiver = new River(previousHex, 1);
                        else newRiver = new River(previousHex, 4);
                    } else {
                        if (nextRow < lastRow) newRiver = new River(previousHex, 2);
                        else newRiver = new River(previousHex, 3);
                    }
                }
                game->getRivers()->append(newRiver);
                ended = true;
            }
        } else ended = true;
    } while (!ended);
}
