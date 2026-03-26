#ifndef DATA_STRUCTURE_1_HORSE_H
#define DATA_STRUCTURE_1_HORSE_H

#include <memory>

class Herd; // Forward declaration of Herd

class Horse {
private:
    int horseId;                        // ID of the horse
    int speed;                          // Speed of the horse
    std::shared_ptr<Herd> belong_herd;  // Herd the horse belongs to (shared_ptr)
    std::weak_ptr<Horse> follow;        // Horse this horse is following (weak_ptr)
    mutable int state; // 0: Unvisited, 1: Visiting, 2: Visited


    friend bool operator<(const std::shared_ptr<Horse>& a, const std::shared_ptr<Horse>& b) {
        return *a < *b; // Compare the underlying Horse objects
    }

    friend bool operator>(const std::shared_ptr<Horse>& a, const std::shared_ptr<Horse>& b) {
        return *a > *b; // Compare the underlying Horse objects
    }

    friend bool operator==(const std::shared_ptr<Horse>& a, const std::shared_ptr<Horse>& b) {
        return *a == *b; // Compare the underlying Horse objects
    }

    // Comparison operators for Horse
    friend bool operator<(const Horse& horse1, const Horse& horse2) {
        return horse1.horseId < horse2.horseId;
    }

    friend bool operator==(const Horse& horse1, const Horse& horse2) {
        return horse1.horseId == horse2.horseId;
    }

    friend bool operator>(const Horse& horse1, const Horse& horse2) {
        return horse1.horseId > horse2.horseId;
    }

    // Comparison operators for Horse with ID
    friend bool operator<(const Horse& horse, int id) {
        return horse.horseId < id;
    }

    friend bool operator>(const Horse& horse, int id) {
        return horse.horseId > id;
    }

    friend bool operator==(const Horse& horse, int id) {
        return horse.horseId == id;
    }

    // Comparison operators for weak_ptr<Horse>
    friend bool operator<(const std::weak_ptr<Horse>& w1, const std::weak_ptr<Horse>& w2) {
        auto locked1 = w1.lock();
        auto locked2 = w2.lock();
        if (!locked1 || !locked2) {
            return false; // If either weak_ptr is expired, return false
        }
        return locked1->getId() < locked2->getId();
    }

    friend bool operator>(const std::weak_ptr<Horse>& w1, const std::weak_ptr<Horse>& w2) {
        auto locked1 = w1.lock();
        auto locked2 = w2.lock();
        if (!locked1 || !locked2) {
            return false; // If either weak_ptr is expired, return false
        }
        return locked1->getId() > locked2->getId();
    }

    friend bool operator==(const std::weak_ptr<Horse>& w1, const std::weak_ptr<Horse>& w2) {
        auto locked1 = w1.lock();
        auto locked2 = w2.lock();
        if (!locked1 || !locked2) {
            return false; // If either weak_ptr is expired, return false
        }
        return locked1->getId() == locked2->getId();
    }

public:
    // Constructor
    Horse(int horseId, int speed) : horseId(horseId), speed(speed), belong_herd(nullptr), follow(), state(0) {}

    // Getters
    int getId() const {
        return this->horseId;
    }

    int getSpeed() const {
        return this->speed;
    }

    std::shared_ptr<Herd> getHerd() const {
        return this->belong_herd;
    }

    // Setters
    void setHerd(const std::shared_ptr<Herd>& herd) {
        this->belong_herd = herd;
    }

    void setFollow(const std::shared_ptr<Horse>& other) {
        follow = other;
    }

    std::shared_ptr<Horse> getFollow() const {
        return follow.lock();
    }

    int getState() const {
        return state;
    }

    void setState(int newState) const {
        state = newState;
    }

};

#endif //DATA_STRUCTURE_1_HORSE_H

