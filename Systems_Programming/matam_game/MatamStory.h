
#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <string>



#include "Players/Player.h"
#include "Events/Event.h"
#include "Players/Character.h"
#include "Players/Job.h"

using namespace std;


class MatamStory{

private:
    unsigned int m_turnIndex;
    unsigned int eventIndex;
    vector<shared_ptr<Event>> events_list;
    vector<Player> players_list;
    vector<string> round_players;

    /**
     * Playes a single turn for a player
     *
     * @param player - the player to play the turn for
     *
     * @return - void
    */
    void playTurn(Player& player);

    /**
     * Plays a single round of the game
     *
     * @return - void
    */
    void playRound();

    /**
     * Checks if the game is over
     *
     * @return - true if the game is over, false otherwise
    */
    bool isGameOver() const;

public:
    /**
     * Constructor of MatamStory class
     *
     * @param eventsStream - events input stream (file)
     * @param playersStream - players input stream (file)
     *
     * @return - MatamStory object with the given events and players
     *
    */
    MatamStory(std::istream& eventsStream, std::istream& playersStream);

    /**
     * Plays the entire game
     *
     * @return - void
    */
    void play();


    shared_ptr<Pack> createPack(istream& eventsStream);
};

bool comparePlayers(Player& player1, Player& player2);

//g++ --std=c++17 -o MatamStory -Wall -pedantic-errors -Werror -DNDEBUG *.cpp Events/*.cpp Players/*.cpp -I. -I./Players -I./Events
//./MatamStory tests/test1.events tests/test1.players