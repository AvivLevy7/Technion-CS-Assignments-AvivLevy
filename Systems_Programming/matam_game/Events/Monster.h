#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Players/Player.h"

using std::string;
using std::shared_ptr;
using std::vector;

class Monster {
protected:
int loot;
int damage;
string monsterName;
int num_monsters;

public:
    Monster();
    virtual ~Monster();
    string getMonsterName() const;
    int getLoot() const;
    int getDamage() const;
    int getNumMonsters() const;
    virtual int getCombatPower() const = 0;
    virtual string attackPlayer(Player& player) = 0;
    virtual int countBalrogs() const {
        return 0;
    }
};

class Snail : public Monster {
    int combatPower;
public:
    Snail();
    int getCombatPower() const override;
    string attackPlayer(Player& player) override;
};

class Slime : public Monster {
    int combatPower;
public:
    Slime();
    int getCombatPower() const override;
    string attackPlayer(Player& player) override;
};

class Balrog : public Monster {
    int combatPower;
public:
    Balrog();
    int getCombatPower() const override;
    int countBalrogs() const override;
    void SetCombatPower(int cP);
    string attackPlayer(Player& player) override;
};

class Pack : public Monster {
    vector<shared_ptr<Monster>> monsters;
    int combatPower;
    int BalrogCounter;
public:
    Pack(vector<shared_ptr<Monster>>& monsters);
    int getCombatPower() const override;
    int countBalrogs() const override;
    string attackPlayer(Player& player) override;
};