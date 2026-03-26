
#ifndef DATA_STRUCTURE_2_TEAM_H
#define DATA_STRUCTURE_2_TEAM_H

#include <memory>

class Team {
private:
    bool isTeamExist;
    int teamId;
    int tBalance;
    int groupSize;
    shared_ptr<Team> pointTeam;

public:
    explicit Team(int id) :  isTeamExist(true), teamId(id), tBalance(0), groupSize(1), pointTeam(nullptr) {}

    int getId() const {
        return teamId;
    }

    void teamNotExist() {
        isTeamExist = false;
    }

    bool getTeamExist() const {
        return isTeamExist;
    }

    //for unitedTeams in unionFind
    void setId(int id) {
        teamId = id;
    };

    int getGroupSize() const {
        return groupSize;
    }

    void setGroupSize(int size) {
        groupSize = size;
    }

    shared_ptr<Team> getPointTeam() const {
        return pointTeam;
    }

    void setPointTeam(const shared_ptr<Team>& other) {
        pointTeam = other;
    }

    int getBalance() const {
        return tBalance;
    }

    void setBalance(int x) {
        tBalance = x;
    };

    void setWinBalance() {
        tBalance++;
    };

    void setLoosingBalance() {
        tBalance--;
    };

    // Use convertion operator in hasFunction in chainHash.h
    explicit operator int() const {
        return teamId;
    }
    // Use == operator in insertItem in chainHash.h
    bool operator==(const Team& other) const {
        return teamId == other.teamId;
    }

};

#endif //DATA_STRUCTURE_2_TEAM_H
