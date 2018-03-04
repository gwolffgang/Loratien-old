#include "hex.h"
#include "loratien.h"
#include <QLineF>
#include <QPainter>

extern Loratien *game;

Hex::Hex(QGraphicsItem *parent) : QGraphicsPolygonItem(parent),
    col(-1), row(-1), tectonicPlate(-1), fertility(0), riverSize(0), altitude(-99), type(' '), climate(' '),
    lake(false), river(false),
    tempNumber(9999), tempLink(NULL), tempUsed(false),
    lineDir1(NULL), lineDir2(NULL), lineDir3(NULL), lineDir4(NULL), lineDir5(NULL), lineDir6(NULL) {

    // presettings
    int size = game->getWindow()->getHexSize();

    // create a hex to put to the scene
    QVector<QPointF> hexPoints;
    hexPoints << QPointF(0.5, 0) << QPointF(1.5, 0) << QPointF(2, sqrt(3)/2) << QPointF(1.5, sqrt(3)) << QPointF(0.5, sqrt(3)) << QPointF(0, sqrt(3)/2);
    // scale the points
    for (int i = 0; i < hexPoints.size(); ++i) hexPoints[i] *= size;
    QPolygonF hexagon(hexPoints);
    setPolygon(hexagon);

    // allow special mouse events
    setAcceptHoverEvents(true);
}

void Hex::evaluateFertility() {
    QList<Hex*> neighbors = game->getWindow()->getHexNeighbors(col, row, 1, true);
    for (int k = 0; k < neighbors.size(); k++) fertility += neighbors.at(k)->getRiverSize();
}

void Hex::evaluateResources() {

}

void Hex::removeRivers() {
    river = false;
    riverSize = 0;
    if (lineDir1) { delete lineDir1; lineDir1 = NULL; }
    if (lineDir2) { delete lineDir2; lineDir2 = NULL; }
    if (lineDir3) { delete lineDir3; lineDir3 = NULL; }
    if (lineDir4) { delete lineDir4; lineDir4 = NULL; }
    if (lineDir5) { delete lineDir5; lineDir5 = NULL; }
    if (lineDir6) { delete lineDir6; lineDir6 = NULL; }
    foreach (River *river, game->getRivers(col, row)) {
        for (int riverPart = 0; riverPart < river->getWatercourse()->size(); riverPart++) {
            Hex *part = river->getWatercourse()->at(riverPart);
            if (part->getCol() == col && part->getRow() == row) river->getWatercourse()->removeAt(riverPart);
        }
    }
}

void Hex::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
}

void Hex::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
}

void Hex::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    game->getWindow()->setRightClick(false);
    game->getWindow()->setLeftClick(false);
    if (event->button() == Qt::RightButton) game->getWindow()->setRightClick(true);
    if (event->button() == Qt::LeftButton) game->getWindow()->setLeftClick(true);
    game->getWindow()->setMousePos(event->pos());
}

void Hex::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::RightButton) game->getWindow()->setRightClick(false);
    if (event->button() == Qt::LeftButton) game->getWindow()->setLeftClick(false);
}

void Hex::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
   if (game->getWindow()->getRightClick()) game->getWindow()->dragWorldMap(event);
}
