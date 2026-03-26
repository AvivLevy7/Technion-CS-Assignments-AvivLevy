

#ifndef DATA_STRUCTURE_2_UNOINFIND_H
#define DATA_STRUCTURE_2_UNOINFIND_H

#include "chainHash.h"
#include "team.h"
//#include "record.h"

class UnionFind {
private:
    ChainHash<Team> singleTeams; // Hash table for standalone teams
    ChainHash<Team> leaders; // Hash table for leaders

public:
    UnionFind() : singleTeams(), leaders(true) {}

    bool doesSingleTeamExist(int teamId) const {
        return singleTeams.search(teamId);
    }

    std::shared_ptr<Team> returnSingleTeam(int teamId) const {
        return singleTeams.searchAndReturn(teamId);
    }

    void addTeam(std::shared_ptr<Team> team) {
        singleTeams.insertItem(team);
        leaders.insertItem(team);
    }

    shared_ptr<Team> returnRecord(int record) const {
        return leaders.searchAndReturn(record);
    }

    void removeLeader(shared_ptr<Team> record) {
        leaders.deleteItem(record);
    }

    void addLeader(shared_ptr<Team> record) {
        leaders.insertItem(record);
    }

    std::shared_ptr<Team> getLeader(const std::shared_ptr<Team>& team) const {
        std::shared_ptr<Team> rootTeam = team;
        // Find the root team
        while (rootTeam->getPointTeam() != nullptr) {
            rootTeam = rootTeam->getPointTeam();
        }

        // Perform path compression
        std::shared_ptr<Team> current = team;
        while (current->getPointTeam() != nullptr) {
            std::shared_ptr<Team> next = current->getPointTeam();
            current->setPointTeam(rootTeam); // Point directly to the root
            current = next;
        }
        return rootTeam;
    }

    bool uniteTeams(const std::shared_ptr<Team>& team1, const std::shared_ptr<Team>& team2) {
        std::shared_ptr<Team> root1 = getLeader(team1);
        std::shared_ptr<Team> root2 = getLeader(team2);

        if (root1->getId() == root2->getId()) {
            return false; // Already in the same group
        }

        // Union by size: attach the smaller group under the larger group
        if (root1->getGroupSize() < root2->getGroupSize()) {
            root1->setPointTeam(root2);
            if (root1->getBalance() >= root2->getBalance()) {
                //update the changes in the singleTeam hushTable
                int id1 = root1->getId();
                int id2 = root2->getId();
                singleTeams.deleteItem(root1);
                singleTeams.deleteItem(root2);
                root1->setId(id2);
                root2->setId(id1);
                singleTeams.insertItem(root1);
                singleTeams.insertItem(root2);
            }
            root1->teamNotExist();
            root2->setGroupSize(root2->getGroupSize() + root1->getGroupSize());
            leaders.deleteItem(root1);
            leaders.deleteItem(root2);
            root2->setBalance(root2->getBalance() + root1->getBalance());
            leaders.insertItem(root2);
        } else {
            root2->setPointTeam(root1);
            if (root1->getBalance() < root2->getBalance()) {
                //update the changes in the singleTeam hushTable
                int id1 = root1->getId();
                int id2 = root2->getId();
                singleTeams.deleteItem(root1);
                singleTeams.deleteItem(root2);
                root1->setId(id2);
                root2->setId(id1);
                singleTeams.insertItem(root1);
                singleTeams.insertItem(root2);
            }
            root2->teamNotExist();
            root1->setGroupSize(root1->getGroupSize() + root2->getGroupSize());
            leaders.deleteItem(root1);
            leaders.deleteItem(root2);
            root1->setBalance(root1->getBalance() + root2->getBalance());
            leaders.insertItem(root1);
        }
        return true;
    }

    bool canUniteByRecord(int record) const {
        int opositeRecord = record * -1;
        if (leaders.searchAndCount(record) == 1 && leaders.searchAndCount(opositeRecord) == 1) {
            return true;
        } else {
            return false;
        }
    }
};

#endif //DATA_STRUCTURE_2_UNOINFIND_H
