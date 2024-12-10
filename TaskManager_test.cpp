#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <fstream>
#include <regex>
#include <limits>
#include <iomanip>
#include <chrono>
#include <stdexcept>

using namespace std;

std::string getCurrentDateTime() {
    // Get current time
    std::time_t now = std::time(nullptr);
    std::tm currentTime = *std::localtime(&now);

    // Buffer to hold the formatted string
    char buf[64];
    
    // Format: "YYYY-MM-DD hh:mm AM/PM"
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M %p", &currentTime);

    return std::string(buf);
}



std::tm stringToTime(const std::string& dateTime) {
    std::tm tm = {};
    std::istringstream ss(dateTime);
    ss >> std::get_time(&tm, "%Y-%m-%d %I:%M %p");
    if (ss.fail()) {
        throw std::invalid_argument("Failed to parse the date-time string.");
    }
    return tm;
}


// Task Class
class Task {
private:
    string title;
    string description;
    string priority;  // Low, Medium, High
    string dueDate;   // Represent date as "YYYY-MM-DD"
    bool completed;   // Indicates whether the task is completed or not

public:
    Task(string title, string description, string priority, string dueDate)
        : title(title), description(description), priority(priority), dueDate(dueDate), completed(false) {}

    string getTitle() const { return title; }
    string getDescription() const { return description; }
    string getPriority() const { return priority; }
    string getDueDate() const { return dueDate; }
    bool isCompleted() const { return completed; }

    void setTitle(const string& newTitle) { title = newTitle; }
    void setDescription(const string& newDescription) { description = newDescription; }
    void setPriority(const string& newPriority) { priority = newPriority; }
    void setDueDate(const string& newDueDate) { dueDate = newDueDate; }

    void markComplete() { completed = true; }

    void displayTask() const {
        cout << "Title: " << title << endl;
        cout << "Description: " << description << endl;
        cout << "Priority: " << priority << endl;
        cout << "Due Date: " << dueDate << endl;
        cout << (completed ? "Status: Completed" : "Status: Pending") << endl;
        cout << "-------------------------------\n";
    }
};

class TaskManager {
private:
    vector<Task> tasks; // A vector that stores Task objects

public:
    // Allows the user to create a new task
    void createTask(const Task& task) {
        tasks.push_back(task);
        //cout << "Task created successfully.\n";
    }

    // Public method to access the tasks vector
    vector<Task>& getTasks() {
        return tasks;  // Return a reference to the tasks vector
    }

    // Mark a task as complete
    void markTaskComplete() {
        string title;
        cout << "Enter the title of the task to mark as complete: ";
        cin.ignore();
        getline(cin, title);

        for (Task& task : tasks) {
            if (task.getTitle() == title) {
                task.markComplete();
                cout << "Task marked as complete.\n";
                return;
            }
        }
        cout << "Task not found.\n";
    }
     

    // Edit an existing task
    void editTask() {
        string title;
        cout << "Enter the title of the task to edit: ";
        cin.ignore();
        getline(cin, title);

        for (Task& task : tasks) {
            if (task.getTitle() == title) {
                string newTitle, newDescription, newPriority, newDueDate;
                cout << "Enter new title (leave empty to keep current): ";
                getline(cin, newTitle);
                cout << "Enter new description (leave empty to keep current): ";
                getline(cin, newDescription);
                cout << "Enter new priority (Low, Medium, High) (leave empty to keep current): ";
                getline(cin, newPriority);
                cout << "Enter new due date (YYYY-MM-DD HH:MM AM/PM) (leave empty to keep current): ";
                getline(cin, newDueDate);

                if (!newTitle.empty()) task.setTitle(newTitle);
                if (!newDescription.empty()) task.setDescription(newDescription);
                if (!newPriority.empty()) task.setPriority(newPriority);
                if (!newDueDate.empty()) task.setDueDate(newDueDate);

                cout << "Task updated successfully.\n";
                return;
            }
        }
        cout << "Task not found.\n";
    }

