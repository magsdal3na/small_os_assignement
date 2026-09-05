#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

//Job struct to set up variables from jobs.txt
struct Job {
    int id;
    int startTime;
    int size;
    int interval;
    string endState;
};

//Function to print the memory
void printMemory(const vector<int>& memory) {
    cout << "Memory: ";
    
    //Printing each page and whether or not it's empty or has job
    for (int page : memory) {
        if(page == 0) {
            cout << "[ ]";
        }
        else {
            cout << "[" << page << "]";
        }
    }
    cout << endl;
}

//Function to find First-Fit
int findFirstFit(const vector<int>& memory, int requiredSize) {
    
    //Initialize int variable that shows free slots
    int freeCount = 0;

    //Checking to see if enough memory is available for the required size
    for (int i = 0; i < memory.size(); i++) {
        
        if (memory[i] == 0) {
            freeCount++;

            if (freeCount == requiredSize) {
                return i - requiredSize + 1; //Captures correct page start
            }
        }

        else {
            freeCount = 0;
        }
    }

    return -1;
}

//Function for deallocating a job
void freeJob(vector<int>& memory, int jobID) {
    for(int i = 0; i <  memory.size(); i++) {
        if(memory[i] == jobID) {
            memory[i] = 0;
        }
    }

    cout << "Freeing job " << jobID << " complete." << endl;
}

int findBestFit(const vector<int>& memory, int requiredSize) {
    //Initialize int variable that shows free slots
    int freeCount = 0;
    int bestSize{};
    int bestStart{};

    //Checking to see if enough memory is available for the required size
    for (int i = 0; i < memory.size(); i++) {
        
        if (memory[i] == 0) {
            freeCount++;
        }

        else {
            if(freeCount >= requiredSize && (bestSize == 0 || freeCount < bestSize)) {
                bestSize = freeCount;
                bestStart = i - freeCount;
            }

            freeCount = 0;
        }
    }

    if(freeCount >= requiredSize && (bestSize == 0 || freeCount < bestSize)) {
        bestSize = freeCount;
        bestStart = memory.size() - freeCount;
    }

    if(bestSize > 0) {
        return bestStart;
    }
    else {
        return -1;
    } 
}

int main() {

    //Vector for jobs
    vector<Job> jobs;

    //Vector for memory
    vector<int> memory(20, 0);

    //Initializing variables (empty)
    int id{};
    int startTime{};
    int size{};
    int interval{};
    string endState{};

    //Starting text to show program is booted/working
    cout << "Memory simulation starting..." << endl;

    //Setting up the input stream
    ifstream inputFile("jobs.txt");

    //Checking if the file opened correctly
    if (!inputFile) {
        cout << "Could not open jobs.txt" << endl;
        return 1; //Returns an error from not opening
    }

    //Reading the file
    while (inputFile >> id >> startTime >> size >> interval >> endState) {

        //Setting up input collector for each job
        Job job;

        job.id = id;
        job.startTime = startTime;
        job.size = size;
        job.interval = interval;
        job.endState = endState;

        //Adding jobs
        jobs.push_back(job);
    }

    inputFile.close();

    //Printed output
    cout << "Jobs loaded: " << jobs.size() << endl;

    //Looping through jobs and printing variable outputs
    for (Job job : jobs) {
        cout << "Job " << job.id
             << " starts at " << job.startTime
             << ", requires " << job.size
             << " KB, interval " << job.interval
             << ", state " << job.endState
             << endl;


            //First-Fit
        int location = findBestFit(memory, job.size);

        if (location == -1) {
        cout << "Not enough contiguous memory." << endl;
        }
        else {
            for (int i = location; i < location + job.size; i++) {
                memory[i] = job.id;
        }
    }
    }

    printMemory(memory);

    freeJob(memory, 2);

    printMemory(memory);

        //Looping through jobs and printing variable outputs
    for (Job job : jobs) {
        cout << "Job " << job.id
             << " starts at " << job.startTime
             << ", requires " << job.size
             << " KB, interval " << job.interval
             << ", state " << job.endState
             << endl;


            //First-Fit
        int location = findBestFit(memory, job.size);

        if (location == -1) {
        cout << "Not enough contiguous memory." << endl;
        }
        else {
            for (int i = location; i < location + job.size; i++) {
                memory[i] = job.id;
        }
    }

    printMemory(memory);
    
    }

    return 0;
}
