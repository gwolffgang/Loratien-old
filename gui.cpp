#include "gui.h"
#include "loratien.h"

extern Loratien *game;

Gui::Gui(QString type, double x, double y, double w, double h, QGraphicsItem *parent) : QGraphicsPixmapItem(parent),
    posX(x), posY(y), width(w), height(h), selectedHexGui(NULL), selectedHexWorldMap(NULL) {
    pic = QPixmap(width, height);
    pic.fill(Qt::transparent);
    QPainter painter(&pic);
        painter.setPen(Qt::black);
        painter.setBrush(Qt::darkGray);
        painter.drawRect(posX, posY, width, height);
    painter.end();
    setPixmap(pic);

    //allow responding to hover events
    setAcceptHoverEvents(true);

    if (type == "guiHexInfo") setupGuiHexInfo();
    else if (type == "guiMenu") setupGuiMenu();

}

void Gui::setSelectedHex(Hex *hex) {
    if (selectedHexGui == NULL) selectedHexGui = new Hex(-1, -1, this);
    selectedHexWorldMap = hex;
    selectedHexGui->setAltitude(hex->getAltitude());
    selectedHexGui->setCol(hex->getCol());
    selectedHexGui->setRow(hex->getRow());
    selectedHexGui->setPic(hex->getPic());
    selectedHexGui->setBrush(hex->getBrush());
    selectedHexGui->setFogOfWar((hex->getFogOfWar()));
    selectedHexGui->draw((width-20)/2);
}

void Gui::setupGuiHexInfo() {
    Hex *selectedHexWorldMap = game->getPlayer()->getLocation();
    setSelectedHex(selectedHexWorldMap);
    selectedHexGui->setPos(posX+10, posY+10);
    selectedHexWorldMap->setBorder(QPen(Qt::red, 2, Qt::SolidLine));
    selectedHexWorldMap->draw();
}

void Gui::setupGuiMenu() {

}
