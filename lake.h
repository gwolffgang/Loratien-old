#ifndef LAKE_H
#define LAKE_H

#include "hex.h"

class Lake {

private:
    QString name;
    QList<Hex*> *dimension;

public:
    // constructors
    Lake(Hex *hex);

    // getter
    inline QList<Hex*> *getDimension() {return dimension;}

    // methods
    bool addExistingLakeOf(Hex *hex);
    Hex *checkRiver(Hex *lowest);
    void expandLake();
    bool isAlreadyInThisLake(Hex *hex);
};

#endif // LAKE_H
