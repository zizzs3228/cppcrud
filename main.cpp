#include <iostream>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <utility>
#include <chrono>
#include <format>

class CRUD {
private:
    std::map<int, std::map<std::string, std::string>> tasks;

    std::string get_current_iso8601_time() {
        std::time_t now = std::time(nullptr);
        std::tm *gmt = std::gmtime(&now);
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", gmt);
        return std::string(buffer);
    }

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
        infile.close();
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
        outfile.close();
    }

    void show_tasks() {
        if (tasks.empty()) {
            std::cout << "No tasks" << std::endl;
            return;
        }
        for (const auto& task : tasks) {
            std::cout << "Task: №" << task.first << std::endl;
            std::cout << "Task name: " << task.second.at("name") << std::endl;
            std::cout << "Description: " << task.second.at("description") << std::endl;
            std::cout << "Priority: " << task.second.at("priority") << std::endl;
            std::cout << "Created at: " << task.second.at("created_at") << std::endl << std::endl;
        }
    }

    void show_tasks_by_priority(){
        if (tasks.empty()) {
            std::cout << "No tasks" << std::endl;
            return;
        }
        std::vector<std::pair<int, std::map<std::string, std::string>>> sorted_tasks(tasks.begin(), tasks.end());
        std::sort(sorted_tasks.begin(), sorted_tasks.end(), [](const std::pair<int, std::map<std::string, std::string>>& a, const std::pair<int, std::map<std::string, std::string>>& b){
            return std::stoi(a.second.at("priority")) > std::stoi(b.second.at("priority"));
        });
        for (const auto& task:sorted_tasks){
            std::cout << "Task: №" << task.first << std::endl;
            std::cout << "Task name: " << task.second.at("name") << std::endl;
            std::cout << "Description: " << task.second.at("description") << std::endl;
            std::cout << "Priority: " << task.second.at("priority") << std::endl;
            std::cout << "Created at: " << task.second.at("created_at") << std::endl << std::endl;
        }
    }

    void find_in_text(const std::string& search_query){
        if (tasks.empty()) {
            std::cout << "No tasks" << std::endl;
            return;
        }
        bool found = false;
        for (const auto& task:tasks){
            if (task.second.at("name").find(search_query) != std::string::npos || task.second.at("description").find(search_query) != std::string::npos){
                std::cout << "Task: №" << task.first << std::endl;
                std::cout << "Task name: " << task.second.at("name") << std::endl;
                std::cout << "Description: " << task.second.at("description") << std::endl;
                std::cout << "Priority: " << task.second.at("priority") << std::endl;
                std::cout << "Created at: " << task.second.at("created_at") << std::endl << std::endl;
                found = true;
            }
        }
        if (!found){
            std::cout << "No tasks found with the search query" << std::endl;
        }
    }

    std::string read_valid_priority() {
        while (true) {
            std::cout << "Enter priority (from 0 to 1000, 1000 is most important): ";
            std::string input;
            std::getline(std::cin, input);
            try {
                // Попытка преобразования строки в число
                int priority = std::stoi(input);
                // Проверка диапазона
                if (priority < 0 || priority > 1000) {
                    std::cout << "Priority must be between 0 and 1000. Please try again." << std::endl;
                    continue;
                }
                // Можно дополнительно проверить длину строки, если нужно (например, не более 4 символов)
                if (input.size() > 4) {
                    std::cout << "Input too long. Please enter a valid number." << std::endl;
                    continue;
                }
                return input;
            } catch (std::exception& e) {
                std::cout << "Invalid input. Please enter a number between 0 and 1000." << std::endl;
            }
        }
    }

    void handle_add_task() {
        std::cout << "Enter task name: ";
        std::string task_name;
        std::getline(std::cin, task_name);
        std::cout << "Enter description: ";
        std::string description;
        std::getline(std::cin, description);
        std::string priority = read_valid_priority();
        if (tasks.empty()) {
            tasks[1] = {{"name", task_name}, 
                {"description", description}, 
                {"priority", priority},
                {"created_at", get_current_iso8601_time()}
            };
        } else {
            tasks[tasks.rbegin()->first + 1] = {{"name", task_name}, 
                {"description", description}, 
                {"priority", priority},
                {"created_at", get_current_iso8601_time()}
            };
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
        std::cout << "3. Priority" << std::endl;
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
        } else if (choice == 3) {
            std::string priority = read_valid_priority();
            tasks[task_number]["priority"] = priority;

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
            } else if (command == "show -p") {
                show_tasks_by_priority();
            } else if (command.find("search") == 0) {
                find_in_text(command.substr(7));
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