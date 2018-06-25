#ifndef GUI_H
#define GUI_H

#include "hex.h"

class Gui : public QGraphicsPixmapItem {

private:
    // attributes
    QPixmap pic;
    double width, height;
    Hex *selectedHexGui, *selectedHexWorldMap;

public:
    // constructor
    Gui(double x, double y, double w, double h, QGraphicsItem *parent = NULL);

    // getter
    inline double getHeight() {return height;}
    inline Hex *getSelectedHexGui() {return selectedHexGui;}
    inline Hex *getSelectedHexWorldMap() {return selectedHexWorldMap;}
    inline double getWidth() {return width;}

    // setter
    inline void setHeight(double newHeight) {height = newHeight;}
    void setSelectedHex(Hex *hex);
    inline void setWidth(double newWidth) {width = newWidth;}
};

#endif // GUI_H
