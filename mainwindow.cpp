#include "loratien.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

extern Loratien *game;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    hexSize = 30;
    springs = new QList<Hex*>;
    lakes = new QList<Hex*>;
    guiMenu = guiHexInfo = NULL;
    setMouseTracking(true);

    // setup GUI
    ui->setupUi(this);
    screen = QGuiApplication::primaryScreen();
    desktop = screen->geometry();
    windowTitle = "Loratien"; setWindowTitle(windowTitle);

    // setup scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, desktop.width()-2, desktop.height()-2);
    ui->view->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    ui->view->setScene(scene);
    ui->view->setFixedSize(desktop.width(), desktop.height());
    ui->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Sizing Screen
    setGeometry(0,0,desktop.width(),desktop.height());
    QMainWindow::showFullScreen();
}

MainWindow::~MainWindow() {
    delete ui;
}

int MainWindow::calculateAltitude(int altFactor) {
    double randomFactor = (double) (rand()%1001) / 1000;
    double sum = (double) game->getWorldAltitudesList()->count(altFactor+1) + game->getWorldAltitudesList()->count(altFactor) + game->getWorldAltitudesList()->count(altFactor-1);
    if (sum > 0) {
        if (randomFactor < ((double) game->getWorldAltitudesList()->count(altFactor-1)) / (2*sum)) return altFactor-1;
        if (randomFactor < ((double) game->getWorldAltitudesList()->count(altFactor) + game->getWorldAltitudesList()->count(altFactor-1)) / (2*sum) + 0.5) return altFactor;
        return altFactor+1;
    }
    int higher = 0, lower = 0;
    for (int i = -4; i < altFactor-1; i++) lower += game->getWorldAltitudesList()->count(i);
    for (int i = 5; i > altFactor+1; i--) higher += game->getWorldAltitudesList()->count(i);
    if (lower < higher) return 98;
    else return -98;
}

void MainWindow::colorizeWorldMap() {
    for (int col = 0; col < game->getWorldMap()->size(); col++) {
        for (int row = 0; row < game->getWorldMap()->at(col).size(); row++) {
            QBrush brush = QBrush(QColor(0, 0, 0), Qt::Dense3Pattern);
            if (!game->getWorldMap()->at(col).at(row)->getLake()) {
                double alt = game->getWorldMap()->at(col).at(row)->getAltitude();
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
            game->getWorldMap()->at(col).at(row)->setBrush(brush);
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
            hex->setAltitude(-99);
            hex->setType("");
            hex->setClimate("");
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
        QRectF last = scene->sceneRect();
        QPointF pos = event->pos();
        QPointF change = mousePos - pos;
        scene->setSceneRect(last.x()+change.x(), last.y()+change.y(), desktop.width()-2, desktop.height()-2);
        if (scene->sceneRect().x() < 0) scene->setSceneRect(0, scene->sceneRect().y(), desktop.width()-2, desktop.height()-2);
        if (scene->sceneRect().y() < 0) scene->setSceneRect(scene->sceneRect().x(), 0, desktop.width()-2, desktop.height()-2);
        repositionGUI(scene->sceneRect().x(), scene->sceneRect().y());
    }
}

void MainWindow::generateWorldMap(QList<QList<int>> *list) {
    while(list->size() > 0) {
        int next = rand()%list->size();
        double altSum = 0;
        int altCount = 0;
        int hexCol = list->at(next).at(0);
        int hexRow = list->at(next).at(1);
        for (int col = -2; col <= 2; col++) {
            for (int row = -2; row <= 2; row++) {
                if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && hexCol+col > -1 && hexCol+col < game->getWorldWidth())) && hexRow+row > -1 && hexRow+row < game->getWorldHeight()) {
                    double alt = game->getWorldMap()->at((game->getWorldWidth()+(hexCol+col)) % game->getWorldWidth()).at(hexRow+row)->getAltitude();
                    if (alt > -90) {
                        altSum += alt;
                        altCount++;
                    } else {
                        if (alt == -99) {
                            list->append({(game->getWorldWidth()+(hexCol+col)) % game->getWorldWidth(), hexRow+row});
                            game->getWorldMap()->at((game->getWorldWidth()+(hexCol+col)) % game->getWorldWidth()).at(hexRow+row)->setAltitude(-98);
                        }
                    }
                }
            }
        }
        int altFactor = altSum / altCount - game->getWorldEarthStyle() * 3 * abs(game->getWorldHeight()/2 - hexRow) / game->getWorldHeight();
        if (game->getWorldMap()->at(hexCol).at(hexRow)->getAltitude() < -90) {
            int alt = 1000;
            while (alt > game->getWorldAltMax() || alt < game->getWorldAltMin()) {
                alt = calculateAltitude(altFactor);
                if (alt > game->getWorldAltMax()) altFactor++;
                else if (alt < game->getWorldAltMin()) altFactor--;
            }
            game->getWorldMap()->at(hexCol).at(hexRow)->setAltitude(alt);
            for (int i = 0; i < game->getWorldAltitudesList()->size(); i++) {
                if (game->getWorldAltitudesList()->at(i) == alt) {
                    game->getWorldAltitudesList()->removeAt(i);
                    break;
                }
            }
        }
        list->removeAt(next);
    }
}

