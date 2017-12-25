#include <QApplication>
#include "loratien.h"

Loratien *game;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("Shazzarr");
    app.setApplicationName("Loratien");
    app.setApplicationDisplayName("Loratien");
    app.setApplicationVersion("0.1");

    game = new Loratien;
    game->setupGame();
    return app.exec();
}
