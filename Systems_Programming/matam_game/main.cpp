
#include <string>
#include <iostream>
#include <fstream>

#include "MatamStory.h"

using std::string;
using std::cout;
using std::endl;
using std::exception;
using std::ifstream;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Invalid number of arguments" << endl;
        cout << "Usage: MatamStory <events_file_path> <players_file_path>" << endl;
    }

    ifstream eventsStream(argv[1]);
    if (!eventsStream.is_open()) {
        cout << "Invalid Events File" << endl;
        return 1;
    }

    ifstream playersStream(argv[2]);
    if (!playersStream.is_open()) {
        cout << "Invalid Players File" << endl;
        return 1;
    }

    try {
        MatamStory game(eventsStream, playersStream);
        game.play();
    }
    catch (const exception& e) {
        cout << e.what() << endl;
        return 1;
    }
}
