#include "group.h"
#include "loratien.h"

extern Loratien *game;

Group::Group(Char *leader) : name(""), members(new QList<Char*>), location(leader->getLocation()) {
    members->append(leader);
}
