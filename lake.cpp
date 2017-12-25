#include "lake.h"
#include "loratien.h"

extern Loratien *game;

Lake::Lake(Hex *hex) : name("Lake"), dimension(new QList<Hex*>) {
    hex->setLake(true);
    hex->removeRivers();
    dimension->append(hex);
    expandLake();
}

Hex *Lake::checkRiver(Hex *lowest) {
    Hex *act = NULL;
    Hex *rivercheck = NULL;
    int hexCol = lowest->getCol();
    int hexRow = lowest->getRow();
    for (int col = -1; col <= 1; col++) {
        for (int row = -1; row <= 1; row++) {
            if (game->getWindow()->neighbor(hexCol, col, hexRow, row)) {
                act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+col)) % game->getWorldWidth()).at(hexRow+row);
                if (!act->getLake() && !act->getRiver() && (!rivercheck || (act->getAltitude() < rivercheck->getAltitude()))) rivercheck = act;
            }
        }
    }
    return rivercheck;
}

void Lake::expandLake() {
    bool ended = false;
    Hex *last = NULL;
    do {
        Hex *act = NULL;
        Hex *lowest = NULL;
        for (int k = 0; k < dimension->size(); k++) {
            int hexCol = dimension->at(k)->getCol();
            int hexRow = dimension->at(k)->getRow();
            for (int col = -1; col <= 1; col++) {
                for (int row = -1; row <= 1; row++) {
                    if (game->getWindow()->neighbor(hexCol, col, hexRow, row)) {
                        act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+col)) % game->getWorldWidth()).at(hexRow+row);
                        if (!act->getLake() && (!lowest || (act->getAltitude() < lowest->getAltitude()))) {
                            lowest = act;
                            last = dimension->at(k);
                        }
                    }
                }
            }
        }
        Hex *rivercheck = checkRiver(lowest);
        if (rivercheck) {
            if (lowest->getAltitude() <= rivercheck->getAltitude()) {
                dimension->append(lowest);
                lowest->setLake(true);
                lowest->removeRivers();
            } else {
                River *newRiver;
                int lastCol = last->getCol(), lastRow = last->getRow();
                int nextCol = lowest->getCol(), nextRow = lowest->getRow();
                if (lastCol%2==1) {
                    if (nextCol < lastCol) {
                        if (nextRow == lastRow) newRiver = new River(last, 6);
                        else newRiver = new River(last, 5);
                    } else if (nextCol == lastCol) {
                        if (nextRow < lastRow) newRiver = new River(last, 1);
                        else newRiver = new River(last, 4);
                    } else {
                        if (nextRow == lastRow) newRiver = new River(last, 2);
                        else newRiver = new River(last, 3);
                    }
                } else {
                    if (nextCol < lastCol) {
                        if (nextRow < lastRow) newRiver = new River(last, 6);
                        else newRiver = new River(last, 5);
                    } else if (nextCol == lastCol) {
                        if (nextRow < lastRow) newRiver = new River(last, 1);
                        else newRiver = new River(last, 4);
                    } else {
                        if (nextRow < lastRow) newRiver = new River(last, 2);
                        else newRiver = new River(last, 3);
                    }
                }
                game->getRivers()->append(newRiver);
                ended = true;
            }
        }
    } while (!ended);
}
