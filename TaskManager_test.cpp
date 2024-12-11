#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <algorithm>
#include <cctype>
#include <sstream>
// Task struct
struct Task {
    std::string title;
    std::string description;
    std::string dueDate;
    std::string priority;
    bool isCompleted = false;
};

// Function declarations
bool isValidDate(const std::string& date);
bool isValidPriority(const std::string& priority);
void displayTasks(const std::vector<Task>& tasks);
void saveTasksToFile(const std::map<std::string, std::vector<Task>>& userTasks, const std::string& username);
void loadTasksFromFile(std::map<std::string, std::vector<Task>>& userTasks);
bool authenticateUser(const std::string& username, const std::string& password, bool isLogin);
void deleteTask(std::vector<Task>& tasks, const std::string& title);
void markTaskComplete(std::vector<Task>& tasks, const std::string& title);

int main() {
    std::map<std::string, std::vector<Task>> userTasks; // Store tasks for each user
    loadTasksFromFile(userTasks); // Load tasks from file

    std::string username, password;
    bool loggedIn = false;

    std::cout << "Welcome to Task Manager!" << std::endl;

    while (!loggedIn) {
        char choice;
        std::cout << "Do you want to (r)egister or (l)ogin? ";
        std::cin >> choice;
        choice = std::tolower(choice); // Make the input case-insensitive

        if (choice == 'r') {
            std::cout << "Enter username: ";
            std::cin >> username;
            std::cout << "Enter password: ";
            std::cin >> password;

            if (authenticateUser(username, password, false)) {
                std::cout << "User registered successfully.\n";
                loggedIn = true;
            } else {
                std::cout << "Username already exists. Please login instead.\n";
            }
        } else if (choice == 'l') {
            std::cout << "Enter username: ";
            std::cin >> username;
            std::cout << "Enter password: ";
            std::cin >> password;

            if (authenticateUser(username, password, true)) {
                std::cout << "Login successful!" << std::endl;
                loggedIn = true;
            } else {
                std::cout << "Invalid username or password. Please try again.\n";
            }
        } else {
            std::cout << "Invalid option. Please try again.\n";
        }
    }

    // Task management menu for the logged-in user
    std::vector<Task>& tasks = userTasks[username];
    while (true) {
        std::cout << "\nTask Management Menu\n";
        std::cout << "1. Add Task\n";
        std::cout << "2. Edit Task\n";
        std::cout << "3. Delete Task\n";
        std::cout << "4. Mark Task as Complete\n";
        std::cout << "5. Display All Tasks\n";
        std::cout << "6. Exit\n";
        std::cout << "Select an option: ";

        int option;
        std::cin >> option;

        if (option == 1) {
            Task newTask;
            std::cout << "Enter task title: ";
            std::cin.ignore(); 
            std::getline(std::cin, newTask.title);
            std::cout << "Enter task description: ";
            std::getline(std::cin, newTask.description);

            // Validate date input
            do {
                std::cout << "Enter task due date (e.g., YYYY-MM-DD): ";
                std::getline(std::cin, newTask.dueDate);
            } while (!isValidDate(newTask.dueDate));

            // Validate priority input
            do {
                std::cout << "Enter task priority (e.g., High, Medium, Low): ";
                std::getline(std::cin, newTask.priority);
            } while (!isValidPriority(newTask.priority));

            tasks.push_back(newTask);
            saveTasksToFile(userTasks, username);
            std::cout << "Task added successfully.\n";

        } else if (option == 2) {
            std::string taskTitle;
            std::cout << "Enter the title of the task to edit: ";
            std::cin.ignore();
            std::getline(std::cin, taskTitle);
            bool taskFound = false;

            for (auto& task : tasks) {
                if (task.title == taskTitle) {
                    taskFound = true;
	            std::cout << "Editing task: " << task.title << "\n";
                    std::cout << "Enter new task title: ";
                    std::getline(std::cin, task.title); 
                    std::cout << "Editing task: " << task.title << "\n";
                    std::cout << "Enter new task description (leave blank to keep current): ";
                    std::getline(std::cin, task.description);  // Get the new description
		    if (task.description.empty()) {
			    std::cout << "Description not changed, keeping the original: " << task.description << "\n";
                     }
                    do {
                        std::cout << "Enter new task due date (e.g., YYYY-MM-DD): ";
                        std::getline(std::cin, task.dueDate);
                    } while (!isValidDate(task.dueDate));
                    do {
                        std::cout << "Enter new task priority (e.g., High, Medium, Low): ";
                        std::getline(std::cin, task.priority);
                    } while (!isValidPriority(task.priority));
                    saveTasksToFile(userTasks, username);
                    std::cout << "Task updated successfully.\n";
                    break;
                }
            }

            if (!taskFound) {
                std::cout << "Task not found.\n";
            }

        } else if (option == 3) {
            std::string taskTitle;
            std::cout << "Enter the title of the task to delete: ";
            std::cin.ignore();
            std::getline(std::cin, taskTitle);
            deleteTask(tasks, taskTitle);
            saveTasksToFile(userTasks, username);

        } else if (option == 4) {
            std::string taskTitle;
            std::cout << "Enter the title of the task to mark as complete: ";
            std::cin.ignore();
            std::getline(std::cin, taskTitle);
            markTaskComplete(tasks, taskTitle);
            saveTasksToFile(userTasks, username);

        } else if (option == 5) {
            displayTasks(tasks);

        } else if (option == 6) {
            std::cout << "Exiting program." << std::endl;
            break;

        } else {
            std::cout << "Invalid option, please try again." << std::endl;
        }
    }

    return 0;
}
// Function implementations...

