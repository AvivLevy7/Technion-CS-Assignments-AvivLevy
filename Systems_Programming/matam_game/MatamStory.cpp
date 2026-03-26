
#include "MatamStory.h"
#include "Utilities.h"


MatamStory::MatamStory(std::istream& eventsStream, std::istream& playersStream) {

    /*===== TODO: Open and read events file =====*/

    map<string, shared_ptr<Event>> EventMap = {
            {"SolarEclipse",    make_shared<SolarEclipse>()},
            {"PotionsMerchant", make_shared<PotionsMerchant>()},
            {"Snail", make_shared<Encounter>(make_shared<Snail>())},
            {"Slime", make_shared<Encounter>(make_shared<Slime>())},
            {"Balrog", make_shared<Encounter>(make_shared<Balrog>())},
    };

    string eventStr;
    while (eventsStream >> eventStr) {
        if (eventStr == "Pack") {
            shared_ptr<Pack> pack = createPack(eventsStream);
            shared_ptr <Event> packEvent = make_shared<Encounter>(pack);
            events_list.push_back(packEvent);
        } else {
            if (EventMap.find(eventStr) == EventMap.end()) {
                throw std::runtime_error("Invalid Events File");
            }
            shared_ptr <Event> event = EventMap[eventStr];
            events_list.push_back(event);
        }
    }
    if (events_list.size() < 2) {
        throw std::out_of_range("Invalid Events File");
    }


    /*==========================================*/
    map<string, shared_ptr<Job>> jobMap = {
            {"Warrior", make_shared<Warrior>()},
            {"Magician", make_shared<Magician>()},
            {"Archer", make_shared<Archer>()}
    };
    map<string, shared_ptr<Character>> characterMap = {
            {"Responsible", make_shared<Responsible>()},
            {"RiskTaking", make_shared<RiskTaking>()}
    };
    string name, jobStr, characterStr;
    while (playersStream >> name >> jobStr) {
        if (!(playersStream >> characterStr)) {
            throw std::runtime_error("Invalid Players File");
        }
        if (name.length() < 3 || name.length() > 15) {
            throw std::out_of_range("Invalid Players File");
        }
        if (jobMap.find(jobStr) == jobMap.end()) {
            throw std::runtime_error("Invalid Players File");
        }
        shared_ptr<Job> job = jobMap[jobStr];
        if (characterMap.find(characterStr) == characterMap.end()) {
            throw std::runtime_error("Invalid Players File");
        }
        shared_ptr<Character> character = characterMap[characterStr];
        players_list.push_back(Player(name, job, character));
    }
    if (players_list.size() < 2 || players_list.size() > 6) {
        throw std::out_of_range("Invalid Players File");
    }
    for (size_t i = 0; i < players_list.size(); i++) {
        round_players.push_back(players_list[i].getName());
    }
    this->m_turnIndex = 1;
    this->eventIndex = 0;
}

void MatamStory::playTurn(Player& player) {
    if (player.getHealthPoints() == 0) {
        return;
    }
    if (eventIndex >= events_list.size()) {
        eventIndex = 0;
    }
    Event& currentEvent = *events_list[eventIndex];
    printTurnDetails(m_turnIndex, player, currentEvent);
    string outcome = currentEvent.apply(player);
    printTurnOutcome(outcome);
    eventIndex++;
    m_turnIndex++;
}

void MatamStory::playRound() {

    printRoundStart();
    for (size_t i = 0; i < round_players.size(); i++) {
        for (size_t j = 0; j < players_list.size(); j++) {
            if (round_players[i] == players_list[j].getName()) {
            playTurn(players_list[j]);
            }
        }
    }

    printRoundEnd();

    printLeaderBoardMessage();
    sort(players_list.begin(), players_list.end(), comparePlayers);
    for (size_t i = 0; i < players_list.size(); i++) {
        printLeaderBoardEntry(i + 1, players_list[i]);
    }

    printBarrier();
}

bool MatamStory::isGameOver() const {
    size_t counter = 0;
    for (size_t i = 0; i < players_list.size(); i++) {
        if (players_list[i].getLevel() >= 10) {
            return true;
        } else if (players_list[i].getHealthPoints() <= 0) {
            counter++;
        }
    }
    if (counter == players_list.size()) {
        return true;
    }
    return false;
}

void MatamStory::play() {
    printStartMessage();
    for (size_t i = 0; i < players_list.size(); i++) {
        printStartPlayerEntry(i + 1, players_list[i]);
    }
    printBarrier();

    while (!isGameOver()) {
        playRound();
    }

    printGameOver();
    for (size_t i = 0; i < players_list.size(); i++) {
        if (players_list[i].getLevel() >= 10) {
            printWinner(players_list[i]);
            return;
        }
    }
    printNoWinners();
}

shared_ptr<Pack> MatamStory::createPack(istream& eventsStream) {
    int numMonsters;
    eventsStream >> numMonsters;
    if (numMonsters < 2) {
        throw std::out_of_range("Invalid Events File");
    }
    vector<shared_ptr<Monster>> monsters;
    for (int i = 0; i < numMonsters; ++i) {
        string monsterType;
        eventsStream >> monsterType;
        if (monsterType == "Pack") {
            shared_ptr<Pack> nestedPack = createPack(eventsStream);
            monsters.push_back(nestedPack);
        } else if (monsterType == "Snail") {
            monsters.push_back(make_shared<Snail>());
        } else if (monsterType == "Slime") {
            monsters.push_back(make_shared<Slime>());
        } else if (monsterType == "Balrog") {
            monsters.push_back(make_shared<Balrog>());
        } else {
            throw std::runtime_error("Invalid Events File");
        }
    }

    return make_shared<Pack>(monsters);
}

bool comparePlayers(Player& player1, Player& player2) {
    if (player1.getLevel() != player2.getLevel()) {
        return (player1.getLevel() > player2.getLevel());
    } else if (player1.getCoins() != player2.getCoins()) {
        return (player1.getCoins() > player2.getCoins());
    } else {
        return (player1.getName() < player2.getName());
    }
}

