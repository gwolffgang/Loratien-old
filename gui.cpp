#include "gui.h"
#include "loratien.h"

extern Loratien *game;

GUI::GUI(float x, float y, float w, float h, QGraphicsItem *parent) : QGraphicsRectItem(parent),
    posX(x), posY(y), height(h), width(w), selectedHex(NULL) {

    //create a GUI to put to the scene
    QGraphicsRectItem *rect;
    rect = new QGraphicsRectItem;
    setRect(posX, posY, width, height);

    selectedHex = new Hex();
    selectedHex->copyValuesFrom(game->getWorldMap()->at(1).at(1));
    //if (selectedHex) scene()->addItem(selectedHex);

    //allow responding to hover events
    setAcceptHoverEvents(true);
}
