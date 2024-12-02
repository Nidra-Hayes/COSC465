#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>  // For formatting dates

using namespace std;

// Helper function to convert a date-time string (YYYY-MM-DD HH:MM AM/PM) to a tm struct
tm stringToTime(const string& dateStr) {
    tm timeStruct = {};
    stringstream ss(dateStr);
    ss >> get_time(&timeStruct, "%Y-%m-%d %I:%M %p");  // Read the full date-time including AM/PM
    return timeStruct;
}

// Helper function to get the current date-time as a string in YYYY-MM-DD HH:MM AM/PM format
string getCurrentDateTime() {
    time_t t = time(0);
    struct tm* now = localtime(&t);
    stringstream ss;
    ss << (1900 + now->tm_year) << "-" << setw(2) << setfill('0') << (1 + now->tm_mon) << "-" 
       << setw(2) << setfill('0') << now->tm_mday << " "
       << setw(2) << setfill('0') << (now->tm_hour % 12 == 0 ? 12 : now->tm_hour % 12) << ":" 
       << setw(2) << setfill('0') << now->tm_min << " " 
       << (now->tm_hour >= 12 ? "PM" : "AM");
    return ss.str();
}

//Task Class used by the TaskManager class to create a Task.
class Task {
private:
    string title;
    string description;
    string priority;  // Low, Medium, High
    string dueDate;   // Represent date as "YYYY-MM-DD"
    bool completed;   // Indicates whether the task is completed or not
//Constructor initializes the object for the Task Class
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

// Notification Class to alert the user of overdue or upcoming tasks
class Notification {
public:
    static void displayTaskAlerts(const vector<Task>& tasks) {
        string currentDateTime = getCurrentDateTime();
        cout << "Task Notifications:\n";
        for (const Task& task : tasks) {
            string taskDueDate = task.getDueDate();
            if (task.isCompleted()) continue;

            tm taskTime = stringToTime(taskDueDate);
            tm currentTime = stringToTime(currentDateTime);

            // Check if the task is overdue or due soon
            if (difftime(mktime(&currentTime), mktime(&taskTime)) > 0) {
                cout << "ALERT! Task \"" << task.getTitle() << "\" is overdue! Due at: " << taskDueDate << "\n";
            } else if (difftime(mktime(&taskTime), mktime(&currentTime)) < 86400) {
                // If the task is due within 24 hours
                cout << "ALERT! Task \"" << task.getTitle() << "\" is due soon! Due at: " << taskDueDate << "\n";
            }
        }
        cout << "-------------------------------\n";
    }
};

//Task Manager Class
class TaskManager {
private:
    vector<Task> tasks; //A vector that stores Task objects

//Allows the user to create a new task
public:
    void createTask() {
        string title, description, priority, dueDate;
        cout << "Enter task title: ";
        cin.ignore();  // Clear the input buffer
        getline(cin, title);
        cout << "Enter task description: ";
        getline(cin, description);
        cout << "Enter task priority (Low, Medium, High): ";
        cin >> priority;
        cout << "Enter task due date (YYYY-MM-DD HH:MM AM/PM): ";
        cin.ignore(); //Clear input again for the time
        getline(cin, dueDate);
        //cin >> dueDate;

        Task newTask(title, description, priority, dueDate);
        tasks.push_back(newTask);
        cout << "Task created successfully.\n";
    }

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

