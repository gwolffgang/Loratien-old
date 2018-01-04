#include "loratien.h"
#include "river.h"
#include "lake.h"

extern Loratien *game;

River::River(Hex *hex, int dir) : name("River"), watercourse(new QList<Hex*>), direction(dir),
    north(QPointF(game->getWindow()->getHexSize(), 0.0)),
    northeast(QPointF(game->getWindow()->getHexSize() * 1.75, sqrt(3) * 0.25 * game->getWindow()->getHexSize())),
    northwest(QPointF(game->getWindow()->getHexSize() * 0.25, sqrt(3) * 0.25 * game->getWindow()->getHexSize())),
    south(QPointF(game->getWindow()->getHexSize(), sqrt(3) * game->getWindow()->getHexSize())),
    southeast(QPointF(game->getWindow()->getHexSize() * 1.75, sqrt(3) * 0.75 * game->getWindow()->getHexSize())),
    southwest(QPointF(game->getWindow()->getHexSize() * 0.25, sqrt(3) * 0.75 * game->getWindow()->getHexSize())),
    center(QPointF(game->getWindow()->getHexSize(), sqrt(3) * 0.5 * game->getWindow()->getHexSize())),
    penRiver(QPen(Qt::blue, 1, Qt::SolidLine)) {

    hex->setRiver(true);
    if (direction != 0) hex->setRiverSize(game->getWindow()->getMaxRiverSize());
    else hex->setRiverSize(1);
    penRiver = QPen(Qt::blue, hex->getRiverSize(), Qt::SolidLine);
    watercourse->append(hex);
    switch (direction) {
    case 1:
        if (!hex->getLineDir4()) hex->setLineDir4(new QGraphicsLineItem);
        hex->getLineDir4()->setLine(QLineF(south, center));
        hex->getLineDir4()->setPen(penRiver);
        break;
    case 2:
        if (!hex->getLineDir5()) hex->setLineDir5(new QGraphicsLineItem);
        hex->getLineDir5()->setLine(QLineF(southwest, center));
        hex->getLineDir5()->setPen(penRiver);
        break;
    case 3:
        if (!hex->getLineDir6()) hex->setLineDir6(new QGraphicsLineItem);
        hex->getLineDir6()->setLine(QLineF(northwest, center));
        hex->getLineDir6()->setPen(penRiver);
        break;
    case 4:
        if (!hex->getLineDir1()) hex->setLineDir1(new QGraphicsLineItem);
        hex->getLineDir1()->setLine(QLineF(north, center));
        hex->getLineDir1()->setPen(penRiver);
        break;
    case 5:
        if (!hex->getLineDir2()) hex->setLineDir2(new QGraphicsLineItem);
        hex->getLineDir2()->setLine(QLineF(northeast, center));
        hex->getLineDir2()->setPen(penRiver);
        break;
    case 6:
        if (!hex->getLineDir3()) hex->setLineDir3(new QGraphicsLineItem);
        hex->getLineDir3()->setLine(QLineF(southeast, center));
        hex->getLineDir3()->setPen(penRiver);
    }
    bool ended = false;
    do {
        Hex *currentHexHex = watercourse->last();
        Hex *nextHex = nextRiverpartHex(watercourse);
        if (nextHex) {
            if (nextHex->getAltitude() <= currentHexHex->getAltitude()) {
                nextHex->setRiverSize(std::max(currentHexHex->getRiverSize(), nextHex->getRiverSize()));
                watercourse->append(nextHex);
                drawRiver();
                if (nextHex->getAltitude() < 0 || nextHex->getLake() == true) {
                    nextHex->removeRivers();
                    watercourse->removeLast();
                    ended = true;
                } else nextHex->setRiver(true);
            } else {
                if (nextHex->getAltitude() >= 0) {
                    watercourse->append(nextHex);
                    drawRiver();
                    watercourse->removeLast();
                    Lake *lake = new Lake(nextHex);
                    game->getLakes()->append(lake);
                    nextHex->removeRivers();
                }
                ended = true;
            }
        } else ended = true;
    } while (!ended);
}

