#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

//Represents a file fragment/block on the disk
struct DiskBlock {
    int blockNumber;
    int sizeInKB;
};

//Represents a process/job in memory
struct Process {
    int id;
    int startTime;
    int requiredSize; //size in KB
    int executionInterval;
    string state; //sleep, active, terminated, etc.
};

//Represents a page in physical memory
struct MemoryPage {
    int pageID;
    int processID; //-1 if free
};

class MemoryManager {
    private:
        int totalMemorySize; //size in KB
        int pageSize; //size in KB
        int totalPages;
        vector<MemoryPage> physicalMemory;
        vector<Process> processTable;

    public:
        MemoryManager(int memSize, int pSize) : totalMemorySize(memSize), pageSize(pSize) {
            totalPages = totalMemorySize / pageSize;
            for(int i = 0; i < totalPages; ++i) {
                physicalMemory.push_back({ i, -1}); //All pages are initially free
            }
        }

    //Initializes memory with the initial tables of processes
    void initializeProcesses(const vector<Process>& processes) {
        processTable = processes;
        //Allocates pages for the existing processes sequentially
        int currentPage = 0;
        for (const auto& proc : processTable) {
            int pagesNeeded = (proc.requiredSize + pageSize - 1) / pageSize;
            for(int p = 0; p < pagesNeeded && currentPage < totalPages; ++p) {
                physicalMemory[currentPage].processID = proc.id;
                currentPage++;
            }
        }
    }

    //Collects disk block info and calculates total file size
    int calculateFileSize(const vector<DiskBlock>& diskBlocks) {
        cout << "\n--- Collecting Disk Fragments & Calculating Required Memory ---" << endl;
        int totalSize = 0;
        cout << "Disk blocks assigned to file: ";
        for(size_t i = 0; i < diskBlocks.size(); ++i) {
            cout << diskBlocks[i].blockNumber;
            if(i < diskBlocks.size() - 1) {
                cout << " -> ";
                totalSize += diskBlocks[i].sizeInKB;
            }
        }
        cout << "\nTotal Block Count: " << diskBlocks.size() << endl;
        cout << "Calculated Total File Size to load into memory: " << totalSize << "KB" << endl;
        return totalSize;
    }

    //Counts currently free pages
    int getFreeMemory() {
        int freeCount = 0;
        for (const auto& page : physicalMemory) {
            if(page.processID == -1) {
                freeCount++;
            }
        }
        return freeCount * pageSize;
    }

    //Liberation/replacement strategy if memory is insufficient
    void liberateSpace(int requiredKB) {
        cout << "\n[Memory Manager] Not enough space! Liberating memory..." << endl;

        //Removes sleeping/inactive processes to free up space
        for(auto& proc : processTable) {
            if(getFreeMemory() >= requiredKB) {
                break;
            }
            if(proc.state == "Sleep" && proc.requiredSize > 0) {
                cout << "-> Liberating Process ID " << proc.id << " (Freed " << proc.requiredSize << " KB)" << endl;
            }

            //Clearing from physical memory
            for(auto& page : physicalMemory) {
                if(page.processID == proc.id) {
                    page.processID = -1;
                }
            }
            proc.state = "Evicted";
        }
    }

    //Step 3: Allocate Space in Memory
    bool allocateFileMemory(int fileID, int fileSizeKB) {
        cout << "\n--- Memory Allocation ---" << endl;
        int pagesNeeded = (fileSizeKB + pageSize - 1) / pageSize;

        if(getFreeMemory() < fileSizeKB) {
            cout << "Error: Failed to allocate memory even after liberation." << endl;
            return false;
        }

        //Allocate Pages
        int allocatedPages = 0;
        for(auto& page : physicalMemory) {
            if(page.processID == -1) {
                page.processID = fileID;
                allocatedPages++;
                if(allocatedPages == pagesNeeded) {
                    break;
                }
            }
        }

        cout << "Sucess: File (ID: " << fileID << ") allocated "
             << pagesNeeded << " pages (" << fileSizeKB << " KB) successfully." << endl;

             return true;
    }

    //Displays current state of physical memory
    void displayMemoryState() {
        cout << "\nCurrent Physical Memory Layout:" << endl;
        cout << "-----------------------------------" << endl;
        cout << "Page ID | Status/Process ID" << endl;
        cout << "-----------------------------------" << endl;
        
        for(const auto& page : physicalMemory) {
            cout << setw(7) << page.pageID << " | ";
            if(page.processID == -1) {
                cout << "Free" << endl;
            }
            else if(page.processID >= 999) {
                cout << "File (ID: " << page.processID << ")" << endl;
            }
            else {
                cout << "Job " << page.processID << endl;
            }
        }
        cout << "-----------------------------------" << endl;
    }
};

int main() {
    MemoryManager memManager(20, 1);

    //Initial memory state table
    vector<Process> initialProcesses = {
        {1, 1, 2, 7, "Sleep"},
        {2, 2, 3, 8, "Sleep"},
        {3, 3, 4, 6, "Sleep"},
        {4, 4, 3, 6, "Sleep"},
        {5, 5, 2, 9, "Sleep"},
        {6, 6, 3, 6, "Sleep"},
        {7, 7, 2, 6, "Sleep"}
    };

    memManager.initializeProcesses(initialProcesses);

    cout << "=== Initial Memory State ===" << endl;
    memManager.displayMemoryState();

    vector<int> diskBlockNumbers = {28, 5, 12, 13, 1, 4};
    int diskBlockSizeKB = 1;

    vector<DiskBlock> fileFragments;
    for(int blockNum : diskBlockNumbers) {
        fileFragments.push_back({blockNum, diskBlockSizeKB});
    }

    //Target file parameters
    int targetFileID = 999;
    int targetStartTime = 12;
    int targetEndTime = 16;

    cout << "\n>>> Simulation Time Interval: " << targetStartTime<< " <<<" << endl;
    cout << "Request: Transfer file into memory from interval "
         << targetStartTime << " to " << targetEndTime << endl;

    int requiredMemoryKB = 8;
    memManager.calculateFileSize(fileFragments);

    memManager.allocateFileMemory(targetFileID, requiredMemoryKB);

    memManager.displayMemoryState();

    return 0;
}
