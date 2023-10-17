#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <limits.h>
#define PS_MAX 10

// holds the scheduling data of one process
typedef struct{
    int idx; // process idx (index)
    int at, bt, rt, wt, ct, tat; // arrival time, burst time, response time, waiting time, completion time, turnaround time.
    int burst; // remaining burst (this should decrement when the process is being executed)
} ProcessData;

// the idx of the running process
int running_process = -1; // -1 means no running processes

// the total time of the timer
unsigned total_time; // should increment one second at a time by the scheduler

// data of the processes
ProcessData data[PS_MAX]; // array of process data
int completed[PS_MAX];

// array of all process pids
pid_t ps[PS_MAX]; // zero valued pids - means the process is terminated or not created yet

// size of data array
unsigned data_size;

void read_file(FILE* file) {
    data_size = 0;
    char line[256];
    fgets(line, sizeof(line), file);

    // reads a data from the file and stores it in the array.
    while (fscanf(file, "%d %d %d", &data[data_size].idx, &data[data_size].at, &data[data_size].bt) == 3) {
        data[data_size].rt = data[data_size].bt;
        data[data_size].wt = 0;
        data[data_size].ct = 0;
        data[data_size].tat = 0;
        data[data_size].burst = data[data_size].bt;

        // number of processes already read
        data_size++;

        if (data_size >= PS_MAX) {
            printf("Exceeded the maximum limit of processes (PS_MAX).\n");
            break;
        }
    }
}


// send signal SIGCONT to the worker process
void resume(pid_t process) {
    // (1.not created yet or 2.terminated)
    if (process == -1 || ps[process] == 0) {
        return;
    }
    kill(ps[process], SIGCONT);
}

// send signal SIGTSTP to the worker process
void suspend(pid_t process) {
    // (1.not created yet or 2.terminated)
    if (process == -1 || ps[process] == 0) {
        return;
    }
    kill(ps[process], SIGTSTP);

}

// send signal SIGTERM to the worker process
void terminate(pid_t process) {
    // (1.not created yet or 2.terminated)
    if (process == -1 || ps[process] == 0) {
        return;
    }
    kill(ps[process], SIGTERM);
    waitpid(ps[process], NULL, 0);

    // calculates metrics for the terminated process
    data[process].ct = total_time;
    data[process].tat = data[process].ct - data[process].at;
    data[process].wt = data[process].tat - data[process].bt;
    printf("Process %d terminated. ct=%d tat=%d wt=%d\n", process, data[process].ct, data[process].tat, data[process].wt);

    ps[process] = 0;
}

// create a process using fork
void create_process(int new_process) {

    // stops the running process
    if (running_process != -1) {
        suspend(running_process);
        printf("Scheduler: Stopping Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
        running_process = -1;
    }

    // forks a new process and adds it to ps array
    pid_t pid = fork();

    // now the idx of the running process is new_process
    running_process = new_process;

    // the scheduler process runs the program "./worker {new_process}"
    // using one of the exec functions like execvp
    if (pid == 0) {
        char process_idx[10];
        sprintf(process_idx, "%d", new_process);
        char *args[] = {"./worker", process_idx, NULL};
        execvp(args[0], args);

        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else {
            ps[new_process] = pid;
            running_process = new_process;
        }
    }

}

// quantum time (specified by the user from command-line argument)
int quantum = 1;

// Function to find the next process to run (Round Robin)
ProcessData find_next_process() {
    int location = 0;
    // Check if the current running process has consumed its quantum
    if (running_process != -1 && data[running_process].burst == 0) {
        terminate(running_process);
        running_process = -1;
        completed[running_process] = 1;
    }

    // Look for the next available process
    for (int i = 0; i < data_size; i++) {
        if (!completed[i] && data[i].at <= total_time && data[i].burst > 0) {
            location = i;
            return data[i];
        }
    }

    // No process has arrived yet or needs to run, increment time and try again
    total_time++;
    printf("Scheduler: Runtime: %u seconds.\nProcess %d: has not arrived yet.\n", total_time, location);
    return find_next_process();
}


// reports the metrics and simulation results
void report(){
    printf("Simulation results.....\n");
    int sum_wt = 0;
    int sum_tat = 0;
    for (int i=0; i< data_size; i++){
        printf("process %d: \n", i);
        printf("  at=%d\n", data[i].at);
        printf("  bt=%d\n", data[i].bt);
        printf("  ct=%d\n", data[i].ct);
        printf("  wt=%d\n", data[i].wt);
        printf("  tat=%d\n", data[i].tat);
        printf("  rt=%d\n", data[i].rt);
        sum_wt += data[i].wt;
        sum_tat += data[i].tat;
    }

    printf("data size = %d\n", data_size);
    float avg_wt = (float)sum_wt/data_size;
    float avg_tat = (float)sum_tat/data_size;
    printf("Average results for this run:\n");
    printf("  avg_wt=%f\n", avg_wt);
    printf("  avg_tat=%f\n", avg_tat);
}

void check_burst(){

    for(int i = 0; i < data_size; i++)
        if (data[i].burst > 0)
            return;

    // report simulation results
    report();

    // terminate the scheduler :)
    exit(EXIT_SUCCESS);
}


// This function is called every one second as handler for SIGALRM signal
void schedule_handler(int signum) {
    // Increment the total time
    total_time++;

    // 1. If there is a worker process running, decrement its remaining burst.
    if (running_process != -1) {
        if (data[running_process].burst > 0) {
            data[running_process].burst--;
        }

        printf("Scheduler: Runtime: %u seconds.\nScheduler: Process %d is running with %d seconds left\n", total_time, running_process, data[running_process].burst);

        // 1.A. If the worker process finished its burst time, then the scheduler should terminate
        //    the running process and prints the message:
        //    "Scheduler: Terminating Process {running_process} (Remaining Time: {data[running_process].burst})"
        if (data[running_process].burst == 0) {
            printf("Scheduler: Terminating Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);

            terminate(running_process);
            running_process = -1;
            completed[running_process] = 1;
        }
    }

    // 2. After that, we need to find the next process to run {next_process}.
    check_burst();
    ProcessData next_process = find_next_process();

    // 3. If {next_process} is not running, then we need to check the current process
    if (running_process != next_process.idx) {
        create_process(next_process.idx);
        printf("Scheduler: Starting Process %d (Remaining Time: %d)\n", next_process.idx, data[next_process.idx].burst);
        data[next_process.idx].rt = total_time - data[next_process.idx].at;

        if (data[next_process.idx].rt == data[next_process.idx].bt) {
            printf("Scheduler: Resuming Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
            resume(running_process);
        }
    }

    check_burst();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <data_file> <quantum>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    total_time = 0;
    quantum = atoi(argv[2]); // Set quantum from the command-line argument

    // read the data file
    FILE *in_file = fopen(argv[1], "r");
    if (in_file == NULL) {
        printf("File is not found or cannot open it!\n");
        exit(EXIT_FAILURE);
    } else {
        read_file(in_file);
    }

    // Set a timer
    struct itimerval timer;

    // The timer goes off after quantum seconds
    timer.it_value.tv_sec = quantum;
    timer.it_value.tv_usec = 0;

    // Timer increments quantum seconds at a time
    timer.it_interval.tv_sec = quantum;
    timer.it_interval.tv_usec = 0;

    // This counts down and sends SIGALRM to the scheduler process after expiration.
    setitimer(ITIMER_REAL, &timer, NULL);

    // Register the handler for SIGALRM signal
    signal(SIGALRM, schedule_handler);

    // Infinite loop to keep the scheduler running
    while (1);

    return 0;
}
