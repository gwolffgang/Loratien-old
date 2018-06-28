#ifndef BUILDING_H
#define BUILDING_H

#include "hex.h"
#include "char.h"

class Building: public QGraphicsPixmapItem {

public:
    // constructor
    explicit Building(QGraphicsItem *parent = NULL);

    // getter
    inline Hex *getLocation() {return location;}
    inline QString getType() {return type;}

    // setter
    inline void setLocation(Hex *newLocation) {location = newLocation;}
    inline void setType(QString newType) {type = newType;}

    // methods

private:
    // attributes
    QString type;
    Hex *location;
    QList<Char*> *owner;
};

#endif // BUILDING_H
