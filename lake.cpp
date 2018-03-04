#include "lake.h"
#include "loratien.h"

extern Loratien *game;

Lake::Lake(Hex *hex) : name("Lake"), dimension(new QList<Hex*>) {
    hex->setLake(true);
    hex->removeRivers();
    dimension->append(hex);
    expandLake();
}

bool Lake::addExistingLakeOf(Hex *hex) {
    QList<Lake*> *lakes = game->getLakes();
    Lake *lake = NULL;
    QList<Hex*> *lakeDimension = NULL;
    for (int k = 0; k < lakes->size(); k++) {
        lake = lakes->at(k);
        lakeDimension = lake->getDimension();
        for (int part = 0; part < lakeDimension->size(); part++) {
            if (lakeDimension->at(part) == hex) {
                for (int m = 0; m < lakeDimension->size(); m++) dimension->append(lakeDimension->at(m));
                delete lake;
                lakes->removeAt(k);
                return true;
            }
        }
    }
    return false;
}

Hex *Lake::checkRiver(Hex *lowestHex) {
    Hex *rivercheck = NULL;
    foreach (Hex* neighbor, game->getWindow()->getHexNeighbors(lowestHex->getCol(), lowestHex->getRow()))
        if (!neighbor->getLake() && (!rivercheck || neighbor->getAltitude() < rivercheck->getAltitude())) rivercheck = neighbor;
    return rivercheck;
}

void Lake::expandLake() {
    bool ended = false;
    int lakeSize = 1;
    Hex *currentHex = NULL;
    Hex *lowestHex = NULL;
    Hex *riverCheck = NULL;
    do {
        lakeSize = dimension->size();
        lowestHex = NULL;
        for (int part = 0; part < lakeSize; part++) {
            currentHex = dimension->at(part);
            int hexCol = currentHex->getCol();
            int hexRow = currentHex->getRow();
            foreach (Hex *neighbor, game->getWindow()->getHexNeighbors(hexCol, hexRow))
                if (neighbor->getLake()) {
                    if (!isAlreadyInThisLake(neighbor) && addExistingLakeOf(neighbor)) {
                        lakeSize = dimension->size();
                        part = -1;
                    }
                } else if (!lowestHex || neighbor->getAltitude() < lowestHex->getAltitude()) lowestHex = neighbor;
        }
        riverCheck = checkRiver(lowestHex);
        if (riverCheck) {
            dimension->append(lowestHex);
            lowestHex->setLake(true);
            lowestHex->removeRivers();
            if (lowestHex->getAltitude() > riverCheck->getAltitude()) {
                dimension->append(lowestHex);
                lowestHex->setLake(true);
                lowestHex->removeRivers();
                River *newRiver = NULL;
                int lastCol = lowestHex->getCol(), lastRow = lowestHex->getRow();
                int nextCol = riverCheck->getCol(), nextRow = riverCheck->getRow();
                if (lastCol%2==1) {
                    if (nextCol < lastCol) {
                        if (nextRow == lastRow) newRiver = new River(lowestHex, 6);
                        else newRiver = new River(lowestHex, 5);
                    } else if (nextCol == lastCol) {
                        if (nextRow < lastRow) newRiver = new River(lowestHex, 1);
                        else newRiver = new River(lowestHex, 4);
                    } else {
                        if (nextRow == lastRow) newRiver = new River(lowestHex, 2);
                        else newRiver = new River(lowestHex, 3);
                    }
                } else {
                    if (nextCol < lastCol) {
                        if (nextRow < lastRow) newRiver = new River(lowestHex, 6);
                        else newRiver = new River(lowestHex, 5);
                    } else if (nextCol == lastCol) {
                        if (nextRow < lastRow) newRiver = new River(lowestHex, 1);
                        else newRiver = new River(lowestHex, 4);
                    } else {
                        if (nextRow < lastRow) newRiver = new River(lowestHex, 2);
                        else newRiver = new River(lowestHex, 3);
                    }
                }
                game->getRivers()->append(newRiver);
                ended = true;
            }
        } else ended = true;
    } while (!ended);
}

bool Lake::isAlreadyInThisLake(Hex *hex) {
    for (int i = 0; i < dimension->size(); i++)
        if (dimension->at(i) == hex) return true;
    return false;
}
