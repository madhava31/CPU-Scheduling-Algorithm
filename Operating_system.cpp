#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Structure for a process
typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int completion_time;
} Process;

// Function to sort processes by arrival time
void sortByArrivalTime(Process processes[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// Function to calculate waiting and turnaround time
void calculateTimes(Process processes[], int n) {
    int current_time = 0;
    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }
        processes[i].waiting_time = current_time - processes[i].arrival_time;
        current_time += processes[i].burst_time;
        processes[i].completion_time = current_time;
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
    }
}

// Function to display results
void displayResults(Process processes[], int n, const char *algorithm) {
    printf("\n--- %s Scheduling Results ---\n", algorithm);
    double avg_waiting_time = 0, avg_turnaround_time = 0;
    printf("PID\tAT\tBT\tWT\tTAT\tCT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time,
               processes[i].burst_time, processes[i].waiting_time, processes[i].turnaround_time,
               processes[i].completion_time);
        avg_waiting_time += processes[i].waiting_time;
        avg_turnaround_time += processes[i].turnaround_time;
    }
    printf("Average Waiting Time: %.2f\n", avg_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time / n);
}

// First-Come, First-Served (FCFS) Scheduling
void fcfs_scheduling(Process processes[], int n) {
    Process fcfs_processes[n];
    memcpy(fcfs_processes, processes, sizeof(Process) * n);
    sortByArrivalTime(fcfs_processes, n);
    calculateTimes(fcfs_processes, n);
    displayResults(fcfs_processes, n, "FCFS");
}

// Shortest Job First (SJF) Scheduling - Non-Preemptive
void sjf_scheduling(Process processes[], int n) {
    Process sjf_processes[n];
    memcpy(sjf_processes, processes, sizeof(Process) * n);
    sortByArrivalTime(sjf_processes, n);

    int current_time = 0;
    int completed = 0;
    Process sorted_sjf[n];
    int sorted_index = 0;
    int processed[n];
    memset(processed, 0, sizeof(processed));

    while (completed < n) {
        int shortest_job = -1;
        int min_burst = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (sjf_processes[i].arrival_time <= current_time && !processed[i] && sjf_processes[i].burst_time < min_burst) {
                min_burst = sjf_processes[i].burst_time;
                shortest_job = i;
            }
        }

        if (shortest_job == -1) {
            current_time++;
        } else {
            sjf_processes[shortest_job].waiting_time = current_time - sjf_processes[shortest_job].arrival_time;
            current_time += sjf_processes[shortest_job].burst_time;
            sjf_processes[shortest_job].completion_time = current_time;
            sjf_processes[shortest_job].turnaround_time = sjf_processes[shortest_job].completion_time - sjf_processes[shortest_job].arrival_time;
            sorted_sjf[sorted_index++] = sjf_processes[shortest_job];
            processed[shortest_job] = 1;
            completed++;
        }
    }
    displayResults(sorted_sjf, n, "SJF (Non-Preemptive)");
}

// Priority Scheduling - Non-Preemptive
void priority_scheduling(Process processes[], int n) {
    Process priority_processes[n];
    memcpy(priority_processes, processes, sizeof(Process) * n);
    sortByArrivalTime(priority_processes, n);

    int current_time = 0;
    int completed = 0;
    Process sorted_priority[n];
    int sorted_index = 0;
    int processed[n];
    memset(processed, 0, sizeof(processed));

    while (completed < n) {
        int highest_priority_job = -1;
        int highest_priority = INT_MAX; // Lower number means higher priority

        for (int i = 0; i < n; i++) {
            if (priority_processes[i].arrival_time <= current_time && !processed[i] && priority_processes[i].priority < highest_priority) {
                highest_priority = priority_processes[i].priority;
                highest_priority_job = i;
            }
        }

        if (highest_priority_job == -1) {
            current_time++;
        } else {
            priority_processes[highest_priority_job].waiting_time = current_time - priority_processes[highest_priority_job].arrival_time;
            current_time += priority_processes[highest_priority_job].burst_time;
            priority_processes[highest_priority_job].completion_time = current_time;
            priority_processes[highest_priority_job].turnaround_time = priority_processes[highest_priority_job].completion_time - priority_processes[highest_priority_job].arrival_time;
            sorted_priority[sorted_index++] = priority_processes[highest_priority_job];
            processed[highest_priority_job] = 1;
            completed++;
        }
    }
    displayResults(sorted_priority, n, "Priority (Non-Preemptive)");
}

// Round Robin Scheduling
void round_robin_scheduling(Process processes[], int n, int time_quantum) {
    Process rr_processes[n];
    memcpy(rr_processes, processes, sizeof(Process) * n);
    sortByArrivalTime(rr_processes, n);

    int current_time = 0;
    int completed = 0;
    int remaining_burst[n];
    for (int i = 0; i < n; i++) {
        remaining_burst[i] = rr_processes[i].burst_time;
        rr_processes[i].waiting_time = 0;
    }

    int queue[n];
    int head = 0, tail = 0;
    int visited[n];
    memset(visited, 0, sizeof(visited));

    int i = 0;
    while (completed < n) {
        // Add arriving processes to the queue
        while (i < n && rr_processes[i].arrival_time <= current_time) {
            queue[tail++] = i++;
            visited[i - 1] = 1;
        }

        if (head == tail) {
            current_time++;
            continue;
        }

        int current_process_index = queue[head++];
        int execute_time = (remaining_burst[current_process_index] > time_quantum) ? time_quantum : remaining_burst[current_process_index];
        remaining_burst[current_process_index] -= execute_time;
        current_time += execute_time;

        if (remaining_burst[current_process_index] > 0) {
            queue[tail++] = current_process_index;
        } else {
            completed++;
            rr_processes[current_process_index].completion_time = current_time;
            rr_processes[current_process_index].turnaround_time = rr_processes[current_process_index].completion_time - rr_processes[current_process_index].arrival_time;
            rr_processes[current_process_index].waiting_time = rr_processes[current_process_index].turnaround_time - rr_processes[current_process_index].burst_time;
        }
    }
    displayResults(rr_processes, n, "Round Robin");
}

int main() {
    Process processes[] = {
        {1, 0, 8, 0},
        {2, 1, 4, 1},
        {3, 2, 9, 2},
        {4, 3, 5, 3}
    };
    int n = sizeof(processes) / sizeof(processes[0]);
    int time_quantum = 3;

    fcfs_scheduling(processes, n);
    sjf_scheduling(processes, n);
    priority_scheduling(processes, n);
    round_robin_scheduling(processes, n, time_quantum);

    return 0;
}
