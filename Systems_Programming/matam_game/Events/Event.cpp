#include "Utilities.h"
#include "Event.h"


Event::~Event() {}


string SolarEclipse::apply(Player &player) const {
    int effect = player.applySolarEclipse();
    return getSolarEclipseMessage(player, effect);
}

string SolarEclipse::getDescription() const {
    return "SolarEclipse";
}

string PotionsMerchant::apply(Player &player) const {
    int num_potions = player.applyPotionsMerchant();
    return getPotionsPurchaseMessage(player, num_potions);
}

string PotionsMerchant::getDescription() const {
return "PotionsMerchant";
}

Encounter::Encounter(shared_ptr<Monster> monster) : m_monster(monster) {}

string Encounter::apply(Player& player) const {
return m_monster->attackPlayer(player);
}

string Encounter::getDescription() const {
    if (m_monster->getMonsterName() != "Pack") {
        return (m_monster->getMonsterName() + " (power " + to_string(m_monster->getCombatPower())
                + ", loot " + to_string(m_monster->getLoot()) + ", damage " + to_string(m_monster->getDamage()) + ")");
    } else {
        return ("Pack of " + to_string(m_monster->getNumMonsters()) + " members " + "(power " +
                to_string(m_monster->getCombatPower())
                + ", loot " + to_string(m_monster->getLoot()) + ", damage " + to_string(m_monster->getDamage()) + ")");
    }
}