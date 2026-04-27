#include "include/process.h"
#include "include/scheduler.h"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// ===== Segment struct for timeline compression =====
struct Segment {
    int pid;
    int start;
    int end;
};

// ===== Compress timeline =====
vector<Segment> compressTimeline(const vector<int>& timeline)
{
    vector<Segment> segs;
    if(timeline.empty()) return segs;
    

    int current_pid = timeline[0];
    int start = 0;

    for(int i = 1; i < timeline.size(); i++)
    {
        if(timeline[i] != current_pid)
        {
            segs.push_back({current_pid, start, i});
            current_pid = timeline[i];
            start = i;
        }
    }

    segs.push_back({current_pid, start, (int)timeline.size()});
    return segs;
}

// ===== Pretty Gantt (for SRTF & RR) =====
void printPrettyGantt(const vector<int>& timeline)
{
    auto segs = compressTimeline(timeline);

    cout << "\n=========== GANTT CHART ===========\n\n";

    // Top bar
    cout << " ";
    for(auto &s : segs)
    {
        int len = s.end - s.start;
        for(int i = 0; i < len; i++) cout << "--";
        cout << " ";
    }

    cout << "\n|";

    // Labels
    for(auto &s : segs)
    {
        int len = (s.end - s.start) * 2;
        string label = (s.pid == -1 ? "ID" : "P" + to_string(s.pid));

        int padding = len - label.size();
        int left = padding / 2;
        int right = padding - left;

        for(int i = 0; i < left; i++) cout << " ";
        cout << label;
        for(int i = 0; i < right; i++) cout << " ";

        cout << "|";
    }

    cout << "\n ";

    // Bottom bar
    for(auto &s : segs)
    {
        int len = s.end - s.start;
        for(int i = 0; i < len; i++) cout << "--";
        cout << " ";
    }

    cout << "\n";

    // Time axis
    cout << segs[0].start;
    for(auto &s : segs)
    {
        int len = (s.end - s.start) * 2;
        for(int i = 0; i < len; i++) cout << " ";
        cout << s.end;
    }

    cout << "\n\n";
}

// ===== Classic Gantt (for non-preemptive) =====
void printBetterGantt(const vector<Process>& processes)
{
    if(processes.empty()) return;

    cout << "\n========== GANTT CHART ==========\n\n";

    cout << " ";
    for (auto &p : processes)
    {
        int len = p.completion_time - p.start_time;
        for(int i = 0; i < len; i++) cout << "--";
        cout << " ";
    }

    cout << "\n|";

    for (auto &p : processes)
    {
        int len = p.completion_time - p.start_time;
        int space = (len*2 - 2) / 2;

        for(int i = 0; i < space; i++) cout << " ";
        cout << "P" << p.PID;
        for(int i = 0; i < space; i++) cout << " ";

        cout << "|";
    }

    cout << "\n ";

    for (auto &p : processes)
    {
        int len = p.completion_time - p.start_time;
        for(int i = 0; i < len; i++) cout << "--";
        cout << " ";
    }

    cout << "\n";

    cout << processes[0].start_time;
    for (auto &p : processes)
    {
        int len = p.completion_time - p.start_time;
        for(int i = 0; i < len*2; i++) cout << " ";
        cout << p.completion_time;
    }

    cout << "\n\n";
}

// ===== Avg WT helper =====
double getAvgWT(const vector<Process>& v)
{
    double sum = 0;
    for (auto &p : v) sum += p.waiting_time;
    return sum / v.size();
}

// ===== MAIN =====
int main()
{
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> inputProcesses;

    for(int i = 0; i < n; i++)
    {
        int at, bt;
        cout << "Enter AT and BT for P" << i+1 << ": ";
        cin >> at >> bt;

        inputProcesses.push_back(Process(i+1, bt, at));
    }

    int choice;
    cout << "\nSelect Algorithm:\n";
    cout << "1. FCFS\n2. SJF\n3. SRTF\n4. Priority\n5. Round Robin\n6. Best Algorithm\n";
    cin >> choice;

    // Priority input only when needed
    if(choice == 4)
    {
        for(int i = 0; i < n; i++)
        {
            cout << "Priority for P" << i+1 << ": ";
            cin >> inputProcesses[i].priority;
        }
    }

    Scheduler scheduler;
    for(auto p : inputProcesses) scheduler.addProcess(p);

    // ===== RUN =====
    if(choice == 1)
    {
        scheduler.runFCFS();
        scheduler.calculateAverages();
        scheduler.printTable();
        printBetterGantt(scheduler.ready_queue);
    }
    else if(choice == 2)
    {
        scheduler.runSJF();
        scheduler.calculateAverages();
        scheduler.printTable();
        printBetterGantt(scheduler.ready_queue);
    }
    else if(choice == 3)
    {
        scheduler.runSRTF();
        scheduler.calculateAverages();
        scheduler.printTable();
        printPrettyGantt(scheduler.timeline); // 🔥 real gantt
    }
    else if(choice == 4)
    {
        scheduler.runPriority();
        scheduler.calculateAverages();
        scheduler.printTable();
        printBetterGantt(scheduler.ready_queue);
    }
    else if(choice == 5)
    {
        int q;
        cout << "Enter time quantum: ";
        cin >> q;

        scheduler.runRoundRobin(q);
        scheduler.calculateAverages();
        scheduler.printTable();
        printPrettyGantt(scheduler.timeline); // 🔥 real gantt
    }
    else if(choice == 6)
    {
        vector<Process> original = inputProcesses;

        Scheduler s1, s2, s3, s4, s5;

        s1.ready_queue = original; s1.runFCFS();
        s2.ready_queue = original; s2.runSJF();
        s3.ready_queue = original; s3.runSRTF();

        vector<Process> temp = original;
        for(int i=0;i<n;i++)
        {
            cout<<"Priority for P"<<i+1<<": ";
            cin>>temp[i].priority;
        }
        s4.ready_queue = temp; s4.runPriority();

        int q;
        cout<<"RR quantum: ";
        cin>>q;
        s5.ready_queue = original; s5.runRoundRobin(q);

        double fcfs = getAvgWT(s1.ready_queue);
        double sjf = getAvgWT(s2.ready_queue);
        double srtf = getAvgWT(s3.ready_queue);
        double pr = getAvgWT(s4.ready_queue);
        double rr = getAvgWT(s5.ready_queue);

        cout<<"\nFCFS: "<<fcfs<<"\nSJF: "<<sjf<<"\nSRTF: "<<srtf<<"\nPriority: "<<pr<<"\nRR: "<<rr<<"\n";

        double best = min({fcfs, sjf, srtf, pr, rr});

        cout<<"\nBest:\n";
        if(fcfs==best) cout<<"FCFS\n";
        if(sjf==best) cout<<"SJF\n";
        if(srtf==best) cout<<"SRTF\n";
        if(pr==best) cout<<"Priority\n";
        if(rr==best) cout<<"RR\n";
    }

    return 0;
}
