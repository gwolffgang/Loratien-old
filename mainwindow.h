#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDesktopServices>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QScreen>
#include <QMainWindow>

#include "hex.h"
#include "gui.h"
#include "river.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    // attributes
    QRect desktop;
    QList<Hex*> *springs, *lakes;

    // constructors
    explicit MainWindow(QWidget *parent = NULL);

    // destructors
    ~MainWindow();

    // getter
    inline int getHexSize() {return hexSize;}
    inline bool getLeftClick() {return leftClick;}
    inline int getMaxRiverSize() {return maxRiverSize;}
    inline QPointF getMousePos() {return mousePos;}
    inline bool getRightClick() {return rightClick;}

    // setter
    inline void setHexSize(int newInt) {hexSize = newInt;}
    inline void setLeftClick(bool newBool) {leftClick = newBool;}
    inline void setMaxRiverSize(int newInt) {maxRiverSize = newInt;}
    inline void setMousePos(QPointF newPointF) {mousePos = newPointF;}
    inline void setRightClick(bool newBool) {rightClick = newBool;}

    // methods
    int calculateAltitude(int altFactor);
    void colorizeWorldMap();
    void constructWorldMap();
    void dragWorldMap(QGraphicsSceneMouseEvent *event);
    void evaluateHexes();
    void generateWorldMap(QList<QList<int>> *list);
    QList<Hex*> getHexNeighbors(int hexCol, int hexRow, int radius = 1, bool withOriginHex = false);
    void placeCities();
    void placeMountains(QList<QList<int>> *list);
    void placeOceans(QList<QList<int>> *list);
    void placeRivers();
    void polishWorldMap();
    void repositionGUI(int offspringX, int offspringY);
    void setupGUI();
    void setupWorldMap();
    void doubleUp();

private:
    // attributes
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QScreen *screen;
    QString windowTitle;
    int hexSize, maxRiverSize;
    GUI *guiMenu, *guiHexInfo;
    QPointF mousePos;
    bool rightClick, leftClick;
};

#endif // MAINWINDOW_H
