#include "loratien.h"
#include "mainwindow.h"

Loratien::Loratien() :
    window(NULL), database(NULL), playzoneRadius(10), regionRadius(4), sectorRadius(4), hexSize(0), worldWidth(2), worldHeight(2),
    worldEarthStyle(false), percentOcean(0.60), percentMountain(0.15), percentMountainPeak(0.01), worldAltMax(10), worldAltMin(-10),
    worldRivers(10), worldTectonicPlates(10),
    worldMap(new QList<QList<Hex*>>), worldRegions(new QList<QList<Region*>>), rivers(new QList<River*>), lakes(new QList<Lake*>), springs(new QList<Hex*>),
    group(NULL), player(NULL), npcs(new QList<Char*>), amountNPCs(20) /*, cities(new QList<City*>), villages(new QList<Village*>)*/ {
    if (regionRadius < 1) regionRadius = 1;
    if (sectorRadius < 1) sectorRadius = 1;
    sectorWidth = sectorHeight = 2*sectorRadius+1;
    regionWidth = sectorWidth*(2*regionRadius+1)-2*regionRadius;
    regionHeight = 3*regionRadius*sectorRadius+2*sectorRadius+1;
    worldMapWidth = (regionWidth-1)/2-(sectorRadius-1) + worldWidth*((regionWidth-1)/2+sectorRadius);
    worldMapHeight = (worldHeight*regionHeight - (worldHeight-1)*sectorHeight) + (regionHeight-sectorHeight)/2;

    // seed for randomizer
    unsigned int seed = 23;
    if (seed != 0) srand(seed);
    else srand(unsigned(time(NULL)));
}

double Loratien::getPercentAlt(double percent) {
    if (percent > 1.0) percent = 1.0;
    else if (percent < 0.0) percent = 0.0;
    QList<double> list;
    for (int x = 0; x < worldMapWidth; x++) {
        for (int y = 0; y < worldMapHeight; y++) {
            Hex *currentHex = worldMap->at(x).at(y);
            if (currentHex->getUsed()) list.append(currentHex->getAltitude());
        }
    }
    std::stable_sort(list.begin(), list.end());
    int elem = (list.size()-1)*percent;
    return list.at(elem);
}

QList<River*> Loratien::getRivers(Hex *hex) {
    QList<River*> list;
    for (int i = 0; i < rivers->size(); i++) {
        River *current = rivers->at(i);
        for (int k = 0; k < current->getWatercourse()->size(); k++) {
            Hex *part = current->getWatercourse()->at(k);
            if (part->getCol() == hex->getCol() && part->getRow() == hex->getRow()) list.append(current);
        }
    }
    return list;
}

void Loratien::colorizePlates() {
    for (int col = 0; col < worldMapWidth; col++) {
        for (int row = 0; row < worldMapHeight; row++) {
            Hex *currentHex = worldMap->at(col).at(row);
            if (currentHex->getUsed()) {
                QBrush brush = QBrush(QColor(0, 0, 0), Qt::SolidPattern);
                double alt = currentHex->getTectonicPlate();
                int paint = (100 + 150 * (alt / worldTectonicPlates));
                brush.setColor(QColor(paint, paint, paint));
                if (alt == -1) brush.setColor(Qt::red);
                currentHex->setBrush(brush);
                currentHex->draw();
            }
        }
    }
}

void Loratien::colorizeWorldMap() {
    double snowLevel = getPercentAlt(1-((1-percentOcean) * percentMountainPeak));
    double mountainLevel = getPercentAlt(1-((1-percentOcean) * percentMountain));
    double waterLevel = getPercentAlt(percentOcean);
    for (int col = 0; col < worldMap->size(); col++) {
        for (int row = 0; row < worldMap->at(col).size(); row++) {
            Hex *currentHex = worldMap->at(col).at(row);
            if (currentHex->getUsed()){
                QBrush brush = QBrush(QColor(0, 0, 0), Qt::SolidPattern);
                if (currentHex->getFogOfWar()) brush.setStyle(Qt::Dense6Pattern);
                if (!currentHex->getLake()) {
                    int paint = 0;
                    double alt = currentHex->getAltitude();
                    if (alt > snowLevel) {
                        brush.setColor(Qt::white);
                    } else if (alt > mountainLevel) {
                        paint = 100 + 150 * ((alt - mountainLevel) / (worldAltMax-mountainLevel));
                        brush.setColor(QColor(paint, paint, paint));
                    } else if (alt >= waterLevel) {
                        paint = 255 - 200 * ((alt-waterLevel) / (mountainLevel-waterLevel));
                        brush.setColor(QColor(0, paint, 0));
                    } else {
                        paint = 255 * ((alt-worldAltMin) / (waterLevel-worldAltMin));
                        brush.setColor(QColor(0, 0, paint));
                    }
                } else brush.setColor(Qt::blue);
                currentHex->setBrush(brush);
                currentHex->draw();
            }
        }
    }
}

