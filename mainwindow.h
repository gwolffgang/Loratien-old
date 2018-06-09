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
    double getPercentAlt(double percent);
    inline bool getRightClick() {return rightClick;}

    // setter
    inline void setHexSize(int newInt) {hexSize = newInt;}
    inline void setLeftClick(bool newBool) {leftClick = newBool;}
    inline void setMaxRiverSize(int newInt) {maxRiverSize = newInt;}
    inline void setMousePos(QPointF newPointF) {mousePos = newPointF;}
    inline void setRightClick(bool newBool) {rightClick = newBool;}

    // methods
    void colorizePlates();
    void colorizeWorldMap();
    void constructWorldMap();
    void dragWorldMap(QGraphicsSceneMouseEvent *event);
    void evaluateHexes();
    void generateWorldMap();
    void placeCities();
    void placeRivers();
    void polishWorldMap();
    void repositionGUI(int offspringX, int offspringY);
    void setupGUI();
    void setupWorldMap();
    void translateValuesToWorldMap();

private:
    // attributes
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QScreen *screen;
    QString windowTitle;
    int hexSize, maxRiverSize;
    double percentMountain, percentOcean;
    GUI *guiMenu, *guiHexInfo;
    QPointF mousePos;
    bool rightClick, leftClick;
};

#endif // MAINWINDOW_H