QList<Hex*> River::calculateWatercourse(Hex *spring) {

    /* QList<Hex*> hexes = calculateWatercourse(hex);
     for (int k = 0; k < hexes.size(); k++) {
         Riverpart *part = new Riverpart(hexes.at(k));
         part->setCol(hexes.at(k)->getCol());
         part->setRow(hexes.at(k)->getRow());
         part->setAltitude(hexes.at(k)->getAltitude());
         watercourse->append(part);
     } */

    int worldHeight = game->getWorldHeight();
    int worldWidth = game->getWorldWidth();
    bool worldEarthStyle = game->getWorldEarthStyle();

    for (int col = 0; col < worldWidth; col++) {
        for (int row = 0; row < worldHeight; row++) {
            game->getWorldMap()->at(col).at(row)->setTempLink(NULL);
            game->getWorldMap()->at(col).at(row)->setTempNumber(9999);
            game->getWorldMap()->at(col).at(row)->setTempUsed(false);
        }
    }
    QList<Hex*> hexes, newHexes, waterHexes;
    Hex *currentHex = NULL;
    spring->setTempNumber(spring->getAltitude() / game->getWorldAltMax());
    hexes.append(spring);
    do {
        newHexes.clear();
        for (int i = 0; i < hexes.size(); i++) {
            int hexCol = hexes.at(i)->getCol();
            int hexRow = hexes.at(i)->getRow();
            for (int col = -1; col <= 1; col++) {
                for (int row = -1; row <= 1; row++) {
                    if ((hexCol+col != 0 || hexRow+row != 0) && (worldEarthStyle || (!worldEarthStyle && hexCol+col > -1 && hexCol+col < worldHeight)) && hexRow+row > -1 && hexRow+row < worldWidth) {
                        if (!(hexCol%2==1 && (row == -1) && (col == -1 || col == 1)) && !(hexCol%2==0 && (row == 1) && (col == -1 || col == 1))) {
                            currentHex = game->getWorldMap()->at((worldWidth+(hexCol+col)) % worldWidth).at(hexRow+row);
                            if (!currentHex->getTempUsed()) {
                                double possibleTempNumber = currentHex->getAltitude() / game->getWorldAltMax() + hexes.at(i)->getTempNumber();
                                if (possibleTempNumber < currentHex->getTempNumber()) {
                                    currentHex->setTempNumber(possibleTempNumber);
                                    currentHex->setTempLink(hexes.at(i));
                                }
                                if (currentHex->getAltitude() < 0) waterHexes.append(currentHex);
                                else newHexes.append(currentHex);
                                currentHex->setTempUsed(true);
                            }
                        }
                    }
                }
            }
        }
        hexes.clear();
        for (int i = 0; i < newHexes.size(); i++) hexes.append(newHexes.at(i));
    } while (hexes.size() > 0);
    Hex *mouth = NULL;
    if (waterHexes.size() > 0) {
        mouth = waterHexes.at(0);
        for (int i = 0; i < waterHexes.size(); i++) if (waterHexes.at(i)->getTempNumber() < mouth->getTempNumber()) mouth = waterHexes.at(i);
    }
    if (mouth != NULL) {
        hexes.append(mouth);
        Hex *prevous;
        do {
            prevous = hexes.at(0)->getTempLink();
            if (prevous) hexes.prepend(prevous);
        } while (prevous);
    }
    return hexes;
}

void River::drawRiver() {
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    Hex *previousHex = watercourse->at(watercourse->size()-2);
    Hex *nextHex = watercourse->last();
    penRiver = QPen(Qt::blue, nextHex->getRiverSize(), Qt::SolidLine);
    switch (direction) {
    case 1: // north
        if (!previousHex->getLineDir4()) previousHex->setLineDir4(new QGraphicsLineItem(previousHex));
        previousHex->getLineDir4()->setLine(QLineF(center, north));
        previousHex->getLineDir4()->setPen(penRiver);
        if (nextHex->getAltitude() >= 0 && !worldMap->at(nextHex->getCol()).at(nextHex->getRow())->getLake()) {
            if (!nextHex->getLineDir1()) nextHex->setLineDir1(new QGraphicsLineItem(nextHex));
            nextHex->getLineDir1()->setLine(QLineF(south, center));
            nextHex->getLineDir1()->setPen(penRiver);
        }
        break;
    case 2: // northeast
        if (!previousHex->getLineDir5()) previousHex->setLineDir5(new QGraphicsLineItem(previousHex));
        previousHex->getLineDir5()->setLine(QLineF(center, northeast));
        previousHex->getLineDir5()->setPen(penRiver);
        if (nextHex->getAltitude() >= 0 && !worldMap->at(nextHex->getCol()).at(nextHex->getRow())->getLake()) {
            if (!nextHex->getLineDir2()) nextHex->setLineDir2(new QGraphicsLineItem(nextHex));
            nextHex->getLineDir2()->setLine(QLineF(southwest, center));
            nextHex->getLineDir2()->setPen(penRiver);
        }
        break;
    case 3: // southeast
        if (!previousHex->getLineDir6()) previousHex->setLineDir6(new QGraphicsLineItem(previousHex));
        previousHex->getLineDir6()->setLine(QLineF(center, southeast));
        previousHex->getLineDir6()->setPen(penRiver);
        if (nextHex->getAltitude() >= 0 && !worldMap->at(nextHex->getCol()).at(nextHex->getRow())->getLake()) {
            if (!nextHex->getLineDir3()) nextHex->setLineDir3(new QGraphicsLineItem(nextHex));
            nextHex->getLineDir3()->setLine(QLineF(northwest, center));
            nextHex->getLineDir3()->setPen(penRiver);
        }
        break;
    case 4: // south
        if (!previousHex->getLineDir1()) previousHex->setLineDir1(new QGraphicsLineItem(previousHex));
        previousHex->getLineDir1()->setLine(QLineF(center, south));
        previousHex->getLineDir1()->setPen(penRiver);
        if (nextHex->getAltitude() >= 0 && !worldMap->at(nextHex->getCol()).at(nextHex->getRow())->getLake()) {
            if (!nextHex->getLineDir4()) nextHex->setLineDir4(new QGraphicsLineItem(nextHex));
            nextHex->getLineDir4()->setLine(QLineF(north, center));
            nextHex->getLineDir4()->setPen(penRiver);
        }
        break;
    case 5: // southwest
        if (!previousHex->getLineDir2()) previousHex->setLineDir2(new QGraphicsLineItem(previousHex));
        previousHex->getLineDir2()->setLine(QLineF(center, southwest));
        previousHex->getLineDir2()->setPen(penRiver);
        if (nextHex->getAltitude() >= 0 && !worldMap->at(nextHex->getCol()).at(nextHex->getRow())->getLake()) {
            if (!nextHex->getLineDir5()) nextHex->setLineDir5(new QGraphicsLineItem(nextHex));
            nextHex->getLineDir5()->setLine(QLineF(northeast, center));
            nextHex->getLineDir5()->setPen(penRiver);
        }
        break;
    case 6: // northwest
        if (!previousHex->getLineDir3()) previousHex->setLineDir3(new QGraphicsLineItem(previousHex));
        previousHex->getLineDir3()->setLine(QLineF(center, northwest));
        previousHex->getLineDir3()->setPen(penRiver);
        if (nextHex->getAltitude() >= 0 && !worldMap->at(nextHex->getCol()).at(nextHex->getRow())->getLake()) {
            if (!nextHex->getLineDir6()) nextHex->setLineDir6(new QGraphicsLineItem(nextHex));
            nextHex->getLineDir6()->setLine(QLineF(southeast, center));
            nextHex->getLineDir6()->setPen(penRiver);
        }
    }
}

