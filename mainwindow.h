#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

    // constructors
    explicit MainWindow(QWidget *parent = NULL);

    // destructors
    ~MainWindow();

    // getter
    inline Gui *getGuiHexInfo() {return guiHexInfo;}
    inline Gui *getGuiMenu() {return guiMenu;}
    inline bool getLeftClick() {return leftClick;}
    inline int getMaxRiverSize() {return maxRiverSize;}
    inline QPointF getMousePos() {return mousePos;}
    inline bool getRightClick() {return rightClick;}

    // setter
    inline void setLeftClick(bool newBool) {leftClick = newBool;}
    inline void setMaxRiverSize(int newInt) {maxRiverSize = newInt;}
    inline void setMousePos(QPointF newPointF) {mousePos = newPointF;}
    inline void setRightClick(bool newBool) {rightClick = newBool;}

    // methods
    void drawHexes(int radius, int hexSize);
    void drawScreen();
    void drawSectors(int radius, int sectorSize);
    void setupGui();

private:
    // attributes
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QScreen *screen;
    QString windowTitle;
    QList<QList<Hex*>> *hexes;
    int maxRiverSize;
    Gui *guiMenu, *guiHexInfo;
    QPointF mousePos;
    bool rightClick, leftClick;
};

#endif // MAINWINDOW_H
