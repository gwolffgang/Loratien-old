#include "hex.h"
#include "loratien.h"
#include <QLineF>

extern Loratien *game;

Hex::Hex(QGraphicsItem *parent) : QGraphicsPixmapItem(parent),
    brush(QBrush(QColor(0, 0, 0), Qt::SolidPattern)), border(QPen(Qt::black)),
    col(-1), row(-1), tectonicPlate(-1), tectonicDirection(-1), fertility(0), riverSize(0), altitude(-99),
    type(' '), climate(' '), lake(false), river(false),
    tempNumber(9999), tempLink(NULL), tempUsed(false),
    lineDir1(NULL), lineDir2(NULL), lineDir3(NULL), lineDir4(NULL), lineDir5(NULL), lineDir6(NULL) {
    setAcceptHoverEvents(true); // allow special mouse events
}

QList<Hex*> Hex::getNeighborHexes(int radius, bool withOriginHex) {
    if (radius < 0) radius = 0;
    QList<Hex*> neighbors;
    bool worldEarthStyle = game->getWorldEarthStyle();
    int worldHeight = game->getWorldHeight();
    int worldWidth = game->getWorldWidth();
    for (int modCol = -radius; modCol <= radius; modCol++) {
            for (int modRow = -radius+abs(modCol)/2+(col%2)*(abs(modCol)%2); modRow <= radius-ceil(abs((double)modCol)/2)+(col%2)*(abs(modCol)%2); modRow++)  {
                if ((withOriginHex || modCol != 0 || modRow != 0)
                    && (worldEarthStyle || (!worldEarthStyle && -1 < col+modCol && col+modCol < worldWidth))
                    && -1 < row+modRow && row+modRow < worldHeight) {
                        neighbors.append(game->getWorldMap()->at((worldWidth+col+modCol) % worldWidth).at(row+modRow));
                }
            }
        }
    return neighbors;
}

void Hex::draw(int size) {
    if (-1 == size) size = game->getWindow()->getHexSize();
    double width = 2 * size, height = sqrt(3) * size;
    pic = QPixmap(width, height);
    pic.fill(Qt::transparent);
    QPainter painter(&pic);
        QVector<QPointF> hexPoints;
        hexPoints << QPointF(0.5, 0) << QPointF(1.5, 0) << QPointF(2, sqrt(3)/2) << QPointF(1.5, sqrt(3)) << QPointF(0.5, sqrt(3)) << QPointF(0, sqrt(3)/2);
        for (int i = 0; i < hexPoints.size(); ++i) hexPoints[i] *= size; // scale the points
        QPolygonF hexagon(hexPoints);
        painter.setPen(border);
        painter.setBrush(brush);
        painter.drawPolygon(hexagon);
    painter.end();
    setPixmap(pic);
}

void Hex::evaluateFertility() {
    QList<Hex*> neighbors = getNeighborHexes(1, true);
    for (int k = 0; k < neighbors.size(); k++) fertility += neighbors.at(k)->getRiverSize();
}

void Hex::evaluateResources() {

}

void Hex::removeRivers() {
    foreach (River *river, game->getRivers(this)) {
        bool done = false;
        while (!done && river->getWatercourse()->size() > 0) {
            Hex *part = river->getWatercourse()->last();
            if(part->getCol() == col && part->getRow() == row) done = true;
            part->setRiver(false);
            if (part->getLineDir1()) { delete part->getLineDir1(); part->setLineDir1(NULL); }
            if (part->getLineDir2()) { delete part->getLineDir2(); part->setLineDir2(NULL); }
            if (part->getLineDir3()) { delete part->getLineDir3(); part->setLineDir3(NULL); }
            if (part->getLineDir4()) { delete part->getLineDir4(); part->setLineDir4(NULL); }
            if (part->getLineDir5()) { delete part->getLineDir5(); part->setLineDir5(NULL); }
            if (part->getLineDir6()) { delete part->getLineDir6(); part->setLineDir6(NULL); }
            river->getWatercourse()->removeLast();
        }
    }
}

void Hex::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
}

void Hex::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
}

void Hex::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
   if (game->getWindow()->getRightClick()) game->getWindow()->dragWorldMap(event);
}

void Hex::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    game->getWindow()->setRightClick(false);
    game->getWindow()->setLeftClick(false);
    if (event->button() == Qt::RightButton) {
        game->getWindow()->setRightClick(true);
    }
    if (event->button() == Qt::LeftButton) {
        game->getWindow()->setLeftClick(true);
        Hex *selectedHexWorldMap = game->getWindow()->getGuiHexInfo()->getSelectedHexWorldMap();
        selectedHexWorldMap->setBorder(QPen(Qt::black));
        selectedHexWorldMap->draw();
        this->setBorder(QPen(Qt::red, 2, Qt::SolidLine));
        this->draw();
        game->getWindow()->getGuiHexInfo()->setSelectedHex(this);
        game->getWindow()->refresh();
    }
    game->getWindow()->setMousePos(event->pos());
}

void Hex::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::RightButton) game->getWindow()->setRightClick(false);
    if (event->button() == Qt::LeftButton) game->getWindow()->setLeftClick(false);
}
