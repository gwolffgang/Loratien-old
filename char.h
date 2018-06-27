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
    inline int getAcademics() {return academics;}
    inline int getAge() {return age;}
    inline int getAnimalKen() {return animal_ken;}
    inline int getAthletics() {return athletics;}
    inline QString getBirthday() {return birthday;}
    inline int getBrawl() {return brawl;}
    inline int getComposure() {return composure;}
    inline QString getConcept() {return concept;}
    inline int getCrafts() {return crafts;}
    inline QString getDayOfDeath() {return day_of_death;}
    inline int getDefense() {return defense;}
    inline int getDexterity() {return dexterity;}
    inline int getEmpathy() {return empathy;}
    inline int getExpression() {return expression;}
    inline QString getFirstName() {return firstName;}
    inline int getGender() {return gender;}
    inline int getHealth() {return health;}
    inline int getHealthCurrent() {return health_current;}
    inline int getInitiative() {return initiative;}
    inline int getIntelligence() {return intelligence;}
    inline int getIntimidation() {return intimidation;}
    inline int getInvestigation() {return investigation;}
    inline int getLarceny() {return larceny;}
    inline QString getLastName() {return lastName;}
    inline Hex *getLocation() {return location;}
    inline int getMagic() {return magic;}
    inline int getMana() {return mana;}
    inline int getManaCurrent() {return mana_current;}
    inline int getManipulation() {return manipulation;}
    inline int getMedicine() {return medicine;}
    inline int getPersuation() {return persuation;}
    inline int getPolitics() {return politics;}
    inline int getPresence() {return presence;}
    inline int getRangedCombat() {return ranged_combat;}
    inline int getResolve() {return resolve;}
    inline int getRiding() {return riding;}
    inline int getScience() {return science;}
    inline int getSize() {return size;}
    inline int getSocialize() {return socialize;}
    inline int getSpeed() {return speed;}
    inline int getSpirituality() {return spirituality;}
    inline int getStealth() {return stealth;}
    inline int getStreetwise() {return streetwise;}
    inline int getStrength() {return strength;}
    inline int getSubterfuge() {return subterfuge;}
    inline int getSurvival() {return survival;}
    inline QString getVice() {return vice;}
    inline QString getVirtue() {return virtue;}
    inline int getWeaponry() {return weaponry;}
    inline int getWillpower() {return willpower;}
    inline int getWillpowerCurrent() {return willpower_current;}
    inline int getWits() {return wits;}
    inline int getStamina() {return stamina;}

    // methods
    void chooseAttributes(int group, int dots);
    void chooseSkills(int group, int dots);
    void create_random();
    QString generateBirthday(int specific_age = -1);
};

#endif // CHAR_H
