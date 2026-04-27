#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include "process.h"  // Include the Process class
#include <vector>     // For storing processes in a list
#include <queue>
#include <climits>
using namespace std;

class Scheduler 
{
public:
    vector<Process> ready_queue;  // List of processes waiting to run
    vector<int> timeline;
    // Add a process to the ready queue
    void addProcess(Process p)
    {
        ready_queue.push_back(p);  // Add the process to the end of the queue
    }

    void calculateAverages() 
    {
        int total_turnaround = 0;
        int total_waiting = 0;

        for (const auto& p : ready_queue) {
            total_turnaround += p.turnaround_time;
            total_waiting += p.waiting_time;
        }

        double avg_turnaround = static_cast<double>(total_turnaround) / ready_queue.size();
        double avg_waiting = static_cast<double>(total_waiting) / ready_queue.size();

        cout << "Average Turnaround Time: " << avg_turnaround << endl;
        cout << "Average Waiting Time: " << avg_waiting << endl;
    }


    // Run the First-Come-First-Serve (FCFS) algorithm
    void runFCFS() 
    {
        int current_time = 0;
    
        for (auto& p : ready_queue) 
        {
            // Wait until the process arrives
            if (current_time < p.arrival_time)
            {
                current_time = p.arrival_time;
            }
    
            p.start_time = current_time;
            p.state = "Running";
            cout << "Process " << p.PID << " started at time " << current_time << "!" << endl;
    
            // Simulate execution
            while (p.remaining_time > 0)
            {
                cout << "Process " << p.PID << " is running (" << p.remaining_time << "s left)" << endl;
                p.remaining_time--;
                current_time++;
            }
    
            p.completion_time = current_time;
            p.state = "Terminated";
            p.calculateMetrics(); // Calculate turnaround and waiting time
            cout << "Process " << p.PID << " finished at time " << current_time << "!" << endl << endl;
        }
    }
  void runSJF() 
{
    int current_time = 0;
    vector<bool> completed(ready_queue.size(), false);
    int done = 0;

    while (done < ready_queue.size()) 
    {
        int idx = -1;
        int min_burst = INT_MAX;

        for (int i = 0; i < ready_queue.size(); i++) 
        {
            if (!completed[i] && 
                ready_queue[i].arrival_time <= current_time &&
                ready_queue[i].burst_time < min_burst) 
            {
                min_burst = ready_queue[i].burst_time;
                idx = i;
            }
        }

        if (idx == -1) 
        {
            current_time++;
            continue;
        }

        auto &p = ready_queue[idx];

        p.start_time = current_time;
        current_time += p.burst_time;

        p.completion_time = current_time;
        p.calculateMetrics();

        completed[idx] = true;
        done++;
    }
} 


void runSRTF()
{
    timeline.clear();   // ✅ reset timeline

    int current_time = 0;
    int completed = 0;
    int n = ready_queue.size();

    while (completed < n)
    {
        int idx = -1;
        int min_rem = INT_MAX;

        // find process with shortest remaining time
        for (int i = 0; i < n; i++)
        {
            if (ready_queue[i].arrival_time <= current_time &&
                ready_queue[i].remaining_time > 0 &&
                ready_queue[i].remaining_time < min_rem)
            {
                min_rem = ready_queue[i].remaining_time;
                idx = i;
            }
        }

        // CPU idle
        if (idx == -1)
        {
            timeline.push_back(-1);   // ✅ IMPORTANT (idle slot)
            current_time++;
            continue;
        }

        Process &p = ready_queue[idx];

        // set start time once
        if (p.start_time == -1)
            p.start_time = current_time;

        timeline.push_back(p.PID);   // 🔥 MOST IMPORTANT LINE

        // execute for 1 unit
        p.remaining_time--;
        current_time++;

        // if finished
        if (p.remaining_time == 0)
        {
            p.completion_time = current_time;
            p.calculateMetrics();
            completed++;
        }
    }
}

void runPriority()
{
    int current_time = 0;
    int completed = 0;
    int n = ready_queue.size();
    vector<bool> done(n, false);

    while (completed < n)
    {
        int idx = -1;
        int best_priority = INT_MAX;

        for (int i = 0; i < n; i++)
        {
            if (!done[i] &&
                ready_queue[i].arrival_time <= current_time &&
                ready_queue[i].priority < best_priority)
            {
                best_priority = ready_queue[i].priority;
                idx = i;
            }
        }

        if (idx == -1)
        {
            current_time++;
            continue;
        }

        auto &p = ready_queue[idx];

        p.start_time = current_time;
        current_time += p.burst_time;

        p.completion_time = current_time;
        p.calculateMetrics();

        done[idx] = true;
        completed++;
    }
}


void runRoundRobin(int quantum)
{
    timeline.clear();   // ✅ reset

    queue<int> q;
    int n = ready_queue.size();
    vector<bool> in_queue(n, false);

    int current_time = 0;
    int completed = 0;

    while (completed < n)
    {
        // add arrived processes
        for (int i = 0; i < n; i++)
        {
            if (!in_queue[i] && ready_queue[i].arrival_time <= current_time)
            {
                q.push(i);
                in_queue[i] = true;
            }
        }

        if (q.empty())
        {
            timeline.push_back(-1); // idle
            current_time++;
            continue;
        }

        int idx = q.front(); q.pop();
        Process &p = ready_queue[idx];

        if (p.start_time == -1)
            p.start_time = current_time;

        int run_time = min(quantum, p.remaining_time);

        for (int t = 0; t < run_time; t++)
        {
            timeline.push_back(p.PID);  // ✅ RECORD EACH TIME UNIT
            current_time++;
            p.remaining_time--;

            // add newly arrived processes
            for (int i = 0; i < n; i++)
            {
                if (!in_queue[i] && ready_queue[i].arrival_time <= current_time)
                {
                    q.push(i);
                    in_queue[i] = true;
                }
            }

            if (p.remaining_time == 0) break;
        }

        if (p.remaining_time == 0)
        {
            p.completion_time = current_time;
            p.calculateMetrics();
            completed++;
        }
        else
        {
            q.push(idx);
        }
    }
}

void printTable()
{
    cout << "\nPID\tAT\tBT\tCT\tTAT\tWT\n";
    for (auto &p : ready_queue)
    {
        cout << p.PID << "\t"
             << p.arrival_time << "\t"
             << p.burst_time << "\t"
             << p.completion_time << "\t"
             << p.turnaround_time << "\t"
             << p.waiting_time << endl;
    }
}

};

#endif