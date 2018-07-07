#ifndef REGION_H
#define REGION_H

#include <QGraphicsPixmapItem>
#include "sector.h"

class Region : public QGraphicsPixmapItem {

public:
    // constructor
    explicit Region(int regionCol = -1, int regionRow = -1, QGraphicsItem *parent = NULL);

    // getter
    inline QPen getBorder() const {return border;}
    inline QBrush getBrush() const {return brush;}
    inline int getCol() {return col;}
    inline bool getFogOfWar() {return fogOfWar;}
    QList<Region*> getNeighborRegions(int radius = 1, bool withOriginRegion = false);
    inline QPixmap getPic() {return pic;}
    inline int getRow() {return row;}
    inline QList<QList<Sector*>> *getSectors() {return sectors;}
    inline int getTectonicDirection() const {return tectonicDirection;}
    inline int getTectonicPlate() const {return tectonicPlate;}
    inline Region *getTempLink() {return tempLink;}
    inline double getTempNumber() const {return tempNumber;}
    inline bool getTempUsed() const {return tempUsed;}

    // setter
    inline void setBorder(const QPen newBorder) {border = newBorder;}
    inline void setBrush(const QBrush newBrush) {brush = newBrush;}
    inline void setCol(const int newCol) {col = newCol;}
    inline void setFogOfWar(const bool newBool) {fogOfWar = newBool;}
    inline void setPic(const QPixmap newPic) {pic = newPic;}
    inline void setRow(const int newRow) {row = newRow;}
    inline void setTectonicDirection(const int newDirection) {tectonicDirection = newDirection;}
    inline void setTectonicPlate(const int newPlate) {tectonicPlate = newPlate;}
    inline void setTempLink(Region *temporaryLink) {tempLink = temporaryLink;}
    inline void setTempNumber(const double temporaryNumber) {tempNumber = temporaryNumber;}
    inline void setTempUsed(const bool temporaryUsage) {tempUsed = temporaryUsage;}

    // methods
    void draw(int factor = 1);

protected:
    // events
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    // variables
    int col, row, radius, tectonicPlate, tectonicDirection;
    QList<QList<Sector*>> *sectors;
    QPixmap pic;
    QBrush brush;
    QPen border;
    bool fogOfWar;
    double tempNumber;
    Region *tempLink;
    bool tempUsed;
};
#endif // REGION_H
