
#pragma once


#include <iostream>
#include <string>
#include <memory>



using std::shared_ptr;
using std::string;
using std::to_string;
using std::move;

class Job;
class Character;
class Player {
    string name;
    shared_ptr <Character> character;
    shared_ptr <Job> job;
    int level;
    int force;
    int Max_HP;
    int Current_HP;
    int coins;

public:

    Player(const string &name, shared_ptr<Job> job, shared_ptr<Character> character);


    string getDescription() const;


    string getName() const;


    int getLevel() const;


    int getForce() const;


    int getHealthPoints() const;


    int getCoins() const;
    int getMaxHP() const;
    int getPlayerCP() const;
    bool is_player_far() const;

    void setHealthPoints(int new_HP);
    void setCoins(int new_coins);
    void setForce(int new_force);
    void setMaxHP(int new_max_HP);
    void setLevel(int new_level);

    int applySolarEclipse();
    int applyPotionsMerchant();

};



