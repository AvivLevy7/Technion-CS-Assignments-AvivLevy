
#ifndef DATA_STRUCTURE_2_RECORD_H
#define DATA_STRUCTURE_2_RECORD_H

#include "chainHash.h"
#include "team.h"

class Record : public Team {
public:
    explicit Record(int id) : Team(id) {}

    // Override the int conversion operator to return tBalance
    explicit operator int() const override {
        return getBalance();
    }
};

#endif //DATA_STRUCTURE_2_RECORD_H