bool MainWindow::neighbor(int mapCol, int modCol, int mapRow, int modRow) {
    if ((modCol != 0 || modRow != 0)
        && (game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && mapCol+modCol > -1 && mapCol+modCol < game->getWorldHeight()))
        && mapRow+modRow > -1 && mapRow+modRow < game->getWorldWidth()
        && !(mapCol%2==1 && (modRow == -1) && (modCol == -1 || modCol == 1))
        && !(mapCol%2==0 && (modRow == 1) && (modCol == -1 || modCol == 1)))
        return true;
    else return false;
}

void MainWindow::placeMountains(QList<QList<int>> *list) {
    if (game->getWorldAltitudesList()->count(game->getWorldAltMax()) < game->getWorldMountains()) game->setWorldMountains(game->getWorldAltitudesList()->count(game->getWorldAltMax()));
    for (int i = 0; i < game->getWorldMountains(); i++) {
        QList<int> hex;
        hex.append(rand()%game->getWorldWidth());
        if (!game->getWorldEarthStyle()) hex.append(rand()%game->getWorldHeight());
        else hex.append(game->getWorldHeight()/4 + (rand() % (game->getWorldHeight()/2)));
        game->getWorldMap()->at(hex.at(0)).at(hex.at(1))->setAltitude(game->getWorldAltMax());
        list->append(hex);
        for (int i = 0; i < game->getWorldAltitudesList()->size(); i++) {
            if (game->getWorldAltitudesList()->at(i) == game->getWorldAltMax()) {
                game->getWorldAltitudesList()->removeAt(i);
                break;
            }
        }
    }
}

void MainWindow::placeOceans(QList<QList<int> > *list) {
    if (game->getWorldAltitudesList()->count(game->getWorldAltMin()) < game->getWorldOceans()) game->setWorldOceans(game->getWorldAltitudesList()->count(game->getWorldAltMin()));
    for (int i = 0; i < game->getWorldOceans(); i++) {
        QList<int> hex;
        hex.append(rand()%game->getWorldWidth());
        hex.append(rand()%game->getWorldHeight());
        game->getWorldMap()->at(hex.at(0)).at(hex.at(1))->setAltitude(game->getWorldAltMin());
        list->append(hex);
        for (int i = game->getWorldAltitudesList()->size()-1; i > -1; i--) {
            if (game->getWorldAltitudesList()->at(i) == game->getWorldAltMin()) {
                game->getWorldAltitudesList()->removeAt(i);
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
    springs->clear();
    QList<QList<Hex*>> unpolished = *game->getWorldMap();
    double alt = 0.0, highest = 0.0, lowest = 0.0;
    int count = 0;
    for (int worldCol = 0; worldCol < game->getWorldWidth(); worldCol++) {
        for (int worldRow = 0; worldRow < game->getWorldHeight(); worldRow++) {
            alt = 0.0;
            count = 0;
            for (int col = -1; col <= 1; col++) {
                for (int row = -1; row <= 1; row++) {
                    if ((game->getWorldEarthStyle() || (!game->getWorldEarthStyle() && worldCol+col > -1 && worldCol+col < game->getWorldWidth())) && worldRow+row > -1 && worldRow+row < game->getWorldHeight()) {
                        if (row != 0 || col != 0) {
                            alt += unpolished.at((game->getWorldWidth()+(worldCol+col)) % game->getWorldWidth()).at(worldRow+row)->getAltitude();
                            count++;
                        }
                    }
                }
            }
            alt += count * unpolished.at(worldCol).at(worldRow)->getAltitude();
            count = count * 2;
            double newAlt = alt/count;
            game->getWorldMap()->at(worldCol).at(worldRow)->setAltitude(newAlt);
            if (highest < alt/count) highest = newAlt;
            if (lowest > alt/count) lowest = newAlt;
        }
    }
    double stretchHigh = game->getWorldAltMax() / highest, stretchLow = game->getWorldAltMin() / lowest;
    for (int worldCol = 0; worldCol < game->getWorldWidth(); worldCol++) {
        for (int worldRow = 0; worldRow < game->getWorldHeight(); worldRow++) {
            double newAlt = game->getWorldMap()->at(worldCol).at(worldRow)->getAltitude();
            if (newAlt > 0) {
                game->getWorldMap()->at(worldCol).at(worldRow)->setAltitude(newAlt * stretchHigh);
                if (game->getWorldAltMax()-0.5 > newAlt * stretchHigh &&  newAlt * stretchHigh >= game->getWorldAltMax()-1.5) springs->append(game->getWorldMap()->at(worldCol).at(worldRow));
            } else game->getWorldMap()->at(worldCol).at(worldRow)->setAltitude(newAlt * stretchLow);
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
    colorizeWorldMap();
    //doubleUp();
}

void MainWindow::doubleUp() {
    QList<Hex*> newCol;
    for (int i = 0; i < game->getWorldWidth(); i++) {
        newCol.clear();
        for (int k = 0; k < game->getWorldHeight(); k++) {
            Hex *newHex = new Hex;
            newHex->setCol(game->getWorldMap()->at(i).at(k)->getCol() + i);
            newHex->setRow(k);
            newHex->setAltitude(game->getWorldMap()->at(i).at(k)->getAltitude());
            newHex->setType("");
            newHex->setClimate("");
            newHex->setPos(hexSize * 1.5 * (game->getWorldWidth()+i), hexSize * sqrt(3) * k + sqrt(3)/2 * hexSize * ((game->getWorldWidth() + i) % 2));
            newCol.append(newHex);
            scene->addItem(newHex);
            game->getWorldMap()->at(i).at(k);
            newCol.append(newHex);
        }
        game->getWorldMap()->append(newCol);
    }
}