Hex *River::nextRiverpartHex(QList<Hex*> *watercourse) {
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    int worldHeight = game->getWorldHeight();
    int worldWidth = game->getWorldWidth();
    bool worldEarthStyle = game->getWorldEarthStyle();
    Hex *previousHex = watercourse->last();
    Hex *currentHex = NULL;
    Hex *lowestHex = NULL;
    QList<Hex*> choose_from;
    int hexCol = previousHex->getCol();
    int hexRow = previousHex->getRow();
    switch (direction) {
    case 0: { // undecided yet
        QList<Hex*> neighbors = game->getWindow()->getHexNeighbors(hexCol, hexRow);
        for (int k = 0; k < neighbors.size(); k++) {
            currentHex = neighbors.at(k);
            if (!lowestHex) lowestHex = currentHex;
            else {
                if (currentHex->getAltitude() < lowestHex->getAltitude()) {
                    choose_from.clear();
                    lowestHex = currentHex;
                } else if (currentHex->getAltitude() == lowestHex->getAltitude()) {
                    choose_from.append(currentHex);
                    lowestHex = currentHex;
                }
            }
        }
        if (choose_from.size() > 0) lowestHex = choose_from.at(rand()%choose_from.size());
        if (hexCol%2==1) {
            if (lowestHex->getCol() == hexCol-1) {
                if (lowestHex->getRow() == hexRow) direction = 6; //northwest
                else direction = 5; // southwest
            } else if (lowestHex->getCol() == hexCol) {
                if (lowestHex->getRow() == hexRow-1) direction = 1; // north
                else direction = 4; // south
            } else {
                if (lowestHex->getRow() == hexRow) direction = 2; // northeast
                else direction = 3; // southeast
            }
        } else {
            if (lowestHex->getCol() == hexCol-1) {
                if (lowestHex->getRow() == hexRow-1) direction = 6; // northwest
                else direction = 5; // southwest
            } else if (lowestHex->getCol() == hexCol) {
                if (lowestHex->getRow() == hexRow-1) direction = 1; // north
                else direction = 4; // south
            } else {
                if (lowestHex->getRow() == hexRow-1) direction = 2; // northeast
                else direction = 3; // southeast
            }
        }
        break;
    }
    case 1: // north
        if ((worldEarthStyle || (!worldEarthStyle && hexCol > 0)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) lowestHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow-1);
            else lowestHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow);
            direction = 6;
        }
        if ((worldEarthStyle || (!worldEarthStyle && hexCol < worldWidth-1)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) currentHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow-1);
            else currentHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow);
            if (currentHex->getAltitude() < lowestHex->getAltitude() || (currentHex->getAltitude() == lowestHex->getAltitude() && rand()%2 == 0)) {
                lowestHex = currentHex;
                direction = 2;
            }
        }
        if (hexRow > 0) {
            currentHex = worldMap->at(hexCol).at(hexRow-1);
            if (currentHex->getAltitude() <= lowestHex->getAltitude()) {
                lowestHex = currentHex;
                direction = 1;
            }
        }
        break;
    case 2: // northeast
        if (hexRow > 0) {
            lowestHex = worldMap->at(hexCol).at(hexRow-1);
            direction = 1;
        }
        if ((worldEarthStyle || (!worldEarthStyle && hexCol < worldWidth-1)) && (hexRow < worldHeight-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) currentHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow);
            else currentHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow+1);
            if (currentHex->getAltitude() < lowestHex->getAltitude() || (currentHex->getAltitude() == lowestHex->getAltitude() && rand()%2 == 0)) {
                lowestHex = currentHex;
                direction = 3;
            }
        }
        if ((worldEarthStyle || (!worldEarthStyle && hexCol < worldWidth-1)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) currentHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow-1);
            else currentHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow);
            if (currentHex->getAltitude() <= lowestHex->getAltitude()) {
                lowestHex = currentHex;
                direction = 2;
            }
        }
        break;
    case 3: // southeast
        if ((worldEarthStyle || (!worldEarthStyle && hexCol < worldWidth-1)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) lowestHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow-1);
            else lowestHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow);
            direction = 2;
        }
        if (hexRow < worldHeight-1) {
            currentHex = worldMap->at(hexCol).at(hexRow+1);
            if (currentHex->getAltitude() < lowestHex->getAltitude() || (currentHex->getAltitude() == lowestHex->getAltitude() && rand()%2 == 0)) {
                lowestHex = currentHex;
                direction = 4;
            }
        }
        if ((worldEarthStyle || (!worldEarthStyle && hexCol < worldWidth-1)) && (hexRow < worldHeight-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) currentHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow);
            else currentHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow+1);
            if (currentHex->getAltitude() <= lowestHex->getAltitude()) {
                lowestHex = currentHex;
                direction = 3;
            }
        }
        break;
    case 4: // south
        if ((worldEarthStyle || (!worldEarthStyle && hexCol < worldWidth-1)) && (hexRow < worldHeight-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) lowestHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow);
            else lowestHex = worldMap->at((worldWidth+(hexCol+1)) % worldWidth).at(hexRow+1);
            direction = 3;
        }
        if ((worldEarthStyle || (!worldEarthStyle && hexCol > 0)) && (hexRow < worldHeight-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) currentHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow);
            else currentHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow+1);
            if (currentHex->getAltitude() < lowestHex->getAltitude() || (currentHex->getAltitude() == lowestHex->getAltitude() && rand()%2 == 0)) {
                lowestHex = currentHex;
                direction = 5;
            }
        }
        if (hexRow < worldHeight-1) {
            currentHex = worldMap->at(hexCol).at(hexRow+1);
            if (currentHex->getAltitude() <= lowestHex->getAltitude()) {
                lowestHex = currentHex;
                direction = 4;
            }
        }
        break;
    case 5: // southwest
        if (hexRow < worldHeight-1) {
            lowestHex = worldMap->at(hexCol).at(hexRow+1);
            direction = 4;
        }
        if ((worldEarthStyle || (!worldEarthStyle && hexCol > 0)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) currentHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow-1);
            else currentHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow);
            if (currentHex->getAltitude() < lowestHex->getAltitude() || (currentHex->getAltitude() == lowestHex->getAltitude() && rand()%2 == 0)) {
                lowestHex = currentHex;
                direction = 6;
            }
        }
        if ((worldEarthStyle || (!worldEarthStyle && hexCol > 0)) && (hexRow < worldHeight-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) currentHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow);
            else currentHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow+1);
            if (currentHex->getAltitude() <= lowestHex->getAltitude()) {
                lowestHex = currentHex;
                direction = 5;
            }
        }
        break;
    case 6: // northwest
        if ((worldEarthStyle || (!worldEarthStyle && hexCol > 0)) && (hexRow < worldHeight-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) lowestHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow);
            else lowestHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow+1);
            direction = 5;
        }
        if (hexRow > 0) {
            currentHex = worldMap->at(hexCol).at(hexRow-1);
            if (currentHex->getAltitude() < lowestHex->getAltitude() || (currentHex->getAltitude() == lowestHex->getAltitude() && rand()%2 == 0)) {
                lowestHex = currentHex;
                direction = 1;
            }
        }
        if ((worldEarthStyle || (!worldEarthStyle && hexCol > 0)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) currentHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow-1);
            else currentHex = worldMap->at((worldWidth+(hexCol-1)) % worldWidth).at(hexRow);
            if (currentHex->getAltitude() <= lowestHex->getAltitude()) {
                lowestHex = currentHex;
                direction = 6;
            }
        }
    } // end switch
    return lowestHex;
}