    void editTask() { //Allows the user to edit an existing task
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

    void displayTasks() {
        if (tasks.empty()) {
            cout << "No tasks available.\n";
        } else {
            for (const Task& task : tasks) {
                task.displayTask();
            }
        }
    }

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

class User {
private:
    string username;
    string password;
    TaskManager taskManager;  // Each user has their own task manager

public:
    User(string username, string password) : username(username), password(password) {}

    string getUsername() const {
        return username;
    }

    string getPassword() const {
        return password;
    }

    TaskManager& getTaskManager() {
        return taskManager;
    }
};

class Authentication {
private:
    vector<User> users; // A vector to store registered users
    string filename; // The filename to store and load user data

    void loadUsers() {
        // Loads user data from a file into the 'users' vector
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening file for loading users.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            size_t pos = line.find(',');
            if (pos == string::npos) {
                continue;  // Skip malformed lines
            }

            string username = line.substr(0, pos); // Extract username
            string password = line.substr(pos + 1); // Extract password
            users.push_back(User(username, password)); // Add user to the vector
        }
        file.close(); // Close the file after reading
    }
     // Saves user data from the 'users' vector to the file
    void saveUsers() {
        ofstream file(filename, ios::trunc);
        if (!file) {
            cerr << "Error opening file for saving users.\n";
            return;
        }

        for (const User& user : users) {
            file << user.getUsername() << "," << user.getPassword() << endl; // Write each user to the file
        }
        file.close();
    }

public:
    Authentication(string filename) : filename(filename) {
        loadUsers(); // Load users from the file when the Authentication object is created
    }

    bool isValidUsername(const string& username) {
         // Checks if the username is non-empty and contains only alphanumeric characters
        return !username.empty() && regex_match(username, regex("^[a-zA-Z0-9]+$"));
    }

    bool isValidPassword(const string& password) {
        // Checks if the password is at least 8 characters long and contains a number and a special character
        return password.length() >= 8 && regex_search(password, regex("[0-9]")) && regex_search(password, regex("[!@#$%^&*]"));
    }

    void registerUser(string& username, string& password) {
        // Prompts the user for a valid username and password, and registers the user if valid
        while (!isValidUsername(username)) {
            cout << "Error: Invalid username. Only alphanumeric characters are allowed, and it cannot be empty.\n";
            cout << "Please enter a valid username: ";
            cin >> username;
        }

        // Check if username already exists
        for (const User& user : users) {
            if (user.getUsername() == username) {
                cout << "Error: Username already taken.\n";
                cout << "Please enter a different username: ";
                cin >> username;
                break;
            }
        }

        while (!isValidPassword(password)) {
            cout << "Error: Invalid password. It must be at least 8 characters long, and contain at least one number and one special character.\n";
            cout << "Please enter a valid password: ";
            cin >> password;
        }
        // If username and password are valid, register the user and save the data to the file
        users.push_back(User(username, password));
        saveUsers(); // Save the updated user data to the file
        cout << "Registration successful.\n";
    }

    // Checks if the username and password match any existing user for login
    bool loginUser(string username, string password) {
        for (User& user : users) {
            if (user.getUsername() == username && user.getPassword() == password) {
                return true; //Log in successful
            }
        }
        return false; //Log in failed
    }

    User* getUserByUsername(string username) { //User* points to the user object
        // Retrieves the User object by username (if it exists)
        for (User& user : users) {
            if (user.getUsername() == username) {
                return &user; // Return the User object
            }
        }
        return nullptr; // Return nullptr if user not found
    }
};

int main() {
    Authentication auth("users.txt");
    User* currentUser = nullptr;

    int choice;
    string username, password;
    
    cout << "1. Register\n2. Login\nChoice: ";
    cin >> choice;

    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    if (choice == 1) {
       // Register user, and automatically log them in if registration is successful
        auth.registerUser(username, password);
    } else if (choice == 2) {
        // Attempt login
        if (!auth.loginUser(username, password)) {
            return 0; // Exit if login fails
        }
    } else {
        cout << "Invalid choice.\n";
        return 0; // Exit if invalid choice
    }

    // Get the user object
    User* currentUser = auth.getUser(username);
    if (currentUser) {
        cout << "Welcome, " << username << "!\n";

        while (true) {
            cout << "\nTask Management Menu\n";
            cout << "1. Add Task\n";
            cout << "2. Display All Tasks\n";
            cout << "6. Exit\n";
            cout << "Choose an option: ";
            cin >> choice;

            switch (choice) {
                case 1: {
                    string title, description, priority, dueDate;
                    cout << "Enter task title: ";
                    cin.ignore();
                    getline(cin, title);
                    cout << "Enter task description: ";
                    getline(cin, description);
                    cout << "Enter task priority (Low, Medium, High): ";
                    cin >> priority;
                    cout << "Enter task due date (YYYY-MM-DD): ";
                    cin >> dueDate;
                    Task newTask(title, description, priority, dueDate);
                    currentUser->addTask(conn, newTask);
                    break;
                }
                case 2:
                    currentUser->displayTasks(conn);
                    break;
                case 6:
                    cout << "Exiting program...\n";
                    mysql_close(conn);
                    return 0;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        }
    }

    return 0;
}
