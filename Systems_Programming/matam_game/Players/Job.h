#pragma once

#include <string>
#include <memory>
#include "Player.h"


using std::string;

class Player;
class Job{
public:
    virtual ~Job() = default;
    virtual void applyStart(Player& player) = 0;
    virtual int SolarEclipseEffect(Player& player) = 0;
    virtual string getType() const = 0;
    virtual int getCP(const Player& player) const = 0;
    virtual bool is_far(const Player& player) const = 0;
};

class Warrior : public Job{
public:
    void applyStart(Player& player) override {
        player.setMaxHP(150);
    }
    string getType() const override {
        return "Warrior";
    }
    int SolarEclipseEffect(Player& player) override {
        if (player.getForce() != 0) {
            player.setForce(player.getForce() - 1);
            return -1;
        }
        return 0;
    }
    int getCP(const Player& player) const override {
        int CP = player.getForce() * 2 + player.getLevel();
        return CP;
    }
    bool is_far(const Player& player) const override {
        return false;
    }
};

class Archer : public Job{
public:
    void applyStart(Player& player) override {
        player.setCoins(20);
    }
    string getType() const override {
        return "Archer";
    }
    int SolarEclipseEffect(Player& player) override {
        if (player.getForce() != 0) {
            player.setForce(player.getForce() - 1);
            return -1;
        }
        return 0;
    }
    int getCP(const Player& player) const override {
        int CP = player.getForce() + player.getLevel();
        return CP;
    }
    bool is_far(const Player& player) const override {
        return true;
    }
};

class Magician : public Job{
public:
    virtual void applyStart(Player& player) override {}

    string getType() const override {
        return "Magician";
    }
    int SolarEclipseEffect(Player& player) override {
        player.setForce(player.getForce() + 1);
        return 1;
    }
    int getCP(const Player& player) const override {
        int CP = player.getForce() + player.getLevel();
        return CP;
    }
    bool is_far(const Player& player) const override {
        return true;
    }
};
