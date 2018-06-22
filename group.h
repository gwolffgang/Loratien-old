#ifndef GROUP_H
#define GROUP_H

#include "hex.h"

class Group : public QGraphicsPixmapItem {

private:
    QString name;
    Hex *location;

public:
    // constructors
    Group();

    // getter

    // methods
};

#endif // GROUP_H
