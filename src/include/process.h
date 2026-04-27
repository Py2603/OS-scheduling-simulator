#ifndef PROCESS_H
#define PROCESS_H

#include <string>    

using namespace std; // it saves us from typing "std::" everywhere remember this
class Process
 {
    public:
        int PID;
        string state;
        int burst_time;
        int arrival_time;
        int remaining_time;
        int completion_time; // Time when the process finishes
        int turnaround_time; // Completion time - Arrival time
        int waiting_time;    // Turnaround time - Burst time
        int start_time;      // Time when the process starts running
        int priority;        // For priority scheduling
    
        
     Process(int pid, int burst, int arrival = 0, int pr = 0)
    {
    PID = pid;
    state = "Ready";
    burst_time = burst;
    arrival_time = arrival;
    remaining_time = burst;
    completion_time = 0;
    turnaround_time = 0;
    waiting_time = 0;
    start_time = -1;
    priority = pr;
  }
 
        void calculateMetrics() {
            turnaround_time = completion_time - arrival_time;
            waiting_time = turnaround_time - burst_time;
        }
};
    

#endif
