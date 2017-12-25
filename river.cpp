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
    penRiver(QPen(Qt::blue, 3, Qt::SolidLine)) {

    hex->setRiver(true);
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
        Hex *actHex = watercourse->last();
        Hex *nextHex = nextRiverpartHex(watercourse);
        if (nextHex) {
            if (nextHex->getAltitude() <= actHex->getAltitude()) {
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

    for (int col = 0; col < game->getWorldWidth(); col++) {
        for (int row = 0; row < game->getWorldHeight(); row++) {
            game->getWorldMap()->at(col).at(row)->setTempLink(NULL);
            game->getWorldMap()->at(col).at(row)->setTempNumber(9999);
            game->getWorldMap()->at(col).at(row)->setTempUsed(false);
        }
    }
    QList<Hex*> hexes, newHexes, waterHexes;
    Hex *act = NULL;
    spring->setTempNumber(spring->getAltitude() / game->getWorldAltMax());
    hexes.append(spring);
    do {
        newHexes.clear();
        for (int i = 0; i < hexes.size(); i++) {
            int hexCol = hexes.at(i)->getCol();
            int hexRow = hexes.at(i)->getRow();
            for (int col = -1; col <= 1; col++) {
                for (int row = -1; row <= 1; row++) {
                    if ((hexCol+col != 0 || hexRow+row != 0) && (game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol+col > -1 && hexCol+col < game->getWorldHeight())) && hexRow+row > -1 && hexRow+row < game->getWorldWidth()) {
                        if (!(hexCol%2==1 && (row == -1) && (col == -1 || col == 1)) && !(hexCol%2==0 && (row == 1) && (col == -1 || col == 1))) {
                            act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+col)) % game->getWorldWidth()).at(hexRow+row);
                            if (!act->getTempUsed()) {
                                double possibleTempNumber = act->getAltitude() / game->getWorldAltMax() + hexes.at(i)->getTempNumber();
                                if (possibleTempNumber < act->getTempNumber()) {
                                    act->setTempNumber(possibleTempNumber);
                                    act->setTempLink(hexes.at(i));
                                }
                                if (act->getAltitude() < 0) waterHexes.append(act);
                                else newHexes.append(act);
                                act->setTempUsed(true);
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
    Hex *last = watercourse->at(watercourse->size()-2);
    Hex *next = watercourse->last();
    switch (direction) {
    case 1:
        if (!last->getLineDir4()) last->setLineDir4(new QGraphicsLineItem(last));
        last->getLineDir4()->setLine(QLineF(center, north));
        last->getLineDir4()->setPen(penRiver);
        if (next->getAltitude() >= 0 && !game->getWorldMap()->at(next->getCol()).at(next->getRow())->getLake()) {
            if (!next->getLineDir1()) next->setLineDir1(new QGraphicsLineItem(next));
            next->getLineDir1()->setLine(QLineF(south, center));
            next->getLineDir1()->setPen(penRiver);
        }
        break;
    case 2:
        if (!last->getLineDir5()) last->setLineDir5(new QGraphicsLineItem(last));
        last->getLineDir5()->setLine(QLineF(center, northeast));
        last->getLineDir5()->setPen(penRiver);
        if (next->getAltitude() >= 0 && !game->getWorldMap()->at(next->getCol()).at(next->getRow())->getLake()) {
            if (!next->getLineDir2()) next->setLineDir2(new QGraphicsLineItem(next));
            next->getLineDir2()->setLine(QLineF(southwest, center));
            next->getLineDir2()->setPen(penRiver);
        }
            break;
    case 3:
        if (!last->getLineDir6()) last->setLineDir6(new QGraphicsLineItem(last));
        last->getLineDir6()->setLine(QLineF(center, southeast));
        last->getLineDir6()->setPen(penRiver);
        if (next->getAltitude() >= 0 && !game->getWorldMap()->at(next->getCol()).at(next->getRow())->getLake()) {
            if (!next->getLineDir3()) next->setLineDir3(new QGraphicsLineItem(next));
            next->getLineDir3()->setLine(QLineF(northwest, center));
            next->getLineDir3()->setPen(penRiver);
        }
        break;
    case 4:
        if (!last->getLineDir1()) last->setLineDir1(new QGraphicsLineItem(last));
        last->getLineDir1()->setLine(QLineF(center, south));
        last->getLineDir1()->setPen(penRiver);
        if (next->getAltitude() >= 0 && !game->getWorldMap()->at(next->getCol()).at(next->getRow())->getLake()) {
            if (!next->getLineDir4()) next->setLineDir4(new QGraphicsLineItem(next));
            next->getLineDir4()->setLine(QLineF(north, center));
            next->getLineDir4()->setPen(penRiver);
        }
        break;
    case 5:
        if (!last->getLineDir2()) last->setLineDir2(new QGraphicsLineItem(last));
        last->getLineDir2()->setLine(QLineF(center, southwest));
        last->getLineDir2()->setPen(penRiver);
        if (next->getAltitude() >= 0 && !game->getWorldMap()->at(next->getCol()).at(next->getRow())->getLake()) {
            if (!next->getLineDir5()) next->setLineDir5(new QGraphicsLineItem(next));
            next->getLineDir5()->setLine(QLineF(northeast, center));
            next->getLineDir5()->setPen(penRiver);
        }
        break;
    case 6:
        if (!last->getLineDir3()) last->setLineDir3(new QGraphicsLineItem(last));
        last->getLineDir3()->setLine(QLineF(center, northwest));
        last->getLineDir3()->setPen(penRiver);
        if (next->getAltitude() >= 0 && !game->getWorldMap()->at(next->getCol()).at(next->getRow())->getLake()) {
            if (!next->getLineDir6()) next->setLineDir6(new QGraphicsLineItem(next));
            next->getLineDir6()->setLine(QLineF(southeast, center));
            next->getLineDir6()->setPen(penRiver);
        }
    }
}

Hex *River::nextRiverpartHex(QList<Hex*> *watercourse) {
    Hex *last = watercourse->last();
    Hex *act = NULL;
    Hex* lowest = NULL;
    QList<Hex*> choose_from;
    int hexCol = last->getCol(), hexRow = last->getRow();
    switch (direction) {
    case 0:
        for (int col = -1; col <= 1; col++) {
            for (int row = -1; row <= 1; row++) {
                if (game->getWindow()->neighbor(hexCol, col, hexRow, row)) {
                    act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+col)) % game->getWorldWidth()).at(hexRow+row);
                    if (!lowest) lowest = act;
                    else if (act->getAltitude() < lowest->getAltitude()) {
                        choose_from.clear();
                        lowest = act;
                    } else if (act->getAltitude() == lowest->getAltitude()) {
                        choose_from.append(act);
                        lowest = act;
                    }
                }
            }
        }
        if (choose_from.size() > 0) lowest = choose_from.at(rand()%choose_from.size());
        if (hexCol%2==1) {
            if (lowest->getCol() == hexCol-1) {
                if (lowest->getRow() == hexRow) direction = 6;
                else direction = 5;
            } else if (lowest->getCol() == hexCol) {
                if (lowest->getRow() == hexRow-1) direction = 1;
                else direction = 4;
            } else {
                if (lowest->getRow() == hexRow) direction = 2;
                else direction = 3;
            }
        } else {
            if (lowest->getCol() == hexCol-1) {
                if (lowest->getRow() == hexRow-1) direction = 6;
                else direction = 5;
            } else if (lowest->getCol() == hexCol) {
                if (lowest->getRow() == hexRow-1) direction = 1;
                else direction = 4;
            } else {
                if (lowest->getRow() == hexRow-1) direction = 2;
                else direction = 3;
            }
        }
        break;
    case 1:
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol > 0)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) lowest = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow-1);
            else lowest = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow);
            direction = 6;
        }
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol < game->getWorldWidth()-1)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow-1);
            else act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow);
            if (act->getAltitude() < lowest->getAltitude() || (act->getAltitude() == lowest->getAltitude() && rand()%2 == 0)) {
                lowest = act;
                direction = 2;
            }
        }
        if (hexRow > 0) {
            act = game->getWorldMap()->at(hexCol).at(hexRow-1);
            if (act->getAltitude() <= lowest->getAltitude()) {
                lowest = act;
                direction = 1;
            }
        }
        break;
    case 2:
        if (hexRow > 0) {
            lowest = game->getWorldMap()->at(hexCol).at(hexRow-1);
            direction = 1;
        }
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol < game->getWorldWidth()-1)) && (hexRow < game->getWorldHeight()-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow);
            else act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow+1);
            if (act->getAltitude() < lowest->getAltitude() || (act->getAltitude() == lowest->getAltitude() && rand()%2 == 0)) {
                lowest = act;
                direction = 3;
            }
        }
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol < game->getWorldWidth()-1)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow-1);
            else act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow);
            if (act->getAltitude() <= lowest->getAltitude()) {
                lowest = act;
                direction = 2;
            }
        }
        break;
    case 3:
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol < game->getWorldWidth()-1)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) lowest = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow-1);
            else lowest = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow);
            direction = 2;
        }
        if (hexRow < game->getWorldHeight()-1) {
            act = game->getWorldMap()->at(hexCol).at(hexRow+1);
            if (act->getAltitude() < lowest->getAltitude() || (act->getAltitude() == lowest->getAltitude() && rand()%2 == 0)) {
                lowest = act;
                direction = 4;
            }
        }
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol < game->getWorldWidth()-1)) && (hexRow < game->getWorldHeight()-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow);
            else act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow+1);
            if (act->getAltitude() <= lowest->getAltitude()) {
                lowest = act;
                direction = 3;
            }
        }
        break;
    case 4:
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol < game->getWorldWidth()-1)) && (hexRow < game->getWorldHeight()-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) lowest = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow);
            else lowest = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+1)) % game->getWorldWidth()).at(hexRow+1);
            direction = 3;
        }
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol > 0)) && (hexRow < game->getWorldHeight()-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow);
            else act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow+1);
            if (act->getAltitude() < lowest->getAltitude() || (act->getAltitude() == lowest->getAltitude() && rand()%2 == 0)) {
                lowest = act;
                direction = 5;
            }
        }
        if (hexRow < game->getWorldHeight()-1) {
            act = game->getWorldMap()->at(hexCol).at(hexRow+1);
            if (act->getAltitude() <= lowest->getAltitude()) {
                lowest = act;
                direction = 4;
            }
        }
        break;
    case 5:
        if (hexRow < game->getWorldHeight()-1) {
            lowest = game->getWorldMap()->at(hexCol).at(hexRow+1);
            direction = 4;
        }
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol > 0)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow-1);
            else act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow);
            if (act->getAltitude() < lowest->getAltitude() || (act->getAltitude() == lowest->getAltitude() && rand()%2 == 0)) {
                lowest = act;
                direction = 6;
            }
        }
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol > 0)) && (hexRow < game->getWorldHeight()-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow);
            else act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow+1);
            if (act->getAltitude() <= lowest->getAltitude()) {
                lowest = act;
                direction = 5;
            }
        }
        break;
    case 6:
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol > 0)) && (hexRow < game->getWorldHeight()-1 || hexCol%2 == 0)) {
            if (hexCol%2 == 0) lowest = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow);
            else lowest = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow+1);
            direction = 5;
        }
        if (hexRow > 0) {
            act = game->getWorldMap()->at(hexCol).at(hexRow-1);
            if (act->getAltitude() < lowest->getAltitude() || (act->getAltitude() == lowest->getAltitude() && rand()%2 == 0)) {
                lowest = act;
                direction = 1;
            }
        }
        if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol > 0)) && (hexRow > 0 || hexCol%2 == 1)) {
            if (hexCol%2 == 0) act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow-1);
            else act = game->getWorldMap()->at((game->getWorldWidth()+(hexCol-1)) % game->getWorldWidth()).at(hexRow);
            if (act->getAltitude() <= lowest->getAltitude()) {
                lowest = act;
                direction = 6;
            }
        }
    }
    return lowest;
}
