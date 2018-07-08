#include "loratien.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

extern Loratien *game;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow), scene(new QGraphicsScene(this)), screen(QGuiApplication::primaryScreen()),
    windowTitle("Loratien"), hexes(new QList<QList<Hex*>>), maxRiverSize(5), guiMenu(NULL), guiHexInfo(NULL) {
    setMouseTracking(true);

    // setup GUI
    ui->setupUi(this);
    desktop = screen->geometry();
    game->setHexSize((desktop.height()-10) / ((2*game->getPlayzoneRadius()+1.5) * sqrt(3)));
    setWindowTitle(windowTitle);

    // setup scene
    scene->setSceneRect(0, 0, desktop.width()-2, desktop.height()-2);
    ui->view->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    ui->view->setScene(scene);
    ui->view->setFixedSize(desktop.width(), desktop.height());
    ui->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Sizing Screen
    setGeometry(0, 0, desktop.width(), desktop.height());
    QMainWindow::showFullScreen();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::drawHexes(int radius, int hexSize) {
    int centerize = (desktop.width() - (3*radius+0.5)*hexSize) / 2;
    Hex *location = game->getGroup()->getLocation();
    int hexCol = location->getCol();
    int hexRow = location->getRow();
    int worldMapWidth = game->getWorldMapWidth();
    int worldMapHeight = game->getWorldMapHeight();
    for (int col = -radius; col <= radius; col++) {
        if (col+radius == worldMapWidth) break;
        for (int row = -radius; row <= radius; row++) {
            if ((game->getWorldEarthStyle() || (-1 < hexCol+col && hexCol+col < worldMapWidth)) && -1 < hexRow+row && hexRow+row < worldMapHeight - abs(col%2)*(hexCol%2)) {
                Hex *hex = game->getWorldMap()->at((worldMapWidth+hexCol+col)%worldMapWidth).at(hexRow+row + abs(col%2)*(hexCol%2));
                if (hex->getUsed()){
                    hex->setPos(centerize + hexSize * 1.5 * (col+radius), 5 + hexSize * sqrt(3) * (row+radius) + sqrt(3)/2 * hexSize * abs(col%2));
                    scene->addItem(hex);
                }
            }
        }
    }
}

void MainWindow::drawScreen() {
    //remove everything from scene
    QList<QGraphicsItem*> items = scene->items();
    foreach(QGraphicsItem *item, items) scene->removeItem(item);

    drawHexes(game->getPlayzoneRadius(), game->getHexSize());
    scene->addItem(guiMenu);
    guiHexInfo->getSelectedHexGui()->setParentItem(guiHexInfo);
    scene->addItem(guiHexInfo);
}

void MainWindow::drawSectors(int radius, int sectorSize) {
 /*   int centerize = (desktop.width() - (3*radius+0.5)*sectorSize) / 2;
    Hex *location = game->getGroup()->getLocation();
    int sectorCol = location->getCol();
    int sectorRow = location->getRow();
    int worldWidth = game->getWorldMapWidth();
    int worldHeight = game->getWorldMapHeight();
    QList<Sector*> sectorsCol;
    for (int col = -radius; col <= radius; col++) {
        if (col+radius == worldWidth) break;
        sectorsCol.clear();
        for (int row = -radius; row <= radius; row++) {
            if ((game->getWorldEarthStyle() || (-1 < hexCol+col && hexCol+col < worldWidth)) && -1 < hexRow+row && hexRow+row < worldHeight) {
                Hex *hex = game->getWorldMap()->at((worldWidth+hexCol+col)%worldWidth).at(hexRow+row + abs(col%2)*(hexCol%2));
                hex->setPos(centerize + hexSize * 1.5 * (col+radius), 5 + hexSize * sqrt(3) * (row+radius) + sqrt(3)/2 * hexSize * abs(col%2));
                hexesCol.append(hex);
                scene->addItem(hex);
            }
        }
        hexes->append(hexesCol);
    }*/
}

void MainWindow::setupGui() {
    guiMenu = new Gui("guiMenu", desktop.width()*0.9 -2, 0, desktop.width()*0.1, desktop.height()*0.2);
    guiHexInfo = new Gui("guiHexInfo", 0, 0, desktop.width()*0.1, desktop.height()*0.2);
}
