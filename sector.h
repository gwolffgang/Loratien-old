#ifndef SECTOR_H
#define SECTOR_H

#include <QGraphicsPixmapItem>
#include "hex.h"

class Sector : public QGraphicsPixmapItem {

public:
    // constructor
    explicit Sector(int sectorCol, int sectorRow, QGraphicsItem *parent = NULL);

    // getter
    inline double getAltitude() {return altitude;}
    inline int getCol() {return col;}
    inline QList<QList<Hex*>> *getHexes() {return hexes;}
    inline QPixmap getPic() {return pic;}
    inline int getRow() {return row;}

    // setter
    inline void setAltitude(double newAltitude) {altitude = newAltitude;}
    inline void setRegionCol(int newCol) {regionCol = newCol;}
    inline void setRegionRow(int newRow) {regionRow = newRow;}

    // methods
    void drawSector();

protected:
    // events
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    // variables
    QList<QList<Hex*>> *hexes;
    int regionCol, regionRow, col, row, radius;
    double altitude;
    QPixmap pic;
};

#endif // SECTOR_H
