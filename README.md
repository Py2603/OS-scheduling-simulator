# OS Scheduling Simulator 

This project is a CPU Scheduling Simulator built using C++. It demonstrates how different scheduling algorithms work in an operating system and how they affect process execution.

The simulator supports both preemptive and non-preemptive algorithms and also provides a visual representation using Gantt charts.

---

## 🚀 Implemented Algorithms

- First Come First Serve (FCFS)
- Shortest Job First (SJF)
- Shortest Remaining Time First (SRTF)
- Priority Scheduling
- Round Robin (RR)

---

## ✨ Features

- Takes user input dynamically (Arrival Time, Burst Time, Priority)
- Calculates important metrics:
  - Completion Time (CT)
  - Turnaround Time (TAT)
  - Waiting Time (WT)
- Compares multiple algorithms to determine the most efficient one
- Displays execution using Gantt charts:
  - Simple chart for non-preemptive algorithms
  - Timeline-based chart for preemptive algorithms like SRTF and Round Robin

---

## 📊 Gantt Chart Visualization

- Non-preemptive algorithms are shown using continuous execution blocks
- Preemptive algorithms are visualized using a timeline approach, which accurately shows context switching

---

## 🛠️ How to Run

```bash
g++ src/main.cpp -I src/include -o main
./main
