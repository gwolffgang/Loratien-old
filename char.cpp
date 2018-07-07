#include "char.h"
#include "loratien.h"

extern Loratien *game;

Char::Char(bool random, int fixed_gender, int fixed_age) :
    concept(""), firstName(""), lastName(""), birthday(""), day_of_death(""), virtue(""), vice(""),
    location(NULL), age(fixed_age), gender(fixed_gender), size(5), defense(2), speed(7), initiative(2),
    health(6), mana(0), willpower(2), health_current(6), mana_current(0), willpower_current(2),
    intelligence(1), wits(1), resolve(1), strength(1), dexterity(1), stamina(1), presence(1), manipulation(1), composure(1),
    academics(-3), crafts(-3), investigation(-3), magic(-3), medicine(-3), politics(-3), science(-3), spirituality(-3),
    athletics(-1), brawl(-1), riding(-1), ranged_combat(-1), larceny(-1), stealth(-1), survival(-1), weaponry(-1),
    animal_ken(-1), empathy(-1), expression(-1), intimidation(-1), persuation(-1), socialize(-1), streetwise(-1), subterfuge(-1) {
    location = game->getWorldMap()->at(rand()%game->getWorldMapWidth()).at(rand()%game->getWorldMapHeight());
    if (random) create_random();
}

void Char::chooseAttributes(int group, int dots) {
    QList<int> attributes;
    switch (dots) {
    case 3:
        for (int i = 0; i < dots; i++) attributes.append(group+rand()%3);
        break;
    case 4:
        do {
            attributes.clear();
            for (int i = 0; i < dots; i++) attributes.append(group+rand()%3);
        } while(attributes.count(group) == 4 || attributes.count(group+1) == 4 || attributes.count(group+2) == 4);
        break;
    case 5:
        for (int i = 0; i < dots; i++) attributes.append(group+rand()%3);
        for (int i = group; i < group+3; i++) {
            if (attributes.count(i) > 3) {
                attributes.clear();
                for (int k = 0; k < 4; k++) attributes.append(i);
                break;
            }
        }
    }
    foreach (int dot, attributes) {
        switch (dot) {
        case 0: intelligence++; break;
        case 1: wits++; break;
        case 2: resolve++; break;
        case 3: strength++; break;
        case 4: dexterity++; break;
        case 5: stamina++; break;
        case 6: presence++; break;
        case 7: manipulation++; break;
        case 8: composure++;
        }
    }
}

void Char::chooseSkills(int group, int dots) {
    QList<int> skills, choose_from;
    for (int k = 0; k < 8; k++) choose_from.append(group+k);
    do {
        int chosen = choose_from.at(rand()%choose_from.size());
        if (skills.count(chosen) < 4) {
            skills.append(chosen);
            choose_from.append(chosen);
        } else if (skills.size() < dots-1) {
            dots--;
            skills.append(chosen);
            for (int i=choose_from.size()-1; -1 < i; i--)
                if (choose_from.at(i) == chosen) choose_from.removeAt(i);
            }
    } while (skills.size() < dots);
    foreach (int dot, skills) {
        switch (dot) {
        case 0:
            if (academics < 0) academics = 1;
            else academics++;
            break;
        case 1:
            if (crafts < 0) crafts = 1;
            else crafts++;
            break;
        case 2:
            if (investigation < 0) investigation = 1;
            else investigation++;
            break;
        case 3:
            if (magic < 0) magic = 1;
            else magic++;
            break;
        case 4:
            if (medicine < 0) medicine = 1;
            else medicine++;
            break;
        case 5:
            if (politics < 0) politics = 1;
            else politics++;
            break;
        case 6:
            if (science < 0) science = 1;
            else science++;
            break;
        case 7:
            if (spirituality < 0) spirituality = 1;
            else spirituality++;
            break;
        case 8:
            if (athletics < 0) athletics = 1;
            else athletics++;
            break;
        case 9:
            if (brawl < 0) brawl = 1;
            else brawl++;
            break;
        case 10:
            if (riding < 0) riding = 1;
            else riding++;
            break;
        case 11:
            if (ranged_combat < 0) ranged_combat = 1;
            else ranged_combat++;
            break;
        case 12:
            if (larceny < 0) larceny = 1;
            else larceny++;
            break;
        case 13:
            if (stealth < 0) stealth = 1;
            else stealth++;
            break;
        case 14:
            if (survival < 0) survival = 1;
            else survival++;
            break;
        case 15:
            if (weaponry < 0) weaponry = 1;
            else weaponry++;
            break;
        case 16:
            if (animal_ken < 0) animal_ken = 1;
            else animal_ken++;
            break;
        case 17:
            if (empathy < 0) empathy = 1;
            else empathy++;
            break;
        case 18:
            if (expression < 0) expression = 1;
            else expression++;
            break;
        case 19:
            if (intimidation < 0) intimidation = 1;
            else intimidation++;
            break;
        case 20:
            if (persuation < 0) persuation = 1;
            else persuation++;
            break;
        case 21:
            if (socialize < 0) socialize = 1;
            else socialize++;
            break;
        case 22:
            if (streetwise < 0) streetwise = 1;
            else streetwise++;
            break;
        case 23:
            if (subterfuge < 0) subterfuge = 1;
            else subterfuge++;
        }
    }
}

