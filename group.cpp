#include "group.h"
#include "loratien.h"

extern Loratien *game;

Group::Group() : name(""), location(NULL) {
    location = NULL; // Zeile nur, um Warnung zu unterdrücken. ... Zeile löschen!
}
