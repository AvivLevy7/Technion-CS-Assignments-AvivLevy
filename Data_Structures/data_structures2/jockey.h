//
// Created by eyal7 on 24/01/2025.
//

#ifndef DATA_STRUCTURE_2_JOCKEY_H
#define DATA_STRUCTURE_2_JOCKEY_H

#include <iostream>
#include <memory>
#include <utility>
#include "team.h"

using namespace std;

class Jockey {
private:
    int jockeyId;
    int jBalance;
    shared_ptr<Team> team;

public:
    explicit Jockey(int id, shared_ptr<Team> team) : jockeyId(id), jBalance(0), team(::move(team)) {}

    // Use convertion operator in hasFunction in chainHash.h
    explicit operator int() const {
        return jockeyId; // Use jockeyId as the hash key
    }
    // Use == operator in insertItem in chainHash.h
    bool operator==(const Jockey& other) const {
        return jockeyId == other.jockeyId;
    }

    int getId() const {
        return jockeyId;
    }

    shared_ptr<Team> getTeam() const {
        return team;
    }

    int getBalance() const {
        return jBalance;
    }

    void setWinBalance() {
        jBalance++;
    };
    void setLoseBalance() {
        jBalance--;
    };

};
#endif //DATA_STRUCTURE_2_JOCKEY_H
