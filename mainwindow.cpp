#include "loratien.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

extern Loratien *game;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), springs(new QList<Hex*>), lakes(new QList<Hex*>),
    ui(new Ui::MainWindow), scene(new QGraphicsScene(this)), screen(QGuiApplication::primaryScreen()),
    windowTitle("Loratien"), hexSize(5), maxRiverSize(hexSize/4), percentMountain(0.12), percentOcean(0.65),
    guiMenu(NULL), guiHexInfo(NULL) {

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

double MainWindow::getPercentAlt(double percent) {
    if (percent > 1.0) percent = 1.0;
    else if (percent < 0.0) percent = 0.0;
    QList<double> list;
    for (int x = 0; x < game->getWorldWidth(); x++) {
        for (int y = 0; y < game->getWorldHeight(); y++) {
            list.append(game->getWorldMap()->at(x).at(y)->getAltitude());
        }
    }
    std::stable_sort(list.begin(), list.end());
    int elem = (list.size()-1)*percent;
    return list.at(elem);
}

void MainWindow::colorizePlates() {
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    for (int col = 0; col < game->getWorldWidth(); col++) {
        for (int row = 0; row < game->getWorldHeight(); row++) {
            QBrush brush = QBrush(QColor(0, 0, 0), Qt::SolidPattern);
            double alt = worldMap->at(col).at(row)->getTectonicPlate();
            int paint = (100 + 150 * (alt / game->getWorldTectonicPlates()));
            brush.setColor(QColor(paint, paint, paint));
            if (alt == -1) brush.setColor(Qt::red);
            worldMap->at(col).at(row)->draw(brush);
        }
    }
}

void MainWindow::colorizeWorldMap() {
    int minAlt = game->getWorldAltMin();
    int maxAlt = game->getWorldAltMax();
    double snowLevel = getPercentAlt(1-((1-percentOcean) * 0.02));
    double mountainLevel = getPercentAlt(1-((1-percentOcean) * percentMountain));
    double waterLevel = getPercentAlt(percentOcean);
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    for (int col = 0; col < worldMap->size(); col++) {
        for (int row = 0; row < worldMap->at(col).size(); row++) {
            QBrush brush = QBrush(QColor(0, 0, 0), Qt::SolidPattern);
            if (!worldMap->at(col).at(row)->getLake()) {
                int paint = 0;
                double alt = worldMap->at(col).at(row)->getAltitude();
                if (alt >= snowLevel) {
                    brush.setColor(Qt::white);
                } else if (alt >= mountainLevel) {
                    paint = 100 + 150 * ((alt - mountainLevel) / (maxAlt-mountainLevel));
                    brush.setColor(QColor(paint, paint, paint));
                } else if (alt >= 0) {
                    paint = 255 - 200 * (alt / (mountainLevel-waterLevel));
                    brush.setColor(QColor(0, paint, 0));
                } else {
                    paint = 255 - 255 * (alt / minAlt);
                    brush.setColor(QColor(0, 0, paint));
                }
            } else brush.setColor(Qt::blue);
            worldMap->at(col).at(row)->draw(brush);
        }
    }
}

void MainWindow::constructWorldMap() {
    QList<Hex*> hexesCol;
    for (int col = 0; col < game->getWorldWidth(); col++) {
        hexesCol.clear();
        for (int row = 0; row < game->getWorldHeight(); row++) {
            Hex *hex = new Hex;
            hex->setCol(col);
            hex->setRow(row);
            hex->setPos(hexSize * 1.5 * col, hexSize * sqrt(3) * row + sqrt(3)/2 * hexSize * (col%2));
            hexesCol.append(hex);
            scene->addItem(hex);
        }
        // add row to worldMap
        game->getWorldMap()->append(hexesCol);
    }
}

void MainWindow::dragWorldMap(QGraphicsSceneMouseEvent *event) {
    if (rightClick) {
        QRectF previousPoint = scene->sceneRect();
        QPointF posMouse = event->pos();
        QPointF change = mousePos - posMouse;
        scene->setSceneRect(previousPoint.x() + change.x(), previousPoint.y() + change.y(), desktop.width()-2, desktop.height()-2);
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

void MainWindow::generateWorldMap() {
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    QList<Hex*> hexesList;
    Hex *currentHex = NULL;
    int worldHeight = game->getWorldHeight();
    int worldWidth = game->getWorldWidth();
    for (int k = 0; k < game->getWorldTectonicPlates(); k++) {
        int hexCol = rand() % worldWidth;
        int hexRow = rand() % worldHeight;
        currentHex = worldMap->at(hexCol).at(hexRow);
        if (-1 == currentHex->getTectonicPlate()) {
            currentHex->setTectonicPlate(k);
            currentHex->setTectonicDirection(rand()%6+1);
            currentHex->setAltitude(rand()%20 + 6);
            hexesList.append(currentHex);
        } else k--;
    }
    while(hexesList.size() > 0) {
        int randItem = rand()%hexesList.size();
        Hex *nextHex = hexesList.at(randItem);
        double alt = nextHex->getAltitude();
        double altSum = 0;
        int altCount = 0;
        int hexRow = nextHex->getRow();
        QList<Hex*> neighbors = nextHex->getNeighborHexes(2);
        for (int k = 0; k < neighbors.size(); k++) {
            currentHex = neighbors.at(k);
            double currentAlt = currentHex->getAltitude();
            if (currentAlt > -50) {
                altSum += currentAlt;
                altCount++;
            } else if (currentAlt == -99) {
                    hexesList.append(currentHex);
                    currentHex->setTectonicPlate(nextHex->getTectonicPlate());
                    currentHex->setTectonicDirection(nextHex->getTectonicDirection());
                    currentHex->setAltitude(-95);
                }
        }
        if (altCount != 0) {
            alt = altSum / altCount - game->getWorldEarthStyle() * 3 * abs(worldHeight/2 - hexRow) / worldHeight;
            nextHex->setAltitude(alt + rand()%11 - 5);
        }
        hexesList.removeAt(randItem);
    }
}

void MainWindow::placeCities() {
    evaluateHexes();
}

void MainWindow::placeRivers() {
    double maxAlt = getPercentAlt(1);
    double minAlt = getPercentAlt(0.99);
    for (int col = 0; col < game->getWorldWidth(); col++) {
        for (int row = 0; row < game->getWorldHeight(); row++) {
            Hex *currentHex = game->getWorldMap()->at(col).at(row);
            double currentAlt = currentHex->getAltitude();
            if (currentAlt >= minAlt && maxAlt >= currentAlt) springs->append(currentHex);
        }
    }
    while (game->getWorldRivers() < springs->size()) springs->removeAt(rand()%springs->size());
    while (springs->size() > 0) {
        int nextHex = rand()%springs->size();
        Hex *spring = springs->at(nextHex);
        int hexCol = spring->getCol();
        int hexRow = spring->getRow();
        if (!spring->getLake() && !spring->getRiver()) {
            if (!((!game->getWorldEarthStyle() && (hexCol == 0 || hexCol == game->getWorldWidth()-1)) || hexRow == 0 || hexRow == game->getWorldHeight()-1)) {
                River *river = new River(spring);
                game->getRivers()->append(river);
            }
        }
        springs->removeAt(nextHex);
    }
}

void MainWindow::polishWorldMap() {
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    bool worldEarthStyle = game->getWorldEarthStyle();
    int maxAltitude = game->getWorldAltMax();
    int minAltitude = game->getWorldAltMin();
    int worldHeight = game->getWorldHeight();
    int worldWidth = game->getWorldWidth();
    double alt = 0.0;
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
                            alt += worldMap->at((worldWidth + worldCol + col) % worldWidth).at(worldRow+row)->getAltitude();
                            count++;
                        }
                    }
                }
            }
            alt += count * worldMap->at(worldCol).at(worldRow)->getAltitude();
            count = count * 2;
            worldMap->at(worldCol).at(worldRow)->setAltitude(alt/count);
        }
    }
    double stretchHigh = 0.0;
    double highest = getPercentAlt(1);
    if (highest != 0) stretchHigh = maxAltitude / highest;
    double stretchLow = 0.0;
    double lowest = getPercentAlt(0);
    if (lowest != 0) stretchLow = minAltitude / lowest;
    for (int worldCol = 0; worldCol < worldWidth; worldCol++) {
        for (int worldRow = 0; worldRow < worldHeight; worldRow++) {
            double newAlt = worldMap->at(worldCol).at(worldRow)->getAltitude();
            if (newAlt > 0) {
                double stretchedHeight = newAlt * stretchHigh;
                worldMap->at(worldCol).at(worldRow)->setAltitude(stretchedHeight);
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
    #ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
    #endif
    generateWorldMap();
    simulateTectonicMovement(4);
    translateValuesToWorldMap();
    for (int times = 0; times < 3; times++) polishWorldMap();
    //placeRivers();
    //placeCities();
    colorizePlates();
    colorizeWorldMap();
    #ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
}

void MainWindow::simulateTectonicMovement(int range) {
    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    bool earthStyle = game->getWorldEarthStyle();
    int worldHeight = game->getWorldHeight();
    int worldWidth = game->getWorldWidth();
    Hex *currentHex = NULL;
    Hex *neighborHex = NULL;
    for (int col = 0; col < game->getWorldWidth(); col++) {
        for (int row = 0; row < game->getWorldHeight(); row++) {
            currentHex = worldMap->at(col).at(row);
            currentHex->setTempNumber(-999);
            neighborHex = NULL;
            int neighborCol = col, neighborRow = row;
            switch (currentHex->getTectonicDirection()) {
            case 1:
                neighborRow = row - range;
                break;
            case 2:
                neighborCol = col + range;
                neighborRow = row-ceil((double)range/2)+(col%2)*(range%2);
                break;
            case 3:
                neighborCol = col + range;
                neighborRow = row+range/2+(col%2)*(range%2);
                break;
            case 4:
                neighborRow = row + range;
                break;
            case 5:
                neighborCol = col - range;
                neighborRow = row+range/2+(col%2)*(range%2);
                break;
            case 6:
                neighborCol = col - range;
                neighborRow = row-ceil((double)range/2)+(col%2)*(range%2);
            }
            if ((earthStyle || (-1 < neighborCol && neighborCol < worldWidth)) && -1 < neighborRow && neighborRow < worldHeight) {
                neighborHex = worldMap->at((worldWidth+neighborCol)%worldWidth).at(neighborRow);
                if (currentHex->getTectonicDirection() != neighborHex->getTectonicDirection())
                    currentHex->setAltitude(range/2*(currentHex->getAltitude()+neighborHex->getAltitude()));
            }
        }
    }

/*    QList<QList<Hex*>> *worldMap = game->getWorldMap();
    QList<Hex*> currentPlate;
    for (int plate = 0; plate < game->getWorldTectonicPlates(); plate++) {
        for (int col = 0; col < game->getWorldWidth(); col++) {
            for (int row = 0; row < game->getWorldHeight(); row++) {
                Hex *currentHex = worldMap->at(col).at(row);
                currentHex->setTempNumber(-99);
                if (currentHex->getTectonicPlate() == plate) {
                    foreach (Hex* neighbor, currentHex->getNeighborHexes(2)) {
                        if (neighbor->getTectonicPlate() != plate) {
                            currentHex->setTempNumber(std::max(currentHex->getTempNumber(),neighbor->getAltitude()));
                            currentPlate.append(currentHex);
                            currentHex->setAltitude(currentHex->getAltitude() + currentHex->getTempNumber());
                            break;
                        }
                    }
                }
            }
        }
    }*/
}

void MainWindow::translateValuesToWorldMap() {
    double waterLevel = getPercentAlt(percentOcean);
    double minValue = getPercentAlt(0.0);
    double maxValue = getPercentAlt(1.0);
    for (int x = 0; x < game->getWorldWidth(); x++) {
        for (int y = 0; y < game->getWorldHeight(); y++) {
            double alt = game->getWorldMap()->at(x).at(y)->getAltitude() - waterLevel;
            if (alt > 0) alt = game->getWorldAltMax() / maxValue * alt;
            else alt = game->getWorldAltMin() / minValue * alt;
            game->getWorldMap()->at(x).at(y)->setAltitude(alt);
        }
    }
}
