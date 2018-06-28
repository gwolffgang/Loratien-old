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
    int bbb = (desktop.height()-10) / ((2*game->getHexesRadius()+1.5) * sqrt(3));
    game->setHexSize(bbb);
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

void MainWindow::dragWorldMap(QGraphicsSceneMouseEvent *event) {
    if (rightClick) {
        QRectF previousPoint = scene->sceneRect();
        QPointF posMouse = event->pos();
        QPointF change = mousePos - posMouse;
        scene->setSceneRect(previousPoint.x() + change.x(), previousPoint.y() + change.y(), desktop.width()-2, desktop.height()-2);
        if (scene->sceneRect().x() < 0) scene->setSceneRect(0, scene->sceneRect().y(), desktop.width()-2, desktop.height()-2);
        if (scene->sceneRect().y() < 0) scene->setSceneRect(scene->sceneRect().x(), 0, desktop.width()-2, desktop.height()-2);
        repositionGui(scene->sceneRect().x(), scene->sceneRect().y());
    }
}

void MainWindow::drawHexes(int radius, int hexSize) {
    int centerize = (desktop.width() - (3*radius+0.5)*hexSize) / 2;
    Hex *location = game->getGroup()->getLocation();
    int hexCol = location->getCol();
    int hexRow = location->getRow();
    int worldWidth = game->getWorldWidth();
    int worldHeight = game->getWorldHeight();
    QList<Hex*> hexesCol;
    for (int col = -radius; col <= radius; col++) {
        if (col+radius == game->getWorldWidth()) break;
        hexesCol.clear();
        for (int row = -radius; row <= radius; row++) {
            if ((game->getWorldEarthStyle() || (-1 < hexCol+col && hexCol+col < worldWidth)) && -1 < hexRow+row && hexRow+row < worldHeight) {
                Hex *hex = game->getWorldMap()->at((worldWidth+hexCol+col)%worldWidth).at(hexRow+row);
                hex->setPos(centerize + hexSize * 1.5 * (col+radius), 5 + hexSize * sqrt(3) * (row+radius) + sqrt(3)/2 * hexSize * (std::abs(col)%2));
                hexesCol.append(hex);
                scene->addItem(hex);
            }
        }
        hexes->append(hexesCol);
    }
}

void MainWindow::refresh() {
    if (guiHexInfo) {
        guiHexInfo->getSelectedHexGui()->draw((guiHexInfo->getWidth()-20)/2);
    }
}

void MainWindow::repositionGui(double offspringX, double offspringY) {
    if (guiMenu) guiMenu->setPos(offspringX, offspringY);
    if (guiHexInfo) guiHexInfo->setPos(offspringX, offspringY);
}

void MainWindow::setupGui() {
    //guiMenu = new Gui(desktop.width()-2-desktop.width()*0.1, 0, desktop.width()*0.1, desktop.height()*0.2);
    //scene->addItem(guiMenu);

    guiHexInfo = new Gui(0, 0, desktop.width()*0.1, desktop.height()*0.2);
    scene->addItem(guiHexInfo);
}
