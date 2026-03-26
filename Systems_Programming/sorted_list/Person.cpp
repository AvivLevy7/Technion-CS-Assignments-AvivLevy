
#include "Person.h"

using std::cout;
using std::endl;

// Constructor
Person::Person(const string &name) : m_name(name) {}

// Getters and setters
string Person::getName() const {
    return m_name;
}

const SortedList<Task>& Person::getTasks() const {
    return m_tasks;
}

void Person::setTasks(const SortedList<Task>& tasks) {
    m_tasks = tasks;
}

// Other methods
void Person::assignTask(const Task& task) {
    m_tasks.insert(task);
}


int Person::completeTask() {
    if (m_tasks.length() == 0) {
        throw std::runtime_error("No tasks assigned to this person.");
    }
    int taskId = (*m_tasks.begin()).getId();
    m_tasks.remove(m_tasks.begin());
    return taskId;
}

const Task& Person::getHighestPriorityTask() const {
    if (m_tasks.length() == 0) {
        throw std::runtime_error("No tasks assigned to this person.");
    }
    return (*m_tasks.begin());
}

// Overloaded operators
ostream& operator<<(ostream& os, const Person& person) {
    os << "Person: " << person.m_name << endl;
    // Assuming the SortedList has an appropriate method to list tasks
    for (const Task& t: person.m_tasks) {
        os << t << endl;
    }
    return os;
}
