
#pragma once

#include <iostream>
#include <string>

using std::ostream;
using std::string;

/**
 * @brief Enum class representing different types of tasks.
 */
enum class TaskType {
    Meeting,
    Presentation,
    Documentation,
    Development,
    Testing,
    Research,
    Training,
    Maintenance,
    CustomerSupport,
    General
};

/**
 * @brief Converts a TaskType enum to its corresponding string representation.
 *
 * @param type The TaskType enum to be converted.
 * @return std::string The string representation of the TaskType.
 */
string taskTypeToString(TaskType type);

/**
 * @brief Class representing a task.
 */
class Task {
private:
    int m_id;
    string m_description;
    int m_priority;
    TaskType m_type;

public:
    /**
     * @brief Constructor to create a Task object.
     *
     * @param priority The priority of the task, enforced to be in range [0, 100].
     * @param type The type of the task (default is TaskType::General).
     * @param desc The description of the task (default is an empty string).
     */
    Task(int priority, TaskType type = TaskType::General, const string& desc = "");

    /**
     * @brief Constructor to create a Task object with a default type.
     *
     * @param priority The priority of the task, enforced to be in range [0, 100].
     * @param desc The description of the task.
     */
    Task(int priority, const string& desc = "");

    /**
     * @brief Gets the ID of the task.
     *
     * @return int The ID of the task.
     */
    int getId() const;

    /**
     * @brief Sets the ID of the task.
     *
     * @param newId The new ID to be set for the task.
     */
    void setId(int newId);

    /**
     * @brief Gets the description of the task.
     *
     * @return string The description of the task.
     */
    string getDescription() const;

    /**
     * @brief Gets the priority of the task.
     *
     * @return int The priority of the task.
     */
    int getPriority() const;

    /**
     * @brief Gets the type of the task.
     *
     * @return TaskType The type of the task.
     */
    TaskType getType() const;

    /**
     * @brief Overloaded output stream operator for printing Task details.
     *
     * @param os The output stream.
     * @param task The Task object to be printed.
     * @return ostream& The output stream with the Task details.
     */
    friend ostream &operator<<(ostream& os, const Task& task);

    /**
     * @brief Overloaded greater-than operator to compare two Task objects based on priority.
     *
     * @param lhs The left-hand side Task object.
     * @param rhs The right-hand side Task object.
     * @return true If the priority of lhs is greater than that of rhs.
     * @return false If the priority of lhs is not greater than that of rhs.
     */
    friend bool operator>(const Task& lhs, const Task& rhs);
};