    // Delete a task
    void deleteTask() {
        string title;
        cout << "Enter the title of the task to delete: ";
        cin.ignore();
        getline(cin, title);

        auto it = remove_if(tasks.begin(), tasks.end(), [&](const Task& task) {
            return task.getTitle() == title;
        });

        if (it != tasks.end()) {
            tasks.erase(it, tasks.end());
            cout << "Task deleted successfully.\n";
        } else {
            cout << "Task not found.\n";
        }
    }

    // Display all tasks
    void displayTasks() {
        if (tasks.empty()) {
            cout << "No tasks available.\n";
        } else {
            for (const Task& task : tasks) {
                task.displayTask();
            }
        }
    }

    // Display the task management menu
    void displayMenu() {
        cout << "\nTask Management Menu\n";
        cout << "1. Add Task\n";
        cout << "2. Edit Task\n";
        cout << "3. Delete Task\n";
        cout << "4. Mark Task as Complete\n";
        cout << "5. Display All Tasks\n";
        cout << "6. Exit\n";
    }
};
 

// User Class
class User {
private:
    string username;
    string password;
    TaskManager taskManager; // User-specific TaskManager

public:
    User(string username, string password) : username(username), password(password) {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }

    TaskManager& getTaskManager() { return taskManager; }

    void loadTasksFromFile(const string& filename) {
        // Load tasks from file into TaskManager's task list
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening task file for loading tasks.\n";
            return;
        }

        string line;
        while (getline(file, line)) {

   stringstream ss(line);
            string title, description, priority, dueDate;
            getline(ss, title, ',');
            getline(ss, description, ',');
            getline(ss, priority, ',');
            getline(ss, dueDate, ',');

            // Create a new task and add it to the task manager
            Task task(title, description, priority, dueDate);
            taskManager.createTask(task); // Using createTask with the new task
        }
        file.close();
    }

    void saveTasksToFile(const string& filename) {
        ofstream file(filename, ios::trunc);
        if (!file) {
            cerr << "Error opening task file for saving tasks.\n";
            return;
        }

        for (const Task& task : taskManager.getTasks()) {
            file << task.getTitle() << "," << task.getDescription() << ","
                 << task.getPriority() << "," << task.getDueDate() << endl;
        }
        file.close();
    }
};



class Notification {
public:
    static void displayTaskAlerts(const std::vector<Task>& tasks) {
        std::string currentDateTime = getCurrentDateTime();
        std::cout << "Task Notifications:\n";
        
        for (const Task& task : tasks) {
            std::string taskDueDate = task.getDueDate();
            if (task.isCompleted()) continue;

            try {
                std::tm taskTime = stringToTime(taskDueDate);
                std::tm currentTime = stringToTime(currentDateTime);

                if (difftime(mktime(&currentTime), mktime(&taskTime)) > 0) {
                    std::cout << "ALERT! Task \"" << task.getTitle() << "\" is overdue! Due at: " << taskDueDate << "\n";
                } else if (difftime(mktime(&taskTime), mktime(&currentTime)) < 86400) {
                    std::cout << "ALERT! Task \"" << task.getTitle() << "\" is due soon! Due at: " << taskDueDate << "\n";
                }
            } catch (const std::invalid_argument& e) {
                std::cout << "Invalid date format for task \"" << task.getTitle() << "\". Skipping alert.\n";
            }
        }
        
        std::cout << "-------------------------------\n";
    }
};


// Authentication Class
class Authentication {
private:
    vector<User> users;
    string filename;

    void loadUsers() {
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening file for loading users.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            size_t pos = line.find(',');
            if (pos == string::npos) continue;

            string username = line.substr(0, pos);
            string password = line.substr(pos + 1);
            users.push_back(User(username, password));
        }
        file.close();
    }

    void saveUsers() {
        ofstream file(filename, ios::trunc);
        if (!file) {
       return;
        }

        for (const User& user : users) {
            file << user.getUsername() << "," << user.getPassword() << endl;
        }
        file.close();
    }

public:
    Authentication(string filename) : filename(filename) {
        loadUsers();
    }

    bool isValidUsername(const string& username) {
        return !username.empty() && regex_match(username, regex("^[a-zA-Z0-9]+$"));
    }

    bool isValidPassword(const string& password) {
        return password.length() >= 8 && regex_search(password, regex("[0-9]")) && regex_search(password, regex("[!@#$%^&*]"));
    }

    bool loginUser(const string& username, const string& password) {
    for (User& user : users) {
        if (user.getUsername() == username && user.getPassword() == password) {
            // Display task alerts when the user logs in
            Notification::displayTaskAlerts(user.getTaskManager().getTasks());
            return true;
        }
    }
    cout << "Invalid username or password.\n";
    return false;
}
    void registerUser(const string& username, const string& password) {
        if (isValidUsername(username) && isValidPassword(password)) {
            for (const User& user : users) {
                if (user.getUsername() == username) {
                    cout << "Username already taken. Try another one.\n";
                    return;
                }
            }
            users.push_back(User(username, password));
            saveUsers();
            cout << "User registered successfully.\n";
        } else {
            cout << "Invalid username or password format.\n";
        }
    }

    User* getUser(const string& username) {
        for (User& user : users) {
            if (user.getUsername() == username) {
                return &user;
            }
        } 
        return nullptr; 
}
 };






int main() {
    Authentication auth("users.txt");
    string username, password;
    int choice;

    // Welcome message and main menu for login/register
    cout << "Welcome to Task Manager\n";
    cout << "1. Register\n2. Login\nChoose an option: ";
    cin >> choice;

    // Input username and password
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    // Handle registration or login
    if (choice == 1) {
        // Register user, and automatically log them in if registration is successful
        auth.registerUser(username, password);
        cout << "Registration successful.\n";
    } else if (choice == 2) {
        // Attempt login
        if (auth.loginUser(username, password)) {
            cout << "Login successful.\n";
        } else {
            cout << "Invalid username or password.\n";
            return 0; // Exit if login fails
        }
    } else {
        cout << "Invalid choice.\n";
        return 0; // Exit if invalid choice
    }

    // Get the user object after login/registration
    User* currentUser = auth.getUser(username);

    if (currentUser == nullptr) {
        // If the user object is not found or initialized correctly, show an error and exit
        cout << "Error: User not found or initialization failed.\n";
        return 1; // Exit with error code
    }

    string tasksFile = username + "_tasks.txt";  // Name for the task file

    // Load tasks from file for the current user
    currentUser->loadTasksFromFile(tasksFile);

    cout << "Welcome, " << username << "!\n";
     Notification::displayTaskAlerts(currentUser->getTaskManager().getTasks());
    // Start task management loop
    while (true) {
        // Display Task Management Menu
        currentUser->getTaskManager().displayMenu();
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        // Prompt for task management option
        cout << "Choose an Option: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                // Add task
                string title, description, priority, dueDate;
                cout << "Enter task title: ";
                cin.ignore();  // To clear input buffer
                getline(cin, title);
                cout << "Enter task description: ";
                getline(cin, description);
                cout << "Enter task priority (Low, Medium, High): ";
                getline(cin, priority);
                cout << "Enter task due date (YYYY-MM-DD): ";
                getline(cin, dueDate);

                // Create and add new task
                Task newTask(title, description, priority, dueDate);
                currentUser->getTaskManager().createTask(newTask);
                cout << "Task Created Success!"; 
                break;
            }
            case 2:
                currentUser->getTaskManager().editTask();
                break;
            case 3:
                currentUser->getTaskManager().deleteTask();
                break;
            case 4:
                currentUser->getTaskManager().markTaskComplete();
                break;
            case 5:
                currentUser->getTaskManager().displayTasks();
                break;
            case 6:
                cout << "Exiting program...\n";
                currentUser->saveTasksToFile(tasksFile);  // Save tasks before exit
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
