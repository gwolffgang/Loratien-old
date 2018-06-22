#ifndef CHAR_H
#define CHAR_H

#include "hex.h"

class Char : public QGraphicsPixmapItem {

private:
    QString concept, firstName, lastName, birthday, day_of_death, virtue, vice;
    Hex *location;
    int age, gender, size, defense, speed, initiative, health, mana, willpower,
        health_current, mana_current, willpower_current,
        intelligence, wits, resolve, strength, dexterity, stamina, presence, manipulation, composure,
        academics, crafts, investigation, magic, medicine, politics, science, spirituality,
        athletics, brawl, riding, ranged_combat, larceny, stealth, survival, weaponry,
        animal_ken, empathy, expression, intimidation, persuation, socialize, streetwise, subterfuge;

public:
    // constructors
    Char(int fixed_gender = -1, int fixed_age = -1);

    // getter

    // methods
    void chooseAttributes(int group, int dots);
    void chooseSkills(int group, int dots);
    void create_random();
    QString generateBirthday(int specific_age = -1);
};

#endif // CHAR_H
