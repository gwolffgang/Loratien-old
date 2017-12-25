#ifndef GROUP_H
#define GROUP_H

#include <QGraphicsRectItem>

#include "hex.h"

class Group : public QGraphicsRectItem {

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
