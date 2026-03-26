#pragma once
using std::cout;
using std::endl;
#include <iostream>
#include <stdexcept>

namespace mtm {

    template<typename T>
    class SortedList {
        SortedList *next;
        SortedList *prev;
        T *data;

    public:

        /**
         *
         * the class should support the following public interface:
         * if needed, use =default / =delete
         *
         * constructors and destructor:
         * 1. SortedList() - creates an empty list.
         * 2. copy constructor
         * 3. operator= - assignment operator
         * 4. ~SortedList() - destructor
         *
         * iterator:
         * 5. class ConstIterator;
         * 6. begin method
         * 7. end method
         *
         * functions:
         * 8. insert - inserts a new element to the list
         * 9. remove - removes an element from the list
         * 10. length - returns the number of elements in the list
         * 11. filter - returns a new list with elements that satisfy a given condition
         * 12. apply - returns a new list with elements that were modified by an operation
         */

        SortedList();

        SortedList(const SortedList &list);

        SortedList &operator=(const SortedList &list);

        ~SortedList();

        class ConstIterator;

        ConstIterator begin() const;

        ConstIterator end() const;

        template<class Predicate>
        SortedList filter(Predicate p) const;

        template<class Predicate>
        SortedList apply(Predicate p) const;

        void insert(const T &elem);

        int length() const;

        void remove(const ConstIterator &it);

        void del();

    };

    template<class T>
    class SortedList<T>::ConstIterator {
        /**
         * the class should support the following public interface:
         * if needed, use =default / =delete
         *
         * constructors and destructor:
         * 1. a ctor(or ctors) your implementation needs
         * 2. copy constructor
         * 3. operator= - assignment operator
         * 4. ~ConstIterator() - destructor
         *
         * operators:
         * 5. operator* - returns the element the iterator points to
         * 6. operator++ - advances the iterator to the next element
         * 7. operator!= - returns true if the iterator points to a different element
         *
         */
        const SortedList *list;
        int index;

        //ConstIterator() : list(nullptr), index(0) {}

        ConstIterator(const SortedList *list, int index);


    public:
        ConstIterator(const ConstIterator &it) = default;

        ConstIterator& operator=(const ConstIterator &it) = default;

        ~ConstIterator() = default;

        ConstIterator &operator++();

        bool operator!=(const ConstIterator &it);

        const T &operator*() const;

        friend class SortedList<T>;
    };


    template<typename T>
    SortedList<T>::SortedList() : next(nullptr), prev(nullptr), data(nullptr) {}

    template<typename T>
    SortedList<T>::~SortedList() {
        this->del();
    }

    template<typename T>
    SortedList<T> &SortedList<T>::operator=(const SortedList &list) {
        if (this == &list) {
            return *this;
        }
        SortedList exp = *this;
        try {
            this->del();
            this->next = nullptr;

            SortedList *tmp = list.next;

            while (tmp != nullptr) {
                this->insert(*(tmp->data));
                tmp = tmp->next;
            }
        } catch (const std::bad_alloc&){
            *this = exp;
            throw;
        }
        return *this;
    }

    template<typename T>
    SortedList<T>::SortedList(const SortedList &list) : next(nullptr), prev(nullptr), data(nullptr) {
        SortedList *current = list.next;
        while (current != nullptr) {
            this->insert(*(current->data));
            current = current->next;
        }
    }

    template<typename T>
    void SortedList<T>::insert(const T &elem) {
        if (next == nullptr) {
            SortedList *newList = new SortedList();
            try {
                newList->data = new T(elem);
            }
            catch (const std::bad_alloc &) {
                delete newList;
                throw;
            }
            next = newList;
            newList->prev = this;
            return;
        }

        SortedList *current = this->next;

        while (*(current->data) > elem) {
            if (current->next == nullptr) {
                break;
            }
            current = current->next;
        }
        if (!(*(current->data) > elem)) {
            SortedList *newList = new SortedList();
            try {
                newList->data = new T(elem);
            }
            catch (const std::bad_alloc &) {
                delete newList;
                throw;
            }
            SortedList *tmp = current;
            current = current->prev;
            newList->prev = current;
            newList->next = tmp;
            if (current != nullptr) {
                current->next = newList;
            }
            tmp->prev = newList;
            return;
        } else {
            SortedList *newList = new SortedList();
            try {
                newList->data = new T(elem);
            }
            catch (const std::bad_alloc &) {
                delete newList;
                throw;
            }
            newList->prev = current;
            current->next = newList;
            return;
        }
    }


    template<typename T>
    int SortedList<T>::length() const {
        int count = 0;
        if (this->next == nullptr) {
            return count;
        }
        SortedList *current = this->next;
        while (current != nullptr) {
            count++;
            current = current->next;
        }

        return count;
    }


    template<typename T>
    void SortedList<T>::del() {
        if (next != nullptr) {
            delete next->data;
            delete next;
        }
    }

    template<typename T>
    void SortedList<T>::remove(const ConstIterator &it) {
        if (it.index >= this->length() || it.index < 0) {
            return;
        }
        SortedList* current = next;
        for(ConstIterator it1 = begin(); it1 != end(); ++it1){
            if(!(it1 != it)){
                break;
            }
            current = current->next;
        }
        if(current == nullptr)
            return;

        SortedList *n = current->next;
        SortedList *p = current->prev;
        p->next = n;
        if(n != nullptr)
        n->prev = p;
        current->next = nullptr;
        delete current->data;
        delete current;
        return;
    }

    template<class T>
    template<class Predicate>
    SortedList<T> SortedList<T>::filter(Predicate p) const {
        SortedList<T> result;
        for (const T &elem: *this) {
            if (p(elem)) {
                result.insert(elem);
            }
        }
        return result;
    }

    template<class T>
    template<class Predicate>
    SortedList<T> SortedList<T>::apply(Predicate p) const {
        SortedList<T> result;
        for (const T &elem: *this) {
            result.insert(p(elem));
        }
        return result;
    }

    template<typename T>
    typename SortedList<T>::ConstIterator SortedList<T>::begin() const {
        return ConstIterator(this, 0);
    }

    template<typename T>
    typename SortedList<T>::ConstIterator SortedList<T>::end() const {
        return ConstIterator(this, length());
    }

    template<typename T>
    SortedList<T>::ConstIterator::ConstIterator(const SortedList *list, int index):
            list(list), index(index) {}

    template<typename T>
    typename SortedList<T>::ConstIterator &SortedList<T>::ConstIterator::operator++() {
        if (index >= list->length()) {
            throw std::out_of_range("Iterator out of bounds");
        }
            index++;
        return *this;
    }

    template<class T>
    bool SortedList<T>::ConstIterator::operator!=(const ConstIterator &it) {
        return index != it.index;
    }

    template<class T>
    const T &SortedList<T>::ConstIterator::operator*() const {
        SortedList *current = list->next;
        for (int i = 0; i < index; ++i) {
            current = current->next;
        }
        return *(current->data);
    }
}
//g++ -DNDEBUG -std=c++17 -Wall -pedantic-errors -Werror -o TaskManager *.cpp