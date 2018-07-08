#include "sector.h"
#include "loratien.h"

extern Loratien *game;

Sector::Sector(int sectorCol, int sectorRow, QGraphicsItem *parent) : QGraphicsPixmapItem(parent),
    hexes(new QList<QList<Hex*>>), col(sectorCol), row(sectorRow), radius(game->getSectorRadius()) {
    setAcceptHoverEvents(true); // allow mouse events
    bool worldEarthStyle = game->getWorldEarthStyle();
    int worldHeight = game->getWorldMapHeight();
    int worldWidth = game->getWorldMapWidth();
    QList<Hex*> hexesCol;
    for (int hexCol = -radius; hexCol <= radius; hexCol++) {
        hexesCol.clear();
        for (int hexRow = -radius+abs(hexCol)/2+(sectorCol%2)*(abs(hexCol)%2); hexRow <= radius-ceil(abs((double)hexCol)/2)+(sectorCol%2)*(abs(hexCol)%2); hexRow++) {
            if ((worldEarthStyle || (-1 < sectorCol+hexCol && sectorCol+hexCol < worldWidth))
                && -1 < sectorRow+hexRow && sectorRow+hexRow < worldHeight) {
                Hex *hex = game->getWorldMap()->at(sectorCol+hexCol).at(sectorRow+hexRow);
                hex->setUsed(true);
                hexesCol.append(hex);
            }
        }
        hexes->append(hexesCol);
    }
}

void Sector::drawSector() {
    int hexSize = game->getHexSize();
    int diameter = 2*radius+1;
    int sectorSize = game->getHexSize() * diameter;
    pic = QPixmap(2 * sectorSize, sqrt(3) * sectorSize);
    pic.fill(Qt::transparent);
    QPainter painter(&pic);
        for (int hexCol = 0; hexCol < hexes->size(); hexCol++) {
            for (int hexRow = 0; hexRow < hexes->at(hexCol).size(); hexRow++) {
                Hex *currentHex = hexes->at(hexCol).at(hexRow);
                if (currentHex->getUsed()) {
                    int hexColRel = col - currentHex->getCol();
                    int hexRowRel = row - currentHex->getRow();
                    int posCol = hexSize * 1.5 * (hexColRel);
                    int posRow = hexSize * sqrt(3) * (hexRowRel) + sqrt(3)/2 * hexSize * (abs(hexColRel)%2);
                    currentHex->setTempUsed(true);
                    painter.drawPixmap(posCol, posRow, currentHex->getPic());
                }
            }
        }
    painter.end();
    pic = pic.scaled(hexSize, hexSize);
}

void Sector::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
}

void Sector::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
}

void Sector::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    game->getWindow()->setRightClick(false);
    game->getWindow()->setLeftClick(false);
    if (event->button() == Qt::RightButton) game->getWindow()->setRightClick(true);
    if (event->button() == Qt::LeftButton) game->getWindow()->setLeftClick(true);
    game->getWindow()->setMousePos(event->pos());
}

void Sector::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::RightButton) game->getWindow()->setRightClick(false);
    if (event->button() == Qt::LeftButton) game->getWindow()->setLeftClick(false);
}

void Sector::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
   event->ignore();
}
