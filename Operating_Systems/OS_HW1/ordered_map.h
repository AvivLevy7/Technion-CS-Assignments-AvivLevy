#ifndef _ORDERED_MAP_H
#define _ORDERED_MAP_H

#include <unordered_map>
#include <list>

template<typename K, typename T>
class ordered_map {
public:
    typedef std::pair<K, T> item_t;
    typedef std::list<item_t> list_t;
    typedef typename list_t::iterator iterator;
    typedef std::unordered_map<K, iterator> map_t;
private:
    list_t items;
    map_t map;
public:
    ordered_map(){}

    iterator find(const K& key) {
        typename map_t::iterator it = map.find(key);
        return it == map.end() ? end() : it->second;
    }

    void insert(const K& key, const T& value) {
        typename map_t::iterator it = map.find(key);
        if (it == map.end()) {
            iterator newIt = items.insert(items.end(), std::make_pair(key, value));
            map[key] = newIt;
        } else {
            (it->second)->second = value;
        }
    }

    bool erase(const K& key) {
        typename map_t::iterator it = map.find(key);
        if (it == map.end()) return false;
        items.erase(it->second);
        map.erase(it);
        return true;
    }

    void clear() noexcept { items.clear(); map.clear(); }
    bool empty() const noexcept { return items.empty(); }
    size_t size() const noexcept { return items.size(); }

    iterator begin() const noexcept { return items.begin(); }
    iterator begin() noexcept { return items.begin(); }
    iterator end() const noexcept { return items.end(); }
    iterator end() noexcept { return items.end(); }
};

#endif