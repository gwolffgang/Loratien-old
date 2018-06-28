#ifndef GROUP_H
#define GROUP_H

#include "hex.h"
#include "char.h"

class Group : public QGraphicsPixmapItem {

private:
    QString name;
    QList<Char*> *members;
    Hex *location;

public:
    // constructors
    Group(Char *leader);

    // getter
    inline Hex *getLocation() {return location;}
    inline QList<Char*> *getMembers() {return members;}
    inline QString getName() {return name;}

    //setter
    inline void setLocation(Hex *newLocation) {location = newLocation;}

    // methods
};

#endif // GROUP_H
