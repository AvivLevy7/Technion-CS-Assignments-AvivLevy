#ifndef DATA_STRUCTURE_1_HERD_H
#define DATA_STRUCTURE_1_HERD_H

#include <memory>
#include "avl.h"

class Horse;

class Herd {
private:
    int herdId;  // ID of the herd
    AVLTree<Horse> t_horses_in_herd;  // AVLTree storing shared_ptr<Horse> in the herd


    friend bool operator<(const std::shared_ptr<Herd>& a, const std::shared_ptr<Herd>& b) {
        return *a < *b; // Compare the underlying Horse objects
    }

    friend bool operator>(const std::shared_ptr<Herd>& a, const std::shared_ptr<Herd>& b) {
        return *a > *b; // Compare the underlying Horse objects
    }

    friend bool operator==(const std::shared_ptr<Herd>& a, const std::shared_ptr<Herd>& b) {
        return *a == *b; // Compare the underlying Horse objects
    }

    friend bool operator<(const Herd& herd1, const Herd& herd2) {
        return herd1.herdId < herd2.herdId;
    }

    friend bool operator>(const Herd& herd1, const Herd& herd2) {
        return herd1.herdId > herd2.herdId;
    }

    friend bool operator==(const Herd& herd1, const Herd& herd2) {
        return herd1.herdId == herd2.herdId;
    }

    friend bool operator<(const Herd& herd, int id) {
        return herd.herdId < id;
    }

    friend bool operator>(const Herd& herd, int id) {
        return herd.herdId > id;
    }

    friend bool operator==(const Herd& herd, int id) {
        return herd.herdId == id;
    }

public:
    // Constructor
    explicit Herd(int herdId) : herdId(herdId), t_horses_in_herd() {}

    // Getter for herdId
    int getId() const {
        return this->herdId;
    }

    // Add a horse to the herd (takes a shared_ptr<Horse>)
    void addHorse(const shared_ptr<Horse>& horse) {
        t_horses_in_herd.insert(horse);
    }

    // Remove a horse from the herd by ID
    void removeHorse(const int horseId) {
        t_horses_in_herd.remove(horseId);
    }

    bool is_empty() {
        return t_horses_in_herd.isEmpty();
    }

    // Getter for the tree of horses
    AVLTree<Horse>& getHorseTree() {
        return t_horses_in_herd;
    }

    bool nonCircularTreeCheck() {
        return this->getHorseTree().applyToAllNodes([](Horse& horse){
            if (horse.getState() == 2) {
                 return true;
             }
                 auto current = horse.getFollow();
                 horse.setState(1);
                 while (current != nullptr) {
                      if (current->getState() == 1) {
                        return false; //Circle
                     }
                     if (current->getState() == 2) {
                        //Marking all 2
                         horse.setState(2);current = horse.getFollow();
                         while (current != nullptr) {
                             current->setState(2);
                             current = current->getFollow();
                         }
                        return true;
                    }
                    current->setState(1);
                    current = current->getFollow();
               }
             //We got to the leader
            //Marking all 2
            current = horse.getFollow();
            horse.setState(2);
            while (current != nullptr) {
                current->setState(2);
                current = current->getFollow();
            }
             return true;
        });
    }

    void resetHorseTree() {
        bool b = this->getHorseTree().applyToAllNodes([](Horse& horse){
            horse.setState(0);
            return true;
        });
        if(b){}
    }
};

#endif //DATA_STRUCTURE_1_HERD_H


