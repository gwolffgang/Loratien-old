#ifndef HEX_H
#define HEX_H

#include <QGraphicsPixmapItem>
#include <QBrush>
#include <QPen>

class Hex : public QGraphicsPixmapItem {

public:
    // constructors
    explicit Hex(int hexCol = -1, int hexRow = -1, QGraphicsItem *parent = NULL);

    // getter
    inline double getAltitude() const {return altitude;}
    inline QPen getBorder() const {return border;}
    inline QBrush getBrush() const {return brush;}
    inline QString getClimate() const {return climate;}
    inline int getCol() const {return col;}
    inline int getFertility() const {return fertility;}
    inline bool getFogOfWar() const {return fogOfWar;}
    inline bool getLake() const {return lake;}
    inline QGraphicsLineItem *getLineDir1() {return lineDir1;}
    inline QGraphicsLineItem *getLineDir2() {return lineDir2;}
    inline QGraphicsLineItem *getLineDir3() {return lineDir3;}
    inline QGraphicsLineItem *getLineDir4() {return lineDir4;}
    inline QGraphicsLineItem *getLineDir5() {return lineDir5;}
    inline QGraphicsLineItem *getLineDir6() {return lineDir6;}
    QList<Hex*> getNeighborHexes(int radius = 1, bool withOriginHex = false);
    inline QPixmap getPic() {return pic;}
    inline bool getRiver() const {return river;}
    inline int getRiverSize() const {return riverSize;}
    inline int getRow() const {return row;}
    inline int getTectonicDirection() const {return tectonicDirection;}
    inline int getTectonicPlate() const {return tectonicPlate;}
    inline Hex *getTempLink() {return tempLink;}
    inline double getTempNumber() const {return tempNumber;}
    inline bool getTempUsed() const {return tempUsed;}
    inline QString getType() const {return type;}
    inline bool getUsed() const {return used;}

    // setter
    inline void setAltitude(const double newAltitude) {altitude = newAltitude;}
    inline void setBorder(const QPen newBorder) {border = newBorder;}
    inline void setBrush(const QBrush newBrush) {brush = newBrush;}
    inline void setClimate(const QString newClimate) {climate = newClimate;}
    inline void setCol(const int newCol) {col = newCol;}
    inline void setFertility(const int newFertility) {fertility = newFertility;}
    inline void setFogOfWar(const bool newBool) {fogOfWar = newBool;}
    inline void setLake(const bool newBool) {lake = newBool;}
    inline void setLineDir1(QGraphicsLineItem *newLine) {lineDir1 = newLine;}
    inline void setLineDir2(QGraphicsLineItem *newLine) {lineDir2 = newLine;}
    inline void setLineDir3(QGraphicsLineItem *newLine) {lineDir3 = newLine;}
    inline void setLineDir4(QGraphicsLineItem *newLine) {lineDir4 = newLine;}
    inline void setLineDir5(QGraphicsLineItem *newLine) {lineDir5 = newLine;}
    inline void setLineDir6(QGraphicsLineItem *newLine) {lineDir6 = newLine;}
    inline void setPic(const QPixmap newPixmap) {pic = newPixmap;}
    inline void setRiver(const bool newBool) {river = newBool;}
    inline void setRiverSize(const int newSize) {riverSize = newSize;}
    inline void setRow(const int newRow) {row = newRow;}
    inline void setTectonicDirection(const int newDirection) {tectonicDirection = newDirection;}
    inline void setTectonicPlate(const int newPlate) {tectonicPlate = newPlate;}
    inline void setTempLink(Hex *temporaryLink) {tempLink = temporaryLink;}
    inline void setTempNumber(const double temporaryNumber) {tempNumber = temporaryNumber;}
    inline void setTempUsed(const bool temporaryUsage) {tempUsed = temporaryUsage;}
    inline void setType(const QString newType) {type = newType;}
    inline void setUsed(const bool usage) {used = usage;}

    // methods
    void draw(int size = -1);
    void evaluateFertility();
    void evaluateResources();
    void removeRivers();

protected:
    //events
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    // attributes
    QPixmap pic;
    QBrush brush;
    bool fogOfWar;
    QPen border;
    int col, row, tectonicPlate, tectonicDirection, fertility, riverSize;
    double altitude;
    QString type, climate;
    bool used, lake, river;
    double tempNumber;
    Hex *tempLink;
    bool tempUsed;
    QGraphicsLineItem *lineDir1, *lineDir2, *lineDir3, *lineDir4, *lineDir5, *lineDir6;
};

#endif // HEX_H
