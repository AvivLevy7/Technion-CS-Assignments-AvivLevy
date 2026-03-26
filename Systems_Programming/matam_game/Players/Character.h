#pragma once

#include <string>
#include <memory>
#include "Player.h"


using std::string;


class Player;
class Character{
public:
    virtual ~Character() = default;
    virtual int PotionsMerchantEffect(Player& player) = 0;
    virtual string getType() = 0;
};

class Responsible : public Character{
public:
    string getType() override {
        return "Responsible";
    }
    int PotionsMerchantEffect(Player& player) override {
        int counter = 0;
        while (player.getHealthPoints() <= player.getMaxHP() - 10 && player.getCoins() >= 5) {
            player.setCoins(player.getCoins() - 5);
            player.setHealthPoints(player.getHealthPoints() + 10);
            counter++;
        }
        return counter;
    }
};

class RiskTaking : public Character{
public:
    string getType() override {
        return "RiskTaking";
    }
    int PotionsMerchantEffect(Player& player) override{
        int counter = 0;
        if (player.getHealthPoints() < 50 && player.getCoins() >= 5) {
            player.setCoins(player.getCoins() - 5);
            player.setHealthPoints(player.getHealthPoints() + 10);
            counter++;
        }
        return counter;
    }
};

