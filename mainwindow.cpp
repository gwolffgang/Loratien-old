#include "loratien.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

extern Loratien *game;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    springs(new QList<Hex*>), lakes(new QList<Hex*>),
    ui(new Ui::MainWindow), scene(new QGraphicsScene(this)), screen(QGuiApplication::primaryScreen()),
    windowTitle("Loratien"), hexSize(20), maxRiverSize(hexSize/4), guiMenu(NULL), guiHexInfo(NULL) {

    setMouseTracking(true);

    // setup GUI
    ui->setupUi(this);
    desktop = screen->geometry();
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

int MainWindow::calculateAltitude(int altFactor) {
    QList<int> *altitudesList = game->getWorldAltitudesList();
    double randomFactor = (double) (rand()%1001) / 1000;
    double sum = (double) altitudesList->count(altFactor+1) + altitudesList->count(altFactor) + altitudesList->count(altFactor-1);
    if (sum > 0) {
        if (randomFactor < ((double) altitudesList->count(altFactor-1)) / (2*sum)) return altFactor-1;
        if (randomFactor < ((double) altitudesList->count(altFactor) + altitudesList->count(altFactor-1)) / (2*sum) + 0.5) return altFactor;
        return altFactor+1;
    }
    int higher = 0, lower = 0;
    for (int i = game->getWorldAltMin(); i < altFactor-1; i++) lower += altitudesList->count(i);
    for (int i = game->getWorldAltMax(); i > altFactor+1; i--) higher += altitudesList->count(i);
    if (lower < higher) return 98;
    else return -98;
}

void MainWindow::colorizeWorldMap() {
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    for (int col = 0; col < worldMap->size(); col++) {
        for (int row = 0; row < worldMap->at(col).size(); row++) {
            QBrush brush = QBrush(QColor(0, 0, 0), Qt::Dense3Pattern);
            if (!worldMap->at(col).at(row)->getLake()) {
                double alt = worldMap->at(col).at(row)->getAltitude();
                if (alt >= 4.5) {
                    brush.setColor(Qt::white);
                } else if (alt >= 2.5) {
                    int paint = (100 + 150 * ((alt - 2.5) / 2.0));
                    brush.setColor(QColor(paint, paint, paint));
                } else if (alt >= 0) {
                    int paint = (255 - 200 * (alt / 2.5));
                    brush.setColor(QColor(0, paint, 0));
                } else {
                    int paint = (255 - 255 * (alt / -4.0));
                    brush.setColor(QColor(0, 0, paint));
                }
            } else brush.setColor(Qt::blue);
            worldMap->at(col).at(row)->setBrush(brush);
        }
    }
}

void MainWindow::constructWorldMap() {
    QList<Hex*> hexesListCol;
    for (int col = 0; col < game->getWorldWidth(); col++) {
        hexesListCol.clear();
        for (int row = 0; row < game->getWorldHeight(); row++) {
            Hex *hex = new Hex;
            hex->setCol(col);
            hex->setRow(row);
            hex->setAltitude(-99);
            hex->setType("");
            hex->setClimate("");
            hex->setPos(hexSize * 1.5 * col, hexSize * sqrt(3) * row + sqrt(3)/2 * hexSize * (col%2));
            hexesListCol.append(hex);
            scene->addItem(hex);
        }
        // add row to worldMap
        game->getWorldMap()->append(hexesListCol);
    }
}

void MainWindow::dragWorldMap(QGraphicsSceneMouseEvent *event) {
    if (rightClick) {
        QRectF last = scene->sceneRect();
        QPointF pos = event->pos();
        QPointF change = mousePos - pos;
        scene->setSceneRect(last.x()+change.x(), last.y()+change.y(), desktop.width()-2, desktop.height()-2);
        if (scene->sceneRect().x() < 0) scene->setSceneRect(0, scene->sceneRect().y(), desktop.width()-2, desktop.height()-2);
        if (scene->sceneRect().y() < 0) scene->setSceneRect(scene->sceneRect().x(), 0, desktop.width()-2, desktop.height()-2);
        repositionGUI(scene->sceneRect().x(), scene->sceneRect().y());
    }
}

void MainWindow::evaluateHexes() {
    for (int col = 0; col < game->getWorldMap()->size(); col++) {
        for (int row = 0; row < game->getWorldMap()->at(col).size(); row++) {
            Hex *hex = game->getWorldMap()->at(col).at(row);
            hex->evaluateFertility();
            hex->evaluateResources();
        }
    }
}

void MainWindow::generateWorldMap(QList<QList<int>> *list) {
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    QList<int> *altitudesList = game->getWorldAltitudesList();
    int maxAltitude = game->getWorldAltMax();
    int minAltitude = game->getWorldAltMin();
    while(list->size() > 0) {
        int next = rand()%list->size();
        double altSum = 0;
        int altCount = 0;
        int hexCol = list->at(next).at(0);
        int hexRow = list->at(next).at(1);
        Hex *currentHex = NULL;
        QList<Hex*> neighbors = game->getWindow()->getHexNeighbors(hexCol, hexRow, 2, true);
        for (int k = 0; k < neighbors.size(); k++) {
            currentHex = neighbors.at(k);
            double alt = currentHex->getAltitude();
            if (alt > -90) {
                altSum += alt;
                altCount++;
            } else {
                if (alt == -99) {
                    list->append({currentHex->getCol(), currentHex->getRow()});
                    currentHex->setAltitude(-98);
                }
            }
        }
        int altFactor = 0;
        if (altCount != 0) altFactor = altSum / altCount - game->getWorldEarthStyle() * 3 * abs(game->getWorldHeight()/2 - hexRow) / game->getWorldHeight();
        if (worldMap->at(hexCol).at(hexRow)->getAltitude() < -90) {
            int alt = 1000;
            while (alt > maxAltitude || alt < minAltitude) {
                alt = calculateAltitude(altFactor);
                if (alt > maxAltitude) altFactor++;
                else if (alt < minAltitude) altFactor--;
            }
            worldMap->at(hexCol).at(hexRow)->setAltitude(alt);
            for (int i = 0; i < altitudesList->size(); i++) {
                if (altitudesList->at(i) == alt) {
                    altitudesList->removeAt(i);
                    break;
                }
            }
        }
        list->removeAt(next);
    }
}

QList<Hex*> MainWindow::getHexNeighbors(int hexCol, int hexRow, int radius, bool withOriginHex) {
    if (radius > 2) radius = 2;
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    QList<Hex*> neighbors;
    bool worldEarthStyle = game->getWorldEarthStyle();
    int worldHeight = game->getWorldHeight();
    int worldWidth = game->getWorldWidth();

    for (int modCol = -radius; modCol <= radius; modCol++) {
        for (int modRow = -radius; modRow <= radius; modRow++)  {
            if ((withOriginHex || modCol != 0 || modRow != 0)
                && (worldEarthStyle || (!worldEarthStyle && hexCol+modCol > -1 && hexCol+modCol < worldHeight))
                && hexRow+modRow > -1 && hexRow+modRow < worldHeight
                && !(radius == 1 && hexCol%2 == 1 && modRow == -1 && std::abs(modCol) == 1)
                && !(radius == 1 && hexCol%2 == 0 && modRow == 1 && std::abs(modCol) == 1)
                && !(radius == 2 && hexCol%2 == 1 && ((modRow == -2 && modCol != 0) || (modRow == 2 && std::abs(modCol) == 2)) )
                && !(radius == 2 && hexCol%2 == 0 && ((modRow == 2 && modCol != 0) || (modRow == -2 && std::abs(modCol) == 2)) ) ){
                    neighbors.append(worldMap->at((worldWidth+hexCol+modCol) % worldWidth).at(hexRow+modRow));
            }
        }
    }
    return neighbors;
}

void MainWindow::placeCities() {
    evaluateHexes();
}

void MainWindow::placeMountains(QList<QList<int>> *list) {
    QList<int> *altitudesList = game->getWorldAltitudesList();
    int maxAltitude = game->getWorldAltMax();
    int worldHeight = game->getWorldHeight();
    int worldWidth = game->getWorldWidth();
    if (altitudesList->count(maxAltitude) < game->getWorldMountains()) game->setWorldMountains(altitudesList->count(maxAltitude));
    for (int i = 0; i < game->getWorldMountains(); i++) {
        QList<int> hexesList;
        hexesList.append(rand()%worldWidth);
        if (!game->getWorldEarthStyle()) hexesList.append(rand()%worldHeight);
        else hexesList.append(worldHeight/4 + (rand() % (worldHeight/2)));
        game->getWorldMap()->at(hexesList.at(0)).at(hexesList.at(1))->setAltitude(maxAltitude);
        list->append(hexesList);
        for (int i = 0; i < altitudesList->size(); i++) {
            if (altitudesList->at(i) == maxAltitude) {
                altitudesList->removeAt(i);
                break;
            }
        }
    }
}

void MainWindow::placeOceans(QList<QList<int> > *list) {
    QList<int> *AltitudesList = game->getWorldAltitudesList();
    int MinAltitude = game->getWorldAltMin();
    if (AltitudesList->count(MinAltitude) < game->getWorldOceans()) game->setWorldOceans(AltitudesList->count(MinAltitude));
    for (int i = 0; i < game->getWorldOceans(); i++) {
        QList<int> hexesList;
        hexesList.append(rand()%game->getWorldWidth());
        hexesList.append(rand()%game->getWorldHeight());
        game->getWorldMap()->at(hexesList.at(0)).at(hexesList.at(1))->setAltitude(MinAltitude);
        list->append(hexesList);
        for (int i = AltitudesList->size()-1; i > -1; i--) {
            if (AltitudesList->at(i) == MinAltitude) {
                AltitudesList->removeAt(i);
                break;
            }
        }
    }
}

void MainWindow::placeRivers() {
    while (game->getWorldRivers() < springs->size()) springs->removeAt(rand()%springs->size());
    while (springs->size() > 0) {
        if (!springs->at(0)->getLake() && !springs->at(0)->getRiver()) {
            River *river = new River(springs->at(0));
            game->getRivers()->append(river);
        }
        springs->removeAt(0);
    }
}

void MainWindow::polishWorldMap() {
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    QList<QList<Hex*>> unpolished = *worldMap;
    bool worldEarthStyle = game->getWorldEarthStyle();
    int maxAltitude = game->getWorldAltMax();
    int minAltitude = game->getWorldAltMin();
    int worldHeight = game->getWorldHeight();
    int worldWidth = game->getWorldWidth();
    double alt = 0.0, highest = 0.0, lowest = 0.0;
    int count = 0;
    springs->clear();
    for (int worldCol = 0; worldCol < worldWidth; worldCol++) {
        for (int worldRow = 0; worldRow < worldHeight; worldRow++) {
            alt = 0.0;
            count = 0;
            for (int col = -1; col <= 1; col++) {
                for (int row = -1; row <= 1; row++) {
                    if ((worldEarthStyle || (!worldEarthStyle && worldCol+col > -1 && worldCol+col < worldWidth)) && worldRow+row > -1 && worldRow+row < worldHeight) {
                        if (row != 0 || col != 0) {
                            alt += unpolished.at((worldWidth + worldCol + col) % worldWidth).at(worldRow+row)->getAltitude();
                            count++;
                        }
                    }
                }
            }
            alt += count * unpolished.at(worldCol).at(worldRow)->getAltitude();
            count = count * 2;
            double newAlt = alt/count;
            worldMap->at(worldCol).at(worldRow)->setAltitude(newAlt);
            if (highest < alt/count) highest = newAlt;
            if (lowest > alt/count) lowest = newAlt;
        }
    }
    double stretchHigh = maxAltitude / highest, stretchLow = minAltitude / lowest;
    for (int worldCol = 0; worldCol < worldWidth; worldCol++) {
        for (int worldRow = 0; worldRow < worldHeight; worldRow++) {
            double newAlt = worldMap->at(worldCol).at(worldRow)->getAltitude();
            if (newAlt > 0) {
                worldMap->at(worldCol).at(worldRow)->setAltitude(newAlt * stretchHigh);
                if (maxAltitude-0.5 > newAlt * stretchHigh &&  newAlt * stretchHigh >= maxAltitude-1.5) springs->append(worldMap->at(worldCol).at(worldRow));
            } else worldMap->at(worldCol).at(worldRow)->setAltitude(newAlt * stretchLow);
        }
    }
}

void MainWindow::repositionGUI(int offspringX, int offspringY) {
    if (guiMenu) guiMenu->setRect(offspringX + guiMenu->getPosX(), offspringY + guiMenu->getPosY(), guiMenu->getWidth(), guiMenu->getHeight());
    if (guiHexInfo) guiHexInfo->setRect(offspringX + guiHexInfo->getPosX(), offspringY + guiHexInfo->getPosY(), guiHexInfo->getWidth(), guiHexInfo->getHeight());
}

void MainWindow::setupGUI() {
    guiMenu = new GUI(desktop.width()-2-desktop.width()*0.1, 0, desktop.width()*0.1, desktop.width()*0.2);
    guiMenu->setBrush(QBrush(Qt::darkGray, Qt::SolidPattern));
    scene->addItem(guiMenu);

    guiHexInfo = new GUI(0, 0, desktop.width()*0.1, desktop.width()*0.2);
    guiHexInfo->setBrush(QBrush(Qt::darkGray, Qt::SolidPattern));
    scene->addItem(guiHexInfo);
}

void MainWindow::setupWorldMap() {
    QList<QList<int>> list;
    placeMountains(&list);
    placeOceans(&list);
    generateWorldMap(&list);
    for (int i = 0; i < 5; i++) polishWorldMap();
    placeRivers();
    placeCities();
    colorizeWorldMap();
    //doubleUp();
}

void MainWindow::doubleUp() {
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    QList<Hex*> newCol;
    for (int i = 0; i < game->getWorldWidth(); i++) {
        newCol.clear();
        for (int k = 0; k < game->getWorldHeight(); k++) {
            Hex *newHex = new Hex;
            newHex->setCol(worldMap->at(i).at(k)->getCol() + i);
            newHex->setRow(k);
            newHex->setAltitude(worldMap->at(i).at(k)->getAltitude());
            newHex->setType("");
            newHex->setClimate("");
            newHex->setPos(hexSize * 1.5 * (game->getWorldWidth()+i), hexSize * sqrt(3) * k + sqrt(3)/2 * hexSize * ((game->getWorldWidth() + i) % 2));
            newCol.append(newHex);
            scene->addItem(newHex);
            worldMap->at(i).at(k);
            newCol.append(newHex);
        }
        worldMap->append(newCol);
    }
}
