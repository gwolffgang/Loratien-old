#ifndef GUI_H
#define GUI_H

#include <QMouseEvent>

#include "hex.h"

class GUI : public QGraphicsRectItem {

private:
    // attributes
    float posX, posY, height, width;
    Hex *selectedHex;

public:
    // constructor
    GUI(float x, float y, float w, float h, QGraphicsItem *parent = NULL);

    // getter
    inline float getHeight() {return height;}
    inline float getPosX() {return posX;}
    inline float getPosY() {return posY;}
    inline float getWidth() {return width;}

    // setter
    inline void setHeight(float newHeight) {height = newHeight;}
    inline void setPosX(float newPosX) {posX = newPosX;}
    inline void setPosY(float newPosY) {posY = newPosY;}
    inline void setWidth(float newWidth) {width = newWidth;}
};

#endif // GUI_H
