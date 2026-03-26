
#ifndef DATA_STRUCTURE_2_CHAINHASH_H
#define DATA_STRUCTURE_2_CHAINHASH_H

#include <iostream>
#include <memory> // For shared_ptr
using namespace std;

template <typename T>
struct ChainHash {
private:
    struct Node {
        shared_ptr<T> key;                 // Shared pointer to the data
        shared_ptr<Node> next;             // Shared pointer to the next node in the chain
        explicit Node(shared_ptr<T> k) : key(k), next(nullptr) {}
    };

    bool isLeader;
    int bucket;                           // Number of buckets
    int size;                             // Total number of elements in the hash table
    unique_ptr<shared_ptr<Node>[]> table; // Array of shared_ptr to chains (linked lists)

    // Hash function to map a key to an index
    int hashFunction(shared_ptr<T> key) const {
        if (isLeader) {
            return std::abs(key->getBalance()) % bucket;
        }
        return key->getId() % bucket;
    }

    // Resize the hash table dynamically
    void resizeTable(int newSize) {
        unique_ptr<shared_ptr<Node>[]> newTable(new shared_ptr<Node>[newSize]);

        // Initialize the new table to nullptr
        for (int i = 0; i < newSize; i++) {
            newTable[i] = nullptr;
        }

        // Rehash all existing keys into the new table
        for (int i = 0; i < bucket; i++) {
            shared_ptr<Node> current = move(table[i]);
            while (current) {
                int newIndex;
                if (isLeader) {
                    newIndex = std::abs(current->key->getBalance()) % newSize;
                }else {
                    newIndex = static_cast<int>(*current->key) % newSize;
                }
                shared_ptr<Node> nextNode = move(current->next);

                // Insert into the new table (the start of the chain)
                current->next = move(newTable[newIndex]);
                newTable[newIndex] = move(current);

                current = move(nextNode);
            }
        }


        // Update table and bucket count
        table = move(newTable);
        bucket = newSize;
    }

public:

    // Constructor
    ChainHash() : isLeader(false), bucket(1), size(0), table(new shared_ptr<Node>[bucket]) {}

    ChainHash(bool b) : isLeader(b), bucket(1), size(0), table(new shared_ptr<Node>[bucket]) {}

    bool search(int id) const {
        int index = id % bucket; // Directly calculate the hash using the integer
        shared_ptr<Node> current = table[index];

        // Traverse the chain at the given index
        while (current) {
            if (current->key->getId() == id) { // Compare using the team ID
                return true; // Key exists in the table
            }
            current = current->next;
        }
        return false; // Key not found
    }

    int searchAndCount(int balance) const {
        int index = std::abs(balance) % bucket; // Directly calculate the hash using the integer
        shared_ptr<Node> current = table[index];

        int counter = 0;
        while (current) {
            if (current->key->getBalance() == balance) {
                counter++;
            }
            current = current->next;
        }
        return counter;
    }

    shared_ptr<T> searchAndReturn(int val) const {
        int index = std::abs(val) % bucket;
        shared_ptr<Node> current = table[index];

        while (current) {
            if ((isLeader && current->key->getBalance() == val) || (!isLeader && current->key->getId() == val)) {
                return current->key;
            }
            current = current->next;
        }
        return nullptr;
    }


    // Insert a key into the hash table
    bool insertItem(shared_ptr<T> key) {
        // Resize if load factor exceeds 1.0
        if ((double)size / bucket > 1.0) {
            resizeTable(bucket * 2);
        }

        int index = hashFunction(key);
        // Check if the key already exists in the chain
        shared_ptr<Node> current = table[index];
        while (current) {
            if (*(current->key) == *key) {
                return false; // Key already exists
            }
            current = current->next;
        }

        // Key does not exist, proceed with insertion
        shared_ptr<Node> newNode = make_shared<Node>(key);
        newNode->next = move(table[index]);
        table[index] = move(newNode);
        size++;
        return true; // Insertion successful
    }

    bool deleteItem(shared_ptr<T> key) {
        // Resize the table if size is less than 25% of the bucket count
        if (bucket > 1 && size < bucket / 4) {
            resizeTable(bucket / 2);
        }

        int index = hashFunction(key);
        shared_ptr<Node>& head = table[index];

        shared_ptr<Node> prev = nullptr;  // Use shared_ptr for consistency
        shared_ptr<Node> current = head;

        // Search for the key in the chain
        while (current) {
            if (*current->key == *key) {
                if (prev) {
                    prev->next = move(current->next); // Remove current node by linking previous node to next node
                } else {
                    head = move(current->next); // Remove the first node
                }
                size--;
                return true;  // Successfully deleted
            }
            prev = current;
            current = current->next;
        }
        return false;  // Key not found
    }
};


#endif //DATA_STRUCTURE_2_CHAINHASH_H
