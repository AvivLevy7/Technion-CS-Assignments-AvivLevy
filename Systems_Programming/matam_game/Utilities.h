
#pragma once

#include <string>

#include "Players/Player.h"
#include "Events/Event.h"


/**
 * Prints the start message of the game
 *
 * return - void
*/
void printStartMessage();

/**
 * Prints the start message entry for a player
 *
 * @param index - index of the player in the players queue
 * @param player - player to print
 *
 * @return - void
*/
void printStartPlayerEntry(unsigned int index, const Player& player);

/**
 * Prints details of a single turn
 *
 * @param index - index of the player in the players queue
 * @param player - player to print
 * @param event - event the player is playing
 *
 * @return - void
*/
void printTurnDetails(unsigned int index, const Player& player, const Event& event);

/**
 * Prints the outcome of a single turn
 *
 * @param outcome - the outcome of the turn
 *
 * @return - void
*/
void printTurnOutcome(std::string outcome);

/**
 * Prints the start message of a round
 *
 * @return - void
*/
void printRoundStart();

/**
 * Prints the end message of a round
 *
 * @return - void
*/
void printRoundEnd();

/**
 * Prints a leaderboard headers message
 *
 * @return - void
*/
void printLeaderBoardMessage();

/**
 * Prints a leaderboard entry for a player
 *
 * @param i - index of the player in the leaderboard
 * @param player - player to print
 *
 * @return - void
*/
void printLeaderBoardEntry(unsigned int i, const Player& player);

/**
 * Prints a barrier message
 *
 * @return - void
*/
void printBarrier();

/**
 * Prints game over message
 *
 * @return - void
*/
void printGameOver();

/**
 * Prints a for the player that won
 *
 * @param player - player that won
 *
 * @return - void
*/
void printWinner(const Player& player);

/**
 * Prints a message for when there are no winners
 *
 * @return - void
*/
void printNoWinners();

/**
 * Gets the outcome message for when a player wins an encounter
 *
 * @param player - player that won the encounter
 * @param loot - amount of loot the player won
 *
 * @return - outcome message
*/
std::string getEncounterWonMessage(const Player& player, unsigned int loot);

/**
 * Gets the outcome message for when a player loses an encounter
 *
 * @param player - player that lost the encounter
 * @param damage - amount of damage the player took
 *
 * @return - outcome message
*/
std::string getEncounterLostMessage(const Player& player, unsigned int damage);

/**
 * Gets the outcome message for when a player purchases potions
 *
 * @param player - player that purchased potions
 * @param amount - amount of potions the player purchased
 *
 * @return - outcome message
*/
std::string getPotionsPurchaseMessage(const Player& player, unsigned int amount);

/**
 * Gets the outcome message for when a player experiences a solar eclipse
 *
 * @param player - player that purchased equipment
 * @param effect - the amount of force the player gained (or lost if negative)
 *
 * @return - outcome message

*/
std::string getSolarEclipseMessage(const Player& player, int effect);
