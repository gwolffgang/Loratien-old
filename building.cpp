#include "building.h"
#include "loratien.h"

Building::Building(QGraphicsItem *parent) : QGraphicsPixmapItem(parent),
    type(""), location(NULL), owner(new QList<Char*>) {

}
