#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class Task{
    private:
        int id;
        string description;
        bool isCompleted;

    public:
        //normal constructor
        Task(int id, string desc) : id(id), description(desc), isCompleted(false) {}
        //loading constructor
        Task(int id, string desc, bool status) : id(id), description(desc), isCompleted(status) {}
        int getId() const { return id; }
        string getDescription() const { return description; }
        bool getStatus() const { return isCompleted; }

        void markComplete() { 
            isCompleted = true; 
         }
};


class TaskManager {
private:
    // This vector lives on the stack, but it stores data on the HEAP.
    vector<Task> tasks; 
    int nextId = 1;
    deque<int> addedTaskIds;

public:
    // Add a new task to the list
    void addTask(string desc) {
        Task newTask(nextId, desc);
        tasks.push_back(newTask); 
        addedTaskIds.push_back(nextId);
        cout << ">> Task added successfully! (ID: " << nextId << ")\n";
        nextId++;
    }

    // Mark a task as done
    void completeTask(int id) {
        for (auto &t : tasks) {
            if (t.getId() == id) {
                t.markComplete();
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
            if (it->getId() == lastId) {
                tasks.erase(it);
                cout << ">> Undone: Removed task " << lastId << "\n";
                return;
            }
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
            cout << (t.getStatus() ? "[X] " : "[ ] ");
            cout << "ID " << t.getId() << ": " << t.getDescription() << endl;
        }
        cout << "------------------\n";
    }
    void saveToFile() {
        ofstream outFile("tasks.txt"); // Opens file for writing
        
        if (!outFile) {
            cout << ">> Error: Could not save data!\n";
            return;
        }

        for (const auto &t : tasks) {
            // Write: ID, Description, Status
            outFile << t.getId() << "," << t.getDescription() << "," << t.getStatus() << "\n";
        }
        
        outFile.close(); // Always close the file!
        cout << ">> Data saved to tasks.txt\n";
    }

    void loadFromFile() {
        ifstream inFile("tasks.txt");
        if (!inFile) {
            return; // File doesn't exist yet (first run), that's fine.
        }

        tasks.clear(); // Clear current list to avoid duplicates
        addedTaskIds.clear(); // Clear undo history

        int id;
        int statusInt; // We read 0 or 1 as an int first
        string desc, dummy; // 'dummy' catches the commas

        // While we can read an ID...
        while (inFile >> id) {
            // 1. Read the comma after ID (throw it away)
            getline(inFile, dummy, ','); 
            
            // 2. Read the description (stop at the next comma)
            getline(inFile, desc, ',');

            // 3. Read the status (0 or 1)
            inFile >> statusInt;

            // 4. Create the task and push it
            bool isDone = (statusInt == 1);
            Task loadedTask(id, desc, isDone);
            tasks.push_back(loadedTask);
            
            // Update the counter so new tasks don't reuse old IDs
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
        cout << "\n1. Add Task\n2. Complete Task\n3. List Tasks\n4. Undo\n0. Exit\nChoice: ";
        cin >> choice;

        if (choice == 0){
            manager.saveToFile();
            cout << "Goodbye!\n";
            break;
        }

        switch (choice) {
            case 1:
                cout << "Enter task description: ";
                cin.ignore(); 
                getline(cin, desc); 
                manager.addTask(desc);
                break;
            case 2:
                cout << "Enter Task ID to complete: ";
                cin >> id;
                manager.completeTask(id);
                break;
            case 3:
                manager.listTasks();
                break;
            case 4:
                manager.undo();
                break;
            default:
                cout << "Invalid choice.\n";
        }
    }
    return 0;
}