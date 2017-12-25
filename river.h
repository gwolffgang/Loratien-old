#ifndef RIVER_H
#define RIVER_H

#include "hex.h"

class River {

private:
    QString name;
    QList<Hex*> *watercourse;
    unsigned int direction;
    QPointF north, northeast, northwest, south, southeast, southwest, center;
    QPen penRiver;

public:
    // constructors
    River(Hex *hex, int dir = 0);

    // getter
    inline QList<Hex*> *getWatercourse() {return watercourse;}

    // methods
    QList<Hex*> calculateWatercourse(Hex *spring);
    void drawRiver();
    Hex *nextRiverpartHex(QList<Hex*> *watercourse);
};

#endif // RIVER_H
