#include "Player.h"
#include "Character.h"
#include "Job.h"



// Constructor
Player::Player(const string &name, shared_ptr<Job> job, shared_ptr<Character> character) :
        name(name), character(character), job(job), level(1), force(5), Max_HP(100),
        Current_HP(Max_HP), coins(10) {
    if(job != nullptr) {
        job->applyStart(*this);
    }
    Current_HP = Max_HP;
}

//Getters
string Player::getName() const {
    return name;
}

int Player::getLevel() const {
    return level;
}

int Player::getForce() const {
    return force;
}

int Player::getHealthPoints() const {
    return Current_HP;
}

int Player::getCoins() const {
    return coins;
}

int Player::getMaxHP() const {
    return Max_HP;
}

string Player::getDescription() const {
    return (name + ", " + job->getType() + " with " + character->getType() +
           " character (level " + to_string(level) + ", force " + to_string(force) + ")");
}

int Player::applySolarEclipse() {
    int effect = job->SolarEclipseEffect(*this);
    return effect;
}

int Player::applyPotionsMerchant() {
    int num_potions = character->PotionsMerchantEffect(*this);
    return num_potions;
}

//setters
void Player::setCoins(int new_coins) {
    coins = new_coins;
}

void Player::setHealthPoints(int new_HP) {
    if (new_HP < 0) {
        new_HP = 0;
    }
    Current_HP = new_HP;
}

void Player::setForce(int new_force) {
    force = new_force;
}

void Player::setMaxHP(int new_max_HP) {
    Max_HP = new_max_HP;
}

void Player::setLevel(int new_level) {
    level = new_level;
}

int Player::getPlayerCP() const {
    int cp = job->getCP(*this);
    return cp;
}

bool Player::is_player_far() const {
    return (job->is_far(*this));
}