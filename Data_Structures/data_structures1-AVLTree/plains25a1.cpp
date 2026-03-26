// You can edit anything you want in this file.
// However you need to implement all public Plains function, as provided below as a template

#include "wet1util.h"
#include "plains25a1.h"
#include "horse.h"
#include "herd.h"

Plains::Plains() : t_horses(), t_non_empty_herds(), t_empty_herds() {}

Plains::~Plains() = default;

StatusType Plains::add_herd(int herdId) {
    if (herdId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    try {
        auto empty_herd = t_empty_herds.search(herdId);
        auto non_empty_herd =  t_non_empty_herds.search(herdId);
        if (empty_herd != nullptr || non_empty_herd != nullptr) {
            return StatusType::FAILURE; // Herd already exists in either tree
        }
        t_empty_herds.insert(std::make_shared<Herd>(herdId));
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType Plains::remove_herd(int herdId) {
    // Check if the herdId is valid
    if (herdId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    // Search for the herd in the empty herds tree
    try {
        if (t_empty_herds.search(herdId) == nullptr) {
            return StatusType::FAILURE;
        }
        t_empty_herds.remove(herdId);
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType Plains::add_horse(int horseId, int speed) {
    if (horseId <= 0 || speed <= 0) {
        return StatusType::INVALID_INPUT;
    }
    try {
        if (t_horses.search(horseId) != nullptr) {
            return StatusType::FAILURE;
        }
        t_horses.insert(std::make_shared<Horse>(horseId, speed));
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType Plains::join_herd(int horseId, int herdId) {

    // Input validation
    if (horseId <= 0 || herdId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        // Search for the herd in the empty and non-empty herds trees
        bool herdPtr_non_empty;
        auto herdPtr = t_empty_herds.search(herdId);
        auto herdPtr2 = t_non_empty_herds.search(herdId);
        if (herdPtr == nullptr) {
            herdPtr_non_empty = true;
            herdPtr = t_non_empty_herds.search(herdId);
        } else {
            herdPtr_non_empty = false;
        }
        if (herdPtr == nullptr && herdPtr2 == nullptr) {
            return StatusType::FAILURE; // Herd does not exist
        }

        // Search for the horse in the global horse tree
        auto horsePtr = t_horses.search(horseId);
        if (horsePtr == nullptr) {
            return StatusType::FAILURE; // Horse doesn't exists
        }

        // Check if the horse has herd
        if(horsePtr->data->getHerd() != nullptr){
            return StatusType::FAILURE; // Horse already has an herd
        }
        //Connecting the horse to the herd
        horsePtr->data->setHerd(herdPtr->data);

        //Moving the herd to non empty herds tree
        if (herdPtr_non_empty == false) {
            t_non_empty_herds.insert(herdPtr->data);
            t_empty_herds.remove(herdPtr->data->getId());
        }

        //Inserting the horse to the herd
        horsePtr->data->getHerd()->addHorse(horsePtr->data);

    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR; // Handle memory allocation errors
    }

    return StatusType::SUCCESS; // Successfully joined the herd
}

StatusType Plains::follow(int horseId, int horseToFollowId) {
    if (horseId <= 0 || horseToFollowId <= 0 || horseId == horseToFollowId) {
        return StatusType::INVALID_INPUT;
    }

    try {
        if (t_horses.search(horseId) == nullptr ||
            t_horses.search(horseToFollowId) == nullptr ||
            t_horses.search(horseId)->data->getHerd() == nullptr ||
            t_horses.search(horseToFollowId)->data->getHerd() == nullptr ||
                t_horses.search(horseId)->data->getHerd()->getId() !=
                        t_horses.search(horseToFollowId)->data->getHerd()->getId()) {
            return StatusType::FAILURE;
        }
        t_horses.search(horseId)->data->setFollow(t_horses.search(horseToFollowId)->data);
        return StatusType::SUCCESS;
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType Plains::leave_herd(int horseId) {
    if (horseId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    try {
        if (t_horses.search(horseId) == nullptr ||
                t_horses.search(horseId)->data->getHerd() == nullptr) {
            return StatusType::FAILURE;
        }
        shared_ptr<Herd> herd = t_horses.search(horseId)->data->getHerd();
        herd->removeHorse(horseId);
        if (herd->is_empty()) {
            t_non_empty_herds.remove(herd->getId());
            t_empty_herds.insert(herd);
        }
        int speed = t_horses.search(horseId)->data->getSpeed();
        t_horses.remove(horseId);
        //I killed the horse

        t_horses.insert(make_shared<Horse>(horseId, speed));
        return StatusType::SUCCESS;
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

output_t<int> Plains::get_speed(int horseId) {
    // Input validation
    if (horseId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }

    try {
        // Search for the horse in the global horse tree
        auto horsePtr = t_horses.search(horseId);
        if (!horsePtr) {
            return output_t<int>(StatusType::FAILURE); // Horse does not exist
        }

        // Return the speed of the horse
        return output_t<int>(t_horses.search(horseId)->data->getSpeed());
    } catch (std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR); // Handle memory allocation errors
    }
}

output_t<bool> Plains::leads(int horseId, int otherHorseId) {
    // Check for invalid input
    if (horseId <= 0 || otherHorseId <= 0 || horseId == otherHorseId) {
        return output_t<bool>(StatusType::INVALID_INPUT);
    }

    try {
        // Search for both horses in the global horse tree
        auto horsePtr = t_horses.search(horseId);
        auto otherHorsePtr = t_horses.search(otherHorseId);

        // If either horse is not found, return FAILURE
        if (!horsePtr || !otherHorsePtr) {
            return output_t<bool>(StatusType::FAILURE);
        }
        if(horsePtr->data->getHerd() == nullptr || otherHorsePtr->data->getHerd() == nullptr) {
            return output_t<bool>(false);
        }
        if(horsePtr->data->getHerd() != otherHorsePtr->data->getHerd()) {
            return output_t<bool>(false);
        }

        // Traverse the follow chain starting from horseId
        auto currentHorse = horsePtr->data;
        while (currentHorse && currentHorse->getState() != 1) {
            currentHorse->setState(1);
            // If otherHorseId is found in the chain, return true
            if (currentHorse->getId() == otherHorseId) {
                horsePtr->data->getHerd()->resetHorseTree();
                return output_t<bool>(true);
            }
            // Move to the horse being followed
            currentHorse = currentHorse->getFollow();
        }

        horsePtr->data->getHerd()->resetHorseTree();
        // If the traversal completes without finding otherHorseId, return false
        return output_t<bool>(false);
    } catch (std::bad_alloc&) {
        // Handle memory allocation errors
        return output_t<bool>(StatusType::ALLOCATION_ERROR);
    }
}

output_t<bool> Plains::can_run_together(int herdId) {
    try {
        // Invalid input check
        if (herdId <= 0) {
            return output_t<bool>(StatusType::INVALID_INPUT);
        }

        // Check if the herd exists
        auto herdPtr = t_non_empty_herds.search(herdId);
        if (herdPtr == nullptr) {
            return output_t<bool>(StatusType::FAILURE);
        }

        // Count the number of leaders
        auto horseTree = herdPtr->data->getHorseTree();
        int leaderCount = herdPtr->data->getHorseTree().countMatching([](const Horse& horse) {
            return !horse.getFollow(); // Check if the horse has no follow
        });

        if (leaderCount != 1) {
            return false;
        }

        // Check if there is circular followers
        auto x =herdPtr->data;
        bool check =x->nonCircularTreeCheck();
        herdPtr->data->resetHorseTree();

        return check;

    } catch (std::bad_alloc&) {
        return output_t<bool>(StatusType::ALLOCATION_ERROR);
    }
}


