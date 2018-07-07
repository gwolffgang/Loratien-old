#include "region.h"
#include "loratien.h"

extern Loratien *game;

Region::Region(int regionCol, int regionRow, QGraphicsItem *parent) : QGraphicsPixmapItem(parent),
    col(regionCol), row(regionRow), radius(game->getRegionRadius()), tectonicPlate(-1), tectonicDirection(-1),
    sectors(new QList<QList<Sector*>>), brush(QBrush(QColor(0, 0, 0), Qt::SolidPattern)),
    border(QPen(Qt::black)), fogOfWar(true), tempNumber(-999), tempLink(NULL), tempUsed(false) {
    setAcceptHoverEvents(true); // allow mouse events
    QList<Sector*> sectorsRow;
    for (int sectorRow = -radius; sectorRow <= radius; sectorRow++) {
        sectorsRow.clear();
        for (int sectorCol = -radius+abs(sectorRow)/2; sectorCol <= radius-ceil(abs((double)sectorRow)/2); sectorCol++) {
            int hexCol, hexRow;
            hexCol = regionCol + sectorCol*(game->getSectorRadius()*2) + std::abs(sectorRow)%2*game->getSectorRadius();
            hexRow = regionRow + sectorRow*(game->getSectorRadius()*1.5) + abs(sectorRow%2)*game->getSectorRadius()%2;
            Sector *sector = new Sector(hexCol, hexRow);
            sectorsRow.append(sector);
        }
        sectors->append(sectorsRow);
    }
}

QList<Region *> Region::getNeighborRegions(int radius, bool withOriginRegion) {
    if (radius < 0) radius = 0;
    QList<Region*> neighbors;
    bool worldEarthStyle = game->getWorldEarthStyle();
    int worldHeight = game->getWorldHeight();
    int worldWidth = game->getWorldWidth();
    for (int modCol = -radius; modCol <= radius; modCol++) {
            for (int modRow = -radius+abs(modCol)/2+(col%2)*(abs(modCol)%2); modRow <= radius-ceil(abs((double)modCol)/2)+(col%2)*(abs(modCol)%2); modRow++)  {
                if ((withOriginRegion || modCol != 0 || modRow != 0) && (worldEarthStyle || (-1 < col+modCol && col+modCol < worldWidth))
                    && -1 < row+modRow && row+modRow < worldHeight) {
                        neighbors.append(game->getWorldRegions()->at((worldWidth+col+modCol) % worldWidth).at(row+modRow));
                }
            }
        }
    return neighbors;
}

void Region::draw(int factor) {
    if (factor <= 0) factor = 1;
    double regionWidth = game->getRegionWidth() * factor;
    double regionHeight = game->getRegionHeight() * factor;
    int hexSize = game->getHexSize();
    pic = QPixmap(regionWidth, regionHeight);
    pic.fill(Qt::transparent);
    QPainter painter(&pic);
        for (int sectorCol = 0; sectorCol < sectors->size(); sectorCol++) {
            for (int sectorRow = 0; sectorRow < sectors->at(sectorCol).size(); sectorRow++) {
                Sector *currentSector = sectors->at(sectorCol).at(sectorRow);
                int sectorColRel = col - currentSector->getCol();
                int sectorRowRel = row - currentSector->getRow();
                int posCol = hexSize * 1.5 * (sectorColRel);
                int posRow = hexSize * sqrt(3) * (sectorRowRel) + sqrt(3)/2 * hexSize * (abs(sectorColRel)%2);
                painter.drawPixmap(posCol, posRow, currentSector->getPic());
            }
        }
    painter.end();
    pic = pic.scaled(regionWidth, regionHeight);
/*    if (-1 == size) size = game->getRegionSize();
    double sectorDiameter = (2*game->getSectorRadius()+1);
    double width = 2 * size, height = sqrt(3) * size;
    pic = QPixmap(width, height);
    pic.fill(Qt::transparent);
    QPainter painter(&pic);
        for (int elem = 0; elem < sectors->size(); elem++) {
            Sector *currentSector = sectors->at(elem);
            int sectorCol = currentSector->getCol();
            int sectorRow = currentSector->getRow();
            int posCol = size * ((sectorDiameter-1)/sectorDiameter) * (radius + sectorCol) + 0.5 * size * (abs(sectorRow%2));
            int posRow = size * sqrt(3)/2 * (radius + sectorRow);
            painter.drawPixmap(posCol, posRow, currentSector->getPic());
        }
    painter.end();
    pic = pic.scaled(size, size);
    setPixmap(pic);*/

    /*
        QPainter painter(&pic);
            QVector<QPointF> hexPoints;
            hexPoints << QPointF(0.5, 0) << QPointF(1.5, 0) << QPointF(2, sqrt(3)/2) << QPointF(1.5, sqrt(3)) << QPointF(0.5, sqrt(3)) << QPointF(0, sqrt(3)/2);
            for (int i = 0; i < hexPoints.size(); ++i) hexPoints[i] *= size; // scale the points
            QPolygonF hexagon(hexPoints);
            painter.setPen(border);
            painter.setBrush(brush);
            painter.drawPolygon(hexagon);
        painter.end();
        setPixmap(pic);*/

}

void Region::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
}

void Region::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
}

void Region::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    event->ignore();
/*    game->getWindow()->setRightClick(false);
    game->getWindow()->setLeftClick(false);
    if (event->button() == Qt::RightButton) {
        game->getWindow()->setRightClick(true);
    }
    if (event->button() == Qt::LeftButton) {
        game->getWindow()->setLeftClick(true);
        Region *selectedRegionWorldMap = game->getWindow()->getGuiRegionInfo()->getSelectedRegionWorldMap();
        selectedRegionWorldMap->setBorder(QPen(Qt::black));
        selectedRegionWorldMap->draw();
        this->setBorder(QPen(Qt::red, 2, Qt::SolidLine));
        this->draw();
        game->getWindow()->getGuiRegionInfo()->setSelectedRegion(this);
        game->getWindow()->refresh();
    }
    game->getWindow()->setMousePos(event->pos());*/
}

void Region::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::RightButton) game->getWindow()->setRightClick(false);
    if (event->button() == Qt::LeftButton) game->getWindow()->setLeftClick(false);
}

void Region::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    event->ignore();
   //if (game->getWindow()->getRightClick()) game->getWindow()->dragWorldMap(event);
}
