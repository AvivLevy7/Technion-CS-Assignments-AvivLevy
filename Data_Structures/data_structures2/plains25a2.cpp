// You can edit anything you want in this file.
// However you need to implement all public Plains function, as provided below as a template

#include "plains25a2.h"


Plains::Plains() : t_UnionFind(), j_Hash() {}

Plains::~Plains() = default;

StatusType Plains::add_team(int teamId) {
    if (teamId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    try {
        if (t_UnionFind.doesSingleTeamExist(teamId)) {
            return StatusType::FAILURE;
        }
        t_UnionFind.addTeam(make_shared<Team>(teamId));

    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType Plains::add_jockey(int jockeyId, int teamId) {
    if (teamId <= 0 || jockeyId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        if (!t_UnionFind.doesSingleTeamExist(teamId) ||
                (t_UnionFind.doesSingleTeamExist(teamId) && !t_UnionFind.returnSingleTeam(teamId)->getTeamExist()) ||
                j_Hash.search(jockeyId)) {
            return StatusType::FAILURE;
        }
        shared_ptr<Team> team = t_UnionFind.returnSingleTeam(teamId);
        j_Hash.insertItem(make_shared<Jockey>(jockeyId,team));

    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType Plains::update_match(int victoriousJockeyId, int losingJockeyId) {
    if (victoriousJockeyId <= 0 || losingJockeyId <= 0 || victoriousJockeyId == losingJockeyId) {
        return StatusType::INVALID_INPUT;
    }

    try {
        shared_ptr<Jockey> vicJockey = j_Hash.searchAndReturn(victoriousJockeyId);
        shared_ptr<Jockey> losJockey = j_Hash.searchAndReturn(losingJockeyId);
        if (vicJockey == nullptr || losJockey == nullptr) {
            return StatusType::FAILURE;
        }
        shared_ptr<Team> leader1 = t_UnionFind.getLeader(vicJockey->getTeam());
        shared_ptr<Team> leader2 = t_UnionFind.getLeader(losJockey->getTeam());
        if(leader1->getId() == leader2->getId()) {
            return StatusType::FAILURE;
        }
        vicJockey->setWinBalance();
        losJockey->setLoseBalance();


        t_UnionFind.removeLeader(leader1);
        t_UnionFind.removeLeader(leader2);

        leader1->setWinBalance();
        leader2->setLoosingBalance();

        t_UnionFind.addLeader(leader1);
        t_UnionFind.addLeader(leader2);


    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType Plains::merge_teams(int teamId1, int teamId2) {
    if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2) {
        return StatusType::INVALID_INPUT;
    }

    try {
        shared_ptr<Team> team1 = t_UnionFind.returnSingleTeam(teamId1);
        shared_ptr<Team> team2 = t_UnionFind.returnSingleTeam(teamId2);

        if (team1 == nullptr || team2 == nullptr ||
        !team1->getTeamExist() || !team2->getTeamExist()) {
            return StatusType::FAILURE;
        }
        t_UnionFind.uniteTeams(team1, team2);

    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType Plains::unite_by_record(int record) {
    if (record <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        if (!t_UnionFind.canUniteByRecord(record)) {
            return StatusType::FAILURE;
        }
        int team1 = t_UnionFind.returnRecord(record)->getId();
        int team2 = t_UnionFind.returnRecord(-record)->getId();
        merge_teams(team1, team2);
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

output_t<int> Plains::get_jockey_record(int jockeyId) {
    if (jockeyId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    try {
        shared_ptr<Jockey> jockey = j_Hash.searchAndReturn(jockeyId);
        if (jockey == nullptr) {
            return output_t<int>(StatusType::FAILURE);
        }
        return output_t<int>(jockey->getBalance());
    } catch (std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR); // Handle memory allocation errors
    }
}

output_t<int> Plains::get_team_record(int teamId) {
    if (teamId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    try {
        shared_ptr<Team> team = t_UnionFind.returnSingleTeam(teamId);
        if (team == nullptr || !team->getTeamExist()) {
            return output_t<int>(StatusType::FAILURE);
        }
        return output_t<int>(team->getBalance());
    } catch (std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR); // Handle memory allocation errors
    }
}
