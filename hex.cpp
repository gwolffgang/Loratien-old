#include "hex.h"
#include "loratien.h"
#include <QLineF>
#include <QPainter>

extern Loratien *game;

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

Hex::Hex(QGraphicsItem *parent) : QGraphicsPolygonItem(parent),
    col(-1), row(-1), altitude(-99), type(' '),
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

void Hex::removeRivers() {
    river = false;
    if (lineDir1) {
        delete lineDir1;
        lineDir1 = NULL;
    }
    if (lineDir2) {
        delete lineDir2;
        lineDir2 = NULL;
    }
    if (lineDir3) {
        delete lineDir3;
        lineDir3 = NULL;
    }
    if (lineDir4) {
        delete lineDir4;
        lineDir4 = NULL;
    }
    if (lineDir5) {
        delete lineDir5;
        lineDir5 = NULL;
    }
    if (lineDir6) {
        delete lineDir6;
        lineDir6 = NULL;
    }
    QList<River*> rivers = game->getRivers(col, row);
    for (int riverNumber = 0; riverNumber < rivers.size(); riverNumber++) {
        for (int riverPart = 0; riverPart < rivers.at(riverNumber)->getWatercourse()->size(); riverPart++) {
            Hex *act = rivers.at(riverNumber)->getWatercourse()->at(riverPart);
            if (act->getCol() == col && act->getRow() == row)
                rivers.at(riverNumber)->getWatercourse()->removeAt(riverPart);
        }
    }
}