void Loratien::constructWorldMap() {
    QList<Hex*> hexesCol;
    for (int hexCol = 0; hexCol < worldMapWidth; hexCol++) {
        hexesCol.clear();
        for (int hexRow = 0; hexRow < worldMapHeight; hexRow++) {
            Hex *hex = new Hex(hexCol, hexRow);
            hexesCol.append(hex);
        }
        worldMap->append(hexesCol);
    }
    QList<Region*> regionsRow;
    for (int regionRow = 0; regionRow < worldHeight; regionRow++) {
        regionsRow.clear();
        for (int regionCol = 0; regionCol < worldWidth; regionCol++) {
            int hexCol = (regionWidth-1)/2 + regionCol*((regionWidth-1)/2+sectorRadius);
            int hexRow = (regionHeight-1)/2 + regionRow*regionHeight - regionRow*(worldHeight-1)*sectorHeight + (regionHeight-sectorHeight)/2*(regionCol%2);
            Region *region = new Region(hexCol, hexRow);
            regionsRow.append(region);
        }
        worldRegions->append(regionsRow);
    }
}

void Loratien::createPlayerChar() {
    player = new Char(false);
    foreach (Hex *hex, player->getLocation()->getNeighborHexes(1,true)) hex->setFogOfWar(false);
    group = new Group(player);
}

void Loratien::evaluateHexes() {
    for (int col = 0; col < worldMapWidth; col++) {
        for (int row = 0; row < worldMapHeight; row++) {
            Hex *hex = worldMap->at(col).at(row);
            if (hex->getUsed()) {
                hex->evaluateFertility();
                hex->evaluateResources();
            }
        }
    }
}

