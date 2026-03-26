
#include "Task.h"

// Constructor
Task::Task(int priority, TaskType type, const string &desc)
    : m_description(desc), m_priority(priority), m_type(type)
{
    // enforce priority range of 0-100
    // 0 is lowest priority, 100 is highest
    if (m_priority < 0)
    {
        m_priority = 0;
    }
    else if (m_priority > 100)
    {
        m_priority = 100;
    }
}

Task::Task(int priority, const string &desc)
    : Task(priority, TaskType::General, desc) {}

// Getters and setters
int Task::getId() const {
    return m_id;
}

void Task::setId(int newId) {
    m_id = newId;
}

TaskType Task::getType() const {
    return m_type;
}

string Task::getDescription() const {
    return m_description;
}

int Task::getPriority() const {
    return m_priority;
}


// Overloaded operators
ostream &operator<<(ostream& os, const Task& task) {
    os << "Task ID: " << task.m_id << ", Priority: " << task.m_priority;
    os << ", Type: " << taskTypeToString(task.m_type) << ", Description: " << task.m_description;
    return os;
}

bool operator>(const Task& lhs, const Task& rhs) {
    return lhs.m_priority > rhs.m_priority;
}


// Convert TaskType to string
std::string taskTypeToString(TaskType type) {
    switch (type) {
    case TaskType::Meeting:
        return "Meeting";
    case TaskType::Presentation:
        return "Presentation";
    case TaskType::Documentation:
        return "Documentation";
    case TaskType::Development:
        return "Development";
    case TaskType::Testing:
        return "Testing";
    case TaskType::Research:
        return "Research";
    case TaskType::Training:
        return "Training";
    case TaskType::Maintenance:
        return "Maintenance";
    case TaskType::CustomerSupport:
        return "Customer Support";
    case TaskType::General:
        return "General";
    default:
        return "Unknown Task";
    }
}
