#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <deque>
using namespace std;



string color(string text, string colorCode) {
    // 31=Red, 32=Green, 33=Yellow, 34=Blue, 36=Cyan, 0=Reset
    return "\033[" + colorCode + "m" + text + "\033[0m";
}
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
class Task{
    protected:
        int id;
        string description;
        bool isCompleted;

    public:
        //normal constructor
        Task(int id, string desc) : id(id), description(desc), isCompleted(false) {}
        virtual ~Task() {} 
        
        virtual void printTask() const {
            cout << (isCompleted ? "[X] " : "[ ] ") 
                << "ID " << id << ": " << description << endl;
        }
        //loading constructor
        Task(int id, string desc, bool status) : id(id), description(desc), isCompleted(status) {}
        int getId() const { return id; }
        string getDescription() const { return description; }
        bool getStatus() const { return isCompleted; }

        void markComplete() { 
            isCompleted = true; 
        }
};

class TimedTask : public Task {
private:
    string deadline; 

public:
    // Constructor passes ID and Desc up to the Base class
    TimedTask(int id, string desc, string time) 
        : Task(id, desc), deadline(time) {}

    // OVERRIDE the print function
    void printTask() const override {
        // We can access 'isCompleted' etc. because they are 'protected'
        cout << (isCompleted ? "[X] " : "[ ] ") 
             << "ID " << id << ": " << description 
             << " [DUE: " << deadline << "]" << endl; // <--- The Extra Part
    }
};

class TaskManager {
private:
    // This vector lives on the stack, but it stores data on the HEAP.
    vector<Task*> tasks; 
    int nextId = 1;
    deque<int> addedTaskIds;

public:
    ~TaskManager() {
        for (Task* t : tasks) {
            delete t; // Free the heap memory
        }
    }
    // Add a new task to the list
    void addTask(string desc) {
        Task* newTask = new Task(nextId, desc);
        tasks.push_back(newTask); 
        addedTaskIds.push_back(nextId);
        cout << ">> Task added successfully! (ID: " << nextId << ")\n";
        nextId++;
    }
    void addTimedTask(string desc, string deadline) {
        Task* newTask = new TimedTask(nextId, desc, deadline); 
        tasks.push_back(newTask); 
        addedTaskIds.push_back(nextId++);
        cout << ">> Timed Task added.\n";
    }
    // Mark a task as done
    void completeTask(int id) {
        for (auto &t : tasks) {
            if (t->getId() == id) {
                t->markComplete();
                cout << ">> Task " << id << " marked as done.\n";
                return;
            }
        }
        cout << ">> Task ID not found.\n";
    }

    // Undo last task addition
    void undo() {
        if (addedTaskIds.empty()) {
            cout << ">> Nothing to undo.\n";
            return;
        }
        
        int lastId = addedTaskIds.back();
        addedTaskIds.pop_back();
        
        // Remove task from vector
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if ((*it)->getId() == lastId) {
                delete *it; // Free the heap memory
                tasks.erase(it);
                cout << ">> Undone: Removed task " << lastId << "\n";
                return;
            }
        }
    }
    void sortTasks() {
        if (tasks.empty()) return;

        cout << "\nSort by:\n1. ID (Default)\n2. Status (Todo First)\nChoice: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            sort(tasks.begin(), tasks.end(), [](Task* a, Task* b) {
                return a->getId() < b->getId(); 
            });
        } else if (choice == 2) {
            sort(tasks.begin(), tasks.end(), [](Task* a, Task* b) {
                return a->getStatus() < b->getStatus(); 
            });
        }
    }

    // Show all tasks
    void listTasks() {
        if (tasks.empty()) {
            cout << ">> No tasks found.\n";
            return;
        }

        cout << "\n--- TO-DO LIST ---\n";
        for (const auto &t : tasks) {
            cout << (t->getStatus() ? "[X] " : "[ ] ");
            cout << "ID " << t->getId() << ": " << t->getDescription() << endl;
        }
        cout << "------------------\n";
    }
    void saveToFile() {
        ofstream outFile("tasks.txt"); 
        
        if (!outFile) {
            cout << ">> Error: Could not save data!\n";
            return;
        }

        for (const auto &t : tasks) {
            //ID, Description, Status
            outFile << t->getId() << "," << t->getDescription() << "," << t->getStatus() << "\n";
        }
        
        outFile.close(); 
        cout << ">> Data saved to tasks.txt\n";
    }

    void loadFromFile() {
        ifstream inFile("tasks.txt");
        if (!inFile) {
            return; 
        }

        tasks.clear();
        addedTaskIds.clear(); 

        int id;
        int statusInt; 
        string desc, dummy; 

        while (inFile >> id) {
            //Read the comma 
            getline(inFile, dummy, ','); 
            
            //Read the description
            getline(inFile, desc, ',');

            //Read the status 
            inFile >> statusInt;


            bool isDone = (statusInt == 1);
            Task* loadedTask = new Task(id, desc, isDone);
            tasks.push_back(loadedTask);
            
            // Update the counter
            if (id >= nextId) {
                nextId = id + 1;
            }
        }
        inFile.close();
        cout << ">> Data loaded from tasks.txt.\n";
    }
};

int main() {
    TaskManager manager;
    manager.loadFromFile();
    int choice;
    string desc;
    int id;

    while (true) {
       clearScreen(); // Clears terminal every loop
        
        // --- UI HEADER ---
        cout << "\n========================================\n";
        cout << "       " << color("TASK MASTER", "36") << "\n";
        cout << "========================================\n";
        manager.listTasks(); // Show tasks continuously
        cout << "========================================\n";
        cout << "  1. " << color("+", "32") << " Add Task\n";
        cout << "  2. " << color("X", "32") << " Complete Task\n";
        cout << "  3. " << color("<", "31") << " Undo Last Add\n";
        cout << "  4. " << color("^", "35") << " Sort List\n";
        cout << "  0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 0){
            manager.saveToFile();
            cout << "Goodbye!\n";
            break;
        }

        switch (choice) {
            case 1: {
                cout << "Type (1) Normal or (2) Timed? ";
                int type;
                cin >> type;
                cin.ignore(); // Clear buffer

                cout << "Enter task description: ";
                getline(cin, desc); 
                
                if (type == 2) {
                    cout << "Enter Deadline (e.g. 'Tonight'): ";
                    string dl;
                    getline(cin, dl);
                    manager.addTimedTask(desc, dl);
                } else {
                    manager.addTask(desc);
                }
                break;
            }
            case 2:
                cout << "Enter Task ID to complete: ";
                cin >> id;
                manager.completeTask(id);
                break;
            case 3:
                manager.undo();
                break;
            case 4:
                manager.sortTasks();
                break;
            default:
                cout << "Invalid choice.\n";
        }
    }
    return 0;
}