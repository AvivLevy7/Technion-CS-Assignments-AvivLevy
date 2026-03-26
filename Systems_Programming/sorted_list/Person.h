
#pragma once

#include <iostream>
#include <string>
#include "Task.h"
#include "SortedList.h"

using mtm::SortedList;
using std::ostream;
using std::string;

/**
 * @brief Class representing a person who can have tasks assigned.
 */
class Person {
private:
    string m_name;
    SortedList<Task> m_tasks;

public:
    /**
     * @brief Constructor to create a Person object.
     *
     * @param name The name of the person (default is an empty string).
     */
    Person(const string& name = "");

    /**
     * @brief Gets the name of the person.
     *
     * @return string The name of the person.
     */
    string getName() const;

    /**
     * @brief Gets the list of tasks assigned to the person.
     *
     * @return const SortedList<Task>& The list of tasks assigned to the person.
     */
    const SortedList<Task>& getTasks() const;

    /**
     * @brief Sets the list of tasks for the person.
     *
     * @param tasks The list of tasks to be set.
     */
    void setTasks(const SortedList<Task>& tasks);

    /**
     * @brief Assigns a new task to the person.
     *
     * @param task The task to be assigned.
     */
    void assignTask(const Task& task);

    /**
     * @brief Completes the highest priority task from the list of tasks.
     *
     * @return int The ID of the completed task.
     */
    int completeTask();

    /**
     * @brief Gets the highest priority task assigned to the person.
     *
     * @return const Task& The highest priority task.
     */
    const Task& getHighestPriorityTask() const;

    /**
     * @brief Overloaded output stream operator for printing Person details.
     *
     * @param os The output stream.
     * @param person The Person object to be printed.
     * @return ostream& The output stream with the Person details.
     */
    friend ostream &operator<<(ostream &os, const Person &person);
};
