#include "Monster.h"
#include "Utilities.h"

Monster::Monster() : loot(0), damage(0), monsterName("DefualtMonster"){}

Monster::~Monster() {}

int Monster::getLoot() const{
    return loot;
}

int Monster::getDamage() const{
    return damage;
}

string Monster::getMonsterName() const{
    return monsterName;
}

int Monster::getNumMonsters() const{
    return num_monsters;
}

//--------------------------------------------------------

Snail::Snail() : Monster(), combatPower(5) {
    loot = 2;
    damage = 10;
    monsterName = "Snail";
}

int Snail::getCombatPower() const{
        return combatPower;
}

string Snail::attackPlayer(Player& player){
    if (player.getPlayerCP() > combatPower) {
        player.setLevel(player.getLevel() + 1);
        player.setCoins(player.getCoins() + loot);
        if (!player.is_player_far()) {
            player.setHealthPoints(player.getHealthPoints() - 10);
        }
        return getEncounterWonMessage(player, loot);
    } else {
        player.setHealthPoints(player.getHealthPoints() - damage);
        return getEncounterLostMessage(player, damage);
    }
}

//--------------------------------------------------------

Slime::Slime() : Monster(), combatPower(12) {
    loot = 5;
    damage = 25;
    monsterName = "Slime";
}

int Slime::getCombatPower() const{
    return combatPower;
}

string Slime::attackPlayer(Player& player){
    if (player.getPlayerCP() > combatPower) {
        player.setLevel(player.getLevel() + 1);
        player.setCoins(player.getCoins() + loot);
        if (!player.is_player_far()) {
            player.setHealthPoints(player.getHealthPoints() - 10);
        }
        return getEncounterWonMessage(player, loot);
    } else {
        player.setHealthPoints(player.getHealthPoints() - damage);
        return getEncounterLostMessage(player, damage);
    }
}

//--------------------------------------------------------

Balrog::Balrog() : Monster(), combatPower(15) {
    loot = 100;
    damage = 9001;
    monsterName = "Balrog";
}

int Balrog::getCombatPower() const{
    return combatPower;
}

void Balrog::SetCombatPower(int cP){
    this->combatPower = cP;
}

int Balrog::countBalrogs() const {
    return 1;
}

string Balrog::attackPlayer(Player& player){
    if (player.getPlayerCP() > combatPower) {
        player.setLevel(player.getLevel() + 1);
        player.setCoins(player.getCoins() + loot);
        if (!player.is_player_far()) {
            player.setHealthPoints(player.getHealthPoints() - 10);
        }
        combatPower += 2;
        return getEncounterWonMessage(player, loot);
    } else {
        player.setHealthPoints(player.getHealthPoints() - damage);
        combatPower += 2;
        return getEncounterLostMessage(player, damage);
    }
}
//--------------------------------------------------------

Pack::Pack(vector<shared_ptr<Monster>>& monsters) : monsters(monsters), combatPower(0), BalrogCounter(0) {

    num_monsters = 0;
    loot = 0;
    damage = 0;
    monsterName = "Pack";

    for (const auto &monster: this->monsters) {
        combatPower += monster->getCombatPower();
        damage += monster->getDamage();
        loot += monster->getLoot();
        num_monsters++;
        BalrogCounter += monster->countBalrogs();
    }
}

int Pack::countBalrogs() const {
    return BalrogCounter;
}

int Pack::getCombatPower() const{
    return combatPower;
}

string Pack::attackPlayer(Player& player){
    if (player.getPlayerCP() > combatPower) {
        player.setLevel(player.getLevel() + 1);
        player.setCoins(player.getCoins() + loot);
        if (!player.is_player_far()) {
            player.setHealthPoints(player.getHealthPoints() - 10);
        }
        combatPower += (2 * BalrogCounter);
        return getEncounterWonMessage(player, loot);
    } else {
        player.setHealthPoints(player.getHealthPoints() - damage);
        combatPower += (2 * BalrogCounter);
        return getEncounterLostMessage(player, damage);
    }
}
//--------------------------------------------------------