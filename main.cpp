#include <iostream>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <fstream>

class CRUD {
private:
    std::map<int, std::map<std::string, std::string>> tasks;

    void load_tasks_json() {
        std::ifstream infile("tasks.json");
        if (!infile.is_open()) {
            std::cout << "No tasks file found, starting with empty tasks." << std::endl;
            return;
        }
        nlohmann::json j;
        infile >> j;
        tasks.clear();
        for (auto it = j.begin(); it != j.end(); ++it) {
            int task_id = std::stoi(it.key());
            std::map<std::string, std::string> task_details = it.value().get<std::map<std::string, std::string>>();
            tasks[task_id] = task_details;
        }
        std::cout << "Tasks loaded successfully from tasks.json" << std::endl;
    }

    void save_tasks_json() {
        nlohmann::json j;
        for (const auto& task : tasks) {
            j[std::to_string(task.first)] = task.second;
        }
        std::ofstream outfile("tasks.json");
        if (!outfile.is_open()) {
            std::cout << "Error saving tasks to file." << std::endl;
            return;
        }
        outfile << j.dump(4);
    }

    void show_tasks() {
        if (tasks.empty()) {
            std::cout << "No tasks" << std::endl;
            return;
        }
        for (const auto& task : tasks) {
            std::cout << "Task: №" << task.first << std::endl;
            std::cout << "Task name: " << task.second.at("name") << std::endl;
            std::cout << "Description: " << task.second.at("description") << std::endl << std::endl;
        }
    }

    void handle_add_task() {
        if (tasks.empty()) {
            std::cout << "Enter task name: ";
            std::string task_name;
            std::getline(std::cin, task_name);
            std::cout << "Enter description: ";
            std::string description;
            std::getline(std::cin, description);
            tasks[1] = {{"name", task_name}, {"description", description}};
        } else {
            std::cout << "Enter task name: ";
            std::string task_name;
            std::getline(std::cin, task_name);
            std::cout << "Enter description: ";
            std::string description;
            std::getline(std::cin, description);
            tasks[tasks.rbegin()->first + 1] = {{"name", task_name}, {"description", description}};
        }
        save_tasks_json();
    }

    void handle_delete_task(){
        show_tasks();
        std::cout << "Enter task number to delete: ";
        int task_number;
        std::cin >> task_number;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        tasks.erase(task_number);
        std::cout << "Task deleted successfully" << std::endl;
        save_tasks_json();
    }

    void handle_update_task(){
        show_tasks();
        std::cout << "Enter task number to update: ";
        int task_number;
        std::cin >> task_number;
        std::cout << "What do you want to update?" << std::endl;
        std::cout << "1. Task name" << std::endl;
        std::cout << "2. Description" << std::endl;
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice == 1) {
            std::cout << "Enter new task name: ";
            std::string task_name;
            std::getline(std::cin, task_name);
            tasks[task_number]["name"] = task_name;
        } else if (choice == 2) {
            std::cout << "Enter new description: ";
            std::string description;
            std::getline(std::cin, description);
            tasks[task_number]["description"] = description;
        } else {
            std::cout << "Invalid choice" << std::endl;
            handle_update_task();
        }
        save_tasks_json();
    }

public:
    CRUD() {
        load_tasks_json();
    }

    ~CRUD() {
        save_tasks_json();
    }

    void run() {
        std::string command;
        while (true) {
            std::cout << "Enter command ";
            std::getline(std::cin, command);
            if (command == "exit") {
                std::cout << "Exiting" << std::endl;
                save_tasks_json();
                break;
            }
            if (command == "show") {
                show_tasks();
            } else if (command == "add") {
                handle_add_task();
            } else if (command == "update") {
                handle_update_task();
            } else if (command == "delete") {
                handle_delete_task();
            } else {
                std::cout << "Invalid command" << std::endl;
            }

        }
    }
};

int main() {
    CRUD crud;
    crud.run();
    return 0;
}