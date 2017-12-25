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

private:
    // attributes
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QScreen *screen;
    QString windowTitle;
    int hexSize;
    QList<QList<Hex*>> worldMap;
    GUI *guiMenu, *guiHexInfo;
    QPointF mousePos;
    bool rightClick, leftClick;

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
    inline QPointF getMousePos() {return mousePos;}
    inline bool getRightClick() {return rightClick;}

    // setter
    inline void setHexSize(int newHexSize) {hexSize = newHexSize;}
    inline void setLeftClick(bool newBool) {leftClick = newBool;}
    inline void setMousePos(QPointF newPos) {mousePos = newPos;}
    inline void setRightClick(bool newBool) {rightClick = newBool;}

    // methods
    int calculateAltitude(int altFactor);
    void colorizeWorldMap();
    void constructWorldMap();
    void dragWorldMap(QGraphicsSceneMouseEvent *event);
    void generateWorldMap(QList<QList<int>> *list);
    bool neighbor(int mapCol, int modCol, int mapRow, int modRow);
    void placeMountains(QList<QList<int>> *list);
    void placeOceans(QList<QList<int>> *list);
    void placeRivers();
    void polishWorldMap();
    void repositionGUI(int offspringX, int offspringY);
    void setupGUI();
    void setupWorldMap();
    void doubleUp();
};

#endif // MAINWINDOW_H
