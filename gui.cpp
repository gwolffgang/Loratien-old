#include "gui.h"
#include "loratien.h"

extern Loratien *game;

Gui::Gui(double x, double y, double w, double h, QGraphicsItem *parent) : QGraphicsPixmapItem(parent),
    width(w), height(h), selectedHexGui(NULL), selectedHexWorldMap(NULL) {
    pic = QPixmap(width, height);
    pic.fill(Qt::transparent);
    QPainter painter(&pic);
        painter.setPen(Qt::black);
        painter.setBrush(Qt::darkGray);
        painter.drawRect(x, y, width, height);
    painter.end();
    setPixmap(pic);

    selectedHexGui = new Hex(this);
    Hex *selectedHexWorldMap = game->getWorldMap()->at(27).at(17);
    setSelectedHex(selectedHexWorldMap);
    selectedHexWorldMap->setBorder(QPen(Qt::red, 2, Qt::SolidLine));
    selectedHexWorldMap->draw();
    selectedHexGui->setPos(x+10, y+10);
    selectedHexGui->draw((width-20)/2);

    //allow responding to hover events
    setAcceptHoverEvents(true);
}

void Gui::setSelectedHex(Hex *hex) {
    selectedHexWorldMap = hex;
    selectedHexGui->setAltitude(hex->getAltitude());
    selectedHexGui->setCol(hex->getCol());
    selectedHexGui->setRow(hex->getRow());
    selectedHexGui->setPic(hex->getPic());
    selectedHexGui->setBrush(hex->getBrush());
}