// Function to validate date format (YYYY-MM-DD)

bool isValidDate(const std::string& date) {
    // Check format: YYYY-MM-DD
    std::regex datePattern(R"((\d{4})-(\d{2})-(\d{2}))");
    std::smatch match;
    if (!std::regex_match(date, match, datePattern)) {
        return false; // Doesn't match the basic pattern
    }

    int year, month, day;
    std::istringstream(match[1].str()) >> year;
    std::istringstream(match[2].str()) >> month;
    std::istringstream(match[3].str()) >> day;

    // Check valid month and day range
    if (month < 1 || month > 12) {
        return false;
    }

    // Days in each month (index 0 is unused for simplicity)
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Adjust for leap year if it's February
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        daysInMonth[2] = 29;
    }

    if (day < 1 || day > daysInMonth[month]) {
        return false;
    }

    return true;
}

// Function to validate priority input (case-insensitive)
bool isValidPriority(const std::string& priority) {
    std::string lowerPriority = priority;
    std::transform(lowerPriority.begin(), lowerPriority.end(), lowerPriority.begin(), ::tolower);
    return (lowerPriority == "high" || lowerPriority == "medium" || lowerPriority == "low");
}

// Function to display tasks
void displayTasks(const std::vector<Task>& tasks) {
    if (tasks.empty()) {
        std::cout << "No tasks to display." << std::endl;
        return;
    }
    for (const auto& task : tasks) {
        std::cout << "Title: " << task.title << ", Description: " << task.description
                  << ", Due Date: " << task.dueDate << ", Priority: " << task.priority
                  << " (" << (task.isCompleted ? "Completed" : "Pending") << ")" << std::endl;
    }
}

// Function to save tasks to a file
void saveTasksToFile(const std::map<std::string, std::vector<Task>>& userTasks, const std::string& username) {
    std::ofstream file("tasks.txt", std::ios::trunc);
    for (const auto& [user, tasks] : userTasks) {
        file << user << "\n";
        for (const auto& task : tasks) {
            file << task.title << "\n"
                 << task.description << "\n"
                 << task.dueDate << "\n"
                 << task.priority << "\n"
                 << task.isCompleted << "\n";
        }
        file << "END\n"; // Delimiter for tasks of a user
    }
    file.close();
}

// Function to load tasks from a file
void loadTasksFromFile(std::map<std::string, std::vector<Task>>& userTasks) {
    std::ifstream file("tasks.txt");
    if (!file.is_open()) {
        return;
    }
    std::string line, username;
    while (std::getline(file, username)) {
        if (username == "END") {
            continue;
        }
        std::vector<Task> tasks;
        Task task;
        while (std::getline(file, line) && line != "END") {
            task.title = line;
            std::getline(file, task.description);
            std::getline(file, task.dueDate);
            std::getline(file, task.priority);
            file >> task.isCompleted;
            file.ignore(); // Skip the newline character after isCompleted
            tasks.push_back(task);
        }
        userTasks[username] = tasks;
    }
    file.close();
}

// Function to authenticate user
bool authenticateUser(const std::string& username, const std::string& password, bool isLogin) {
    std::ifstream file("auth.txt");
    std::string storedUsername, storedPassword;
    while (file >> storedUsername >> storedPassword) {
        if (storedUsername == username) {
            if (isLogin) {
                return storedPassword == password; // Check password for login
            } else {
                return false; // Username already exists for registration
            }
        }
    }
    file.close();
    if (!isLogin) {
        // Register the new user
        std::ofstream outFile("auth.txt", std::ios::app);
        outFile << username << " " << password << "\n";
        outFile.close();
        return true;
    }
    return false;
}

// Function to delete a task
void deleteTask(std::vector<Task>& tasks, const std::string& title) {
    auto it = std::remove_if(tasks.begin(), tasks.end(), [&](const Task& t) {
        return t.title == title;
    });
    if (it != tasks.end()) {
        tasks.erase(it, tasks.end());
        std::cout << "Task deleted successfully.\n";
    } else {
        std::cout << "Task not found.\n";
    }
}

// Function to mark a task as complete
void markTaskComplete(std::vector<Task>& tasks, const std::string& title) {
    for (auto& task : tasks) {
        if (task.title == title) {
            task.isCompleted = true;
            std::cout << "Task marked as completed." << std::endl;
            return;
        }
    }
    std::cout << "Task not found." << std::endl;
}
