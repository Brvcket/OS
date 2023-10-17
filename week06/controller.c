#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

pid_t agent_pid;

void terminate(){
    kill(agent_pid, SIGUSR2);
    remove("agent.pid");
    exit(0);
}

int main() {
    FILE *file = fopen("agent.pid", "r");
    if (file == NULL) {
        printf("Error: No agent found.");
        exit(0);
    }
    
    fscanf(file, "%d", &agent_pid);
    if (agent_pid == 0){
        printf("Error: No agent found.");
        exit(0);
    }
    
    printf("Agent found.\n");
    fclose(file);
    
    char cmd[256];
    signal(SIGINT,terminate);
    
    while (1) {
        printf("Choose a command {“read”, “exit”, “stop”, “continue”} to send to the agent\n");
        scanf("%s", cmd);
        if (!strcmp(cmd, "read")){
            kill(agent_pid, SIGUSR1);
        } else if (!strcmp(cmd, "exit")){
            terminate();
        } else if (!strcmp(cmd, "stop")){
            kill(agent_pid, SIGSTOP);
        }
        else if (!strcmp(cmd, "continue")){
            kill(agent_pid, SIGCONT);
        }
    }
}
