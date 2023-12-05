#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <stdbool.h>

typedef struct {
    char pname[10];
    int arrival;
    int burst;
    int turnaround;
    int waiting;
    bool completed;
    int cur;
    int start;
    bool started;
} Process;

int difference(const void *p1, const void *p2) {
    return ((Process *)p1)->burst - ((Process *)p2)->burst;
}

int main() {
    FILE *file;
    file = fopen("sjf_input.txt", "r"); // open file
 
    char line[256]; // each line is a testcase
    char lineC[256]; // copy of line for parsing
    int casenum = 1; // test case number

    while (fgets(line, sizeof(line), file) != NULL) { // move line forward to break out of this loop
        strcpy(lineC, line);

        int numProcesses = 0;
        char* token = strtok(lineC, " "); // Split the line by space
        while (token != NULL) {
            token = strtok(NULL, " "); // process name
            token = strtok(NULL, " "); // arrival time
            token = strtok(NULL, " "); // burst time
            numProcesses++;
        }

        Process processes[numProcesses];
        char name[10];
        int arrivalTime, burstTime, numScanned;
        char* ptr = line;
        int i = 0;
        
        while (sscanf(ptr, "%s %d %d%n", name, &arrivalTime, &burstTime, &numScanned) == 3) {
            strcpy(processes[i].pname, name);
            processes[i].arrival = arrivalTime;
            processes[i].burst = burstTime;
            i += 1;
            ptr += numScanned;
        }

        qsort(processes, numProcesses, sizeof(Process), difference); // sort processess array
        printf("Test case #%d: %s\n", casenum++, line);
        printf("Number of Processes: %d\n", numProcesses);
        printf("Process Scheduling Started:\n");

        int cpuTime = 0;
        int count = 0;
        Process *curProcess = NULL;

        while (count < numProcesses) { 
            // try to find a process
            if (curProcess == NULL) {
                for (int i = 0; i < numProcesses; i++) {
                    if (processes[i].arrival <= cpuTime && processes[i].completed != true) {
                        curProcess = &processes[i];
                        break;
                    }
                }
            }
            // cant find a process, increase cpu time and go to next loop
            if (curProcess == NULL) {
                printf("CPU Time %d: None \n", cpuTime);
                cpuTime += 1;
                continue;
            }

            printf("CPU Time %d: ", cpuTime);
            // check if anything arrived
            for (int i = 0; i < numProcesses; i++) {
                if (processes[i].arrival == cpuTime) {
                    printf("[%s Arrived] ", processes[i].pname);
                }
            }
            if (curProcess->started != true) {
                curProcess->start = cpuTime;
                curProcess->started = true;
                curProcess->cur = 0;
            }
            printf("%s [%d/%d] \n", curProcess->pname, curProcess->cur, curProcess->burst);
            curProcess->cur += 1;
            if (curProcess->cur == curProcess->burst) {
                curProcess->completed = true;
                curProcess->waiting = curProcess->start - curProcess->arrival; // start - arrival + burst
                curProcess->turnaround = curProcess->start - curProcess->arrival + curProcess->burst; // 
                count += 1;
                int x = cpuTime + 1;
                printf("CPU Time %d: %s [%d/%d]\n", x, curProcess->pname, curProcess->burst, curProcess->burst);
                printf("Process %s completed with Turnaround Time: %d, Waiting Time: %d\n", curProcess->pname, curProcess->turnaround, curProcess->waiting);
                curProcess = NULL;
                
            }
            
            cpuTime += 1;
        }
        
        float avgTurnaround = 0;
        float avgWaiting = 0;
        for (int i = 0; i < numProcesses; i++) {
            avgTurnaround += processes[i].turnaround;
            avgWaiting += processes[i].waiting;
        }
        avgTurnaround /= numProcesses;
        avgWaiting /= numProcesses;

        printf("Process scheduling completed with Avg Turnaround Time: %.2f, Avg Waiting Time: %.2f\n", avgTurnaround, avgWaiting);
        printf("\n");

    }
    
    fclose(file);
    return 0;
}