#ifndef HEX_H
#define HEX_H

#include <QMouseEvent>
#include <QGraphicsPolygonItem>

class Hex : public QGraphicsPolygonItem {

public:
    // constructors
    explicit Hex(QGraphicsItem *parent = NULL);

    // getter
    inline double getAltitude() {return altitude;}
    inline QString getClimate() {return climate;}
    inline int getCol() {return col;}
    inline int getFertility() {return fertility;}
    inline bool getLake() {return lake;}
    inline QGraphicsLineItem *getLineDir1() {return lineDir1;}
    inline QGraphicsLineItem *getLineDir2() {return lineDir2;}
    inline QGraphicsLineItem *getLineDir3() {return lineDir3;}
    inline QGraphicsLineItem *getLineDir4() {return lineDir4;}
    inline QGraphicsLineItem *getLineDir5() {return lineDir5;}
    inline QGraphicsLineItem *getLineDir6() {return lineDir6;}
    inline bool getRiver() {return river;}
    inline int getRiverSize() {return riverSize;}
    inline int getRow() {return row;}
    inline Hex *getTempLink() {return tempLink;}
    inline double getTempNumber() {return tempNumber;}
    inline bool getTempUsed() {return tempUsed;}
    inline QString getType() {return type;}

    // setter
    inline void setAltitude(double newAltitude) {altitude = newAltitude;}
    inline void setClimate(QString newClimate) {climate = newClimate;}
    inline void setCol(int newCol) {col = newCol;}
    inline void setFertility(int newFertility) {fertility = newFertility;}
    inline void setLake(bool newBool) {lake = newBool;}
    inline void setLineDir1(QGraphicsLineItem *newLine) {lineDir1 = newLine;}
    inline void setLineDir2(QGraphicsLineItem *newLine) {lineDir2 = newLine;}
    inline void setLineDir3(QGraphicsLineItem *newLine) {lineDir3 = newLine;}
    inline void setLineDir4(QGraphicsLineItem *newLine) {lineDir4 = newLine;}
    inline void setLineDir5(QGraphicsLineItem *newLine) {lineDir5 = newLine;}
    inline void setLineDir6(QGraphicsLineItem *newLine) {lineDir6 = newLine;}
    inline void setRiver(bool newBool) {river = newBool;}
    inline void setRiverSize(int newSize) {riverSize = newSize;}
    inline void setRow(int newRow) {row = newRow;}
    inline void setTempLink(Hex *temporaryLink) {tempLink = temporaryLink;}
    inline void setTempNumber(double temporaryNumber) {tempNumber = temporaryNumber;}
    inline void setTempUsed(bool temporaryUsage) {tempUsed = temporaryUsage;}
    inline void setType(QString newType) {type = newType;}

    // methods
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
    // variables
    int col, row, fertility, riverSize;
    double altitude;
    QString type, climate;
    bool lake, river;
    double tempNumber;
    Hex *tempLink;
    bool tempUsed;
    QGraphicsLineItem *lineDir1, *lineDir2, *lineDir3, *lineDir4, *lineDir5, *lineDir6;
};

#endif // HEX_H
