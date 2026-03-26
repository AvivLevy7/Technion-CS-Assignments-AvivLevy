
#pragma once

#include "../Players/Player.h"
#include "../Events/Monster.h"



class Event {
public:
    virtual string getDescription() const = 0;
    virtual ~Event();
    virtual string apply(Player& player) const = 0;
};

class SpecialEvent : public Event {
public:
    virtual string apply(Player& player) const override = 0;
    virtual string getDescription() const override = 0;
};

class SolarEclipse : public SpecialEvent {
public:
    string apply(Player& player) const override;
    string getDescription() const override;
};

class PotionsMerchant : public SpecialEvent {
public:
    string apply(Player& player) const override;
    string getDescription() const override;
};



class Encounter : public Event {
    shared_ptr<Monster> m_monster;
public:
    Encounter(shared_ptr<Monster> monster);
    string apply(Player& player) const override;
    string getDescription() const override;
};