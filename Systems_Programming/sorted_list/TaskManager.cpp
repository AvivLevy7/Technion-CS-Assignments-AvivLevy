#include "TaskManager.h"
#include "Person.h"
#include "Task.h"


using std::cout;
using std::endl;


TaskManager::TaskManager(): index(0), task_counter(0){};

void TaskManager::assignTask(const string &personName, const Task &task){
    Person* per = nullptr;
    for (int i = 0; i < index; ++i){
        if(persons[i].getName() == personName){
            per = &persons[i];
        }
    }
    Task newTask = task;
    newTask.setId(task_counter++);
    if(per == nullptr){
        if(index >= MAX_PERSONS) {
            throw std::out_of_range("TaskManager is full");
        }
        Person p(personName);
        persons[index] = p;
        persons[index].assignTask(newTask);
        index++;
    } else {
        per->assignTask(newTask);
    }
}

void TaskManager::completeTask(const string &personName) {
    for (int i = 0; i < index; i++) {
        if (persons[i].getName() == personName) {
            persons[i].completeTask();
            return;
        }
    }
}

void TaskManager::bumpPriorityByType(TaskType type, int priority) {
    if (priority < 0) {
        return;
    }
    for (int i = 0; i < index; i++) {
        const SortedList<Task>& tasks = persons[i].getTasks();
        const SortedList<Task>& new_tasks = tasks.apply([type, priority](const Task &task) {
            if (taskTypeToString(task.getType()) == taskTypeToString(type)) {
            Task newTask(task.getPriority() + priority, task.getType(), task.getDescription());
                newTask.setId(task.getId());
                return newTask;
            } else {
                return task;
            }
        });
        persons[i].setTasks(new_tasks);

    }
}

void TaskManager::printAllEmployees() const {
    for (int i = 0; i < index; ++i) {
        cout << persons[i] << endl;
    }
}

void TaskManager::printAllTasks() const{
    SortedList<Task> tasksList;
    for (int i = 0; i < index; ++i) {
        for (const Task& task : persons[i].getTasks()) {
            tasksList.insert(task);
        }
    }
    for (const Task& t: tasksList) {
        cout << t << endl;
    }
}

void TaskManager::printTasksByType(TaskType type) const {
    SortedList<Task> tasksList;
    for (int i = 0; i < index; ++i) {
        for (const Task& task : persons[i].getTasks()) {
            tasksList.insert(task);
        }
    }

    tasksList = tasksList.filter( [type](const Task& task) {
        return taskTypeToString(task.getType()) == taskTypeToString(type);
    });

    for (const Task& t: tasksList) {
            cout << t << endl;
    }
}