void Loratien::generateWorldMap() {
    QList<Hex*> hexesList;
    Hex *currentHex = NULL;
    for (int k = 0; k < worldTectonicPlates; k++) {
        int hexCol = rand() % worldMapWidth;
        int hexRow = rand() % worldMapHeight;
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
        QList<Hex*> neighbors = nextHex->getNeighborHexes(3);
        for (int k = 0; k < neighbors.size(); k++) {
            currentHex = neighbors.at(k);
            if (currentHex->getUsed()) {
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
        }
        if (altCount != 0) {
            alt = altSum / altCount - worldEarthStyle * 3 * abs(worldMapHeight/2 - hexRow) / worldMapHeight;
            nextHex->setAltitude(alt + rand()%11 - 5);
        }
        hexesList.removeAt(randItem);
    }
}

void Loratien::createNPCs() {
    for (int i = 0; i < amountNPCs; i++) database->saveCharacter(new Char);
}

void Loratien::newGame() {
    window = new MainWindow;
    window->show();
    constructWorldMap();
    setupWorldMap();
    database = new Database(/*"QOCI"*/);
    createPlayerChar();
    //createNPCs();
    //colorizePlates();
    colorizeWorldMap();
    window->setupGui();
    window->drawScreen();
}

void Loratien::placeCities() {
    evaluateHexes();
}

void Loratien::placeRivers() {
    double snowLevel = getPercentAlt(1-((1-percentOcean) * 0.02));
    double mountainLevel = getPercentAlt(1-((1-percentOcean) * percentMountain));
    for (int col = 0; col < worldMapWidth; col++) {
        for (int row = 0; row < worldMapHeight; row++) {
            Hex *currentHex = worldMap->at(col).at(row);
            if (currentHex->getUsed()) {
                double currentAlt = currentHex->getAltitude();
                if (mountainLevel <= currentAlt && currentAlt <= snowLevel) springs->append(currentHex);
            }
        }
    }
    while (worldRivers < springs->size()) springs->removeAt(rand()%springs->size());
    while (springs->size() > 0) {
        int nextHex = rand()%springs->size();
        Hex *spring = springs->at(nextHex);
        int hexCol = spring->getCol();
        int hexRow = spring->getRow();
        if (!spring->getLake() && !spring->getRiver()) {
            if (!((!worldEarthStyle && (hexCol == 0 || hexCol == worldMapWidth-1)) || hexRow == 0 || hexRow == worldMapHeight-1)) {
                River *river = new River(spring);
                rivers->append(river);
            }
        }
        springs->removeAt(nextHex);
    }
}

void Loratien::polishWorldMap() {
    double alt = 0.0;
    int count = 0;
    springs->clear();
    Hex *currentHex = NULL;
    for (int worldCol = 0; worldCol < worldMapWidth; worldCol++) {
        for (int worldRow = 0; worldRow < worldMapHeight; worldRow++) {
            currentHex = worldMap->at(worldCol).at(worldRow);
            if (currentHex->getUsed()) {
                alt = 0.0;
                count = 0;
                foreach (Hex* neighbor, currentHex->getNeighborHexes(2)) {
                    alt += neighbor->getAltitude();
                    count++;
                }
                alt += count * worldMap->at(worldCol).at(worldRow)->getAltitude();
                count = count * 2;
                worldMap->at(worldCol).at(worldRow)->setAltitude(alt/count);
            }
        }
    }
    double stretchHigh = 0.0;
    double highest = getPercentAlt(1);
    if (highest != 0) stretchHigh = worldAltMax / highest;
    double stretchLow = 0.0;
    double lowest = getPercentAlt(0);
    if (lowest != 0) stretchLow = worldAltMin / lowest;
    for (int worldCol = 0; worldCol < worldMapWidth; worldCol++) {
        for (int worldRow = 0; worldRow < worldMapHeight; worldRow++) {
            Hex *currentHex = worldMap->at(worldCol).at(worldRow);
            if (currentHex->getUsed()) {
                double newAlt = worldMap->at(worldCol).at(worldRow)->getAltitude();
                if (newAlt > 0) {
                    double stretchedHeight = newAlt * stretchHigh;
                    worldMap->at(worldCol).at(worldRow)->setAltitude(stretchedHeight);
                } else worldMap->at(worldCol).at(worldRow)->setAltitude(newAlt * stretchLow);
            }
        }
    }
}

void Loratien::setupWorldMap() {
    generateWorldMap();
    simulateTectonicMovement(3);
    translateValuesToWorldMap();
    for (int times = 0; times < 2; times++) polishWorldMap();
    //placeRivers();
    //placeCities();
}

void Loratien::simulateTectonicMovement(int range) {
    Hex *currentHex = NULL;
    Hex *neighborHex = NULL;
    for (int col = 0; col < worldMapWidth; col++) {
        for (int row = 0; row < worldMapHeight; row++) {
            currentHex = worldMap->at(col).at(row);
            if (currentHex->getUsed()) {
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
                if ((worldEarthStyle || (-1 < neighborCol && neighborCol < worldMapWidth)) && -1 < neighborRow && neighborRow < worldMapHeight) {
                    neighborHex = worldMap->at((worldMapWidth+neighborCol)%worldMapWidth).at(neighborRow);
                    if (currentHex->getTectonicDirection() != neighborHex->getTectonicDirection()) {
                        currentHex->setAltitude((double)range/2*currentHex->getAltitude()+neighborHex->getAltitude());
                    }
                }
            }
        }
    }
}

void Loratien::translateValuesToWorldMap() {
    double waterLevel = getPercentAlt(percentOcean);
    double minValue = getPercentAlt(0.0);
    double maxValue = getPercentAlt(1.0);
    for (int x = 0; x < worldMapWidth; x++) {
        for (int y = 0; y < worldMapHeight; y++) {
            Hex *currentHex = worldMap->at(x).at(y);
            if (currentHex->getUsed()) {
                double alt = currentHex->getAltitude() - waterLevel;
                if (alt > 0) alt = worldAltMax / maxValue * alt;
                else alt = worldAltMin / minValue * alt;
                currentHex->setAltitude(alt);
            }
        }
    }
}