void Char::create_random() {
    if (age < 0) age = rand()%100;
    if (gender < 0 || 1 < gender) gender = rand()%2;
    firstName = game->getDatabase()->getRandomFirstName(gender);
    birthday = generateBirthday();
    // virtue
    switch (rand()%7) {
    case 0: virtue = "Charity"; break;
    case 1: virtue = "Faith"; break;
    case 2: virtue = "Fortitude"; break;
    case 3: virtue = "Hope"; break;
    case 4: virtue = "Justice"; break;
    case 5: virtue = "Prudence"; break;
    case 6: virtue = "Temperance";
    }
    // vice
    switch (rand()%7) {
    case 0: vice = "Envy"; break;
    case 1: vice = "Gluttony"; break;
    case 2: vice = "Greed"; break;
    case 3: vice = "Lust"; break;
    case 4: vice = "Pride"; break;
    case 5: vice = "Sloth"; break;
    case 6: vice = "Wrath";
    }
    int groups[3] = {0,1,2};
    std::random_shuffle(std::begin(groups), std::end(groups));
    // attributes
    for (int i = 0; i < 3; i++) chooseAttributes(groups[i]*3, i+3);
    // skills
    if (rand()%10 == 0) std::random_shuffle(std::begin(groups), std::end(groups));
    if (7 == age) for (int i = 0; i < 3; i++)
        chooseSkills(groups[i]*8, 2*i+2);
    if (8 == age) for (int i = 0; i < 3; i++)
        chooseSkills(groups[i]*8, 0.5*(double)i*(double)i+1.5*(double)i+2);
    if (9 == age) for (int i = 0; i < 3; i++)
        chooseSkills(groups[i]*8, i*i+3);
    if (10 == age || 11 == age) for (int i = 0; i < 3; i++)
        chooseSkills(groups[i]*8, 0.5*(double)i*(double)i+1.5*(double)i+3);
    if (12 == age) for (int i = 0; i < 3; i++)
        chooseSkills(groups[i]*8, i*i+i+3);
    if (12 < age && age <= 17) for (int i = 0; i < 3; i++)
        chooseSkills(groups[i]*8, i*i+i+4);
    if (17 < age) for (int i = 0; i < 3; i++)
        chooseSkills(groups[i]*8, 0.5*(double)i*(double)i+2.5*(double)i+4);
    // calculated values
    if (age < 14) {
        size = 4;
        if (age < 7) {
            size = 3;
            if (age < 4) {
                size = 2;
                if (age == 0) size = 1;
            }
        }
    }
    health = stamina + size;
    health_current = health;
    mana = 2*intelligence + wits;
    mana_current = mana;
    willpower = resolve + composure;
    willpower_current = willpower;
    speed = strength + dexterity + size;
    defense = std::min(wits, dexterity);
    initiative = dexterity + composure;
}

QString Char::generateBirthday(int specific_age) {
    if (specific_age < 0) specific_age = age;
    int day = rand()%365;
    if (0 != day) return QString::number(604-specific_age)+"-" + QString::number((day-1)/91+1) + "-"+ QString::number((day-1)%91+1);
    else return QString::number(604-specific_age)+"-Newyear";
}
