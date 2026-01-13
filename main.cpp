#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Task{
    private:
        int id;
        string description;
        bool isCompleted;

    public:
        Task(int id, string desc) : id(id), description(desc), isCompleted(false) {}
        int getId() const { return id; }
        string getDescription() const { return description; }
        bool getStatus() const { return isCompleted; }

    // Setter: Allows specific WRITE access
        void markComplete() { 
            isCompleted = true; 
         }
};