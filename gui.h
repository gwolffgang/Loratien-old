#ifndef GUI_H
#define GUI_H

#include <QMouseEvent>

#include "hex.h"

class GUI : public QGraphicsRectItem {

private:
    // attributes
    int posX, posY;
    float height, width;
    Hex *selectedHex;

public:
    // constructor
    GUI(int x, int y, float w, float h, QGraphicsItem *parent = NULL);

    // getter
    inline float getHeight() {return height;}
    inline int getPosX() {return posX;}
    inline int getPosY() {return posY;}
    inline float getWidth() {return width;}

    // setter
    inline void setHeight(float newHeight) {height = newHeight;}
    inline void setPosX(int newPosX) {posX = newPosX;}
    inline void setPosY(int newPosY) {posY = newPosY;}
    inline void setWidth(float newWidth) {width = newWidth;}
};

#endif // GUI_H
