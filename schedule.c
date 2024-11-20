#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Task structure
typedef struct {
    char *name;
    int cpuBurst;
    int remainingBurst;
    int *startTime;
    int *endTime;
    int priority;
} Task;

void printTaskIntervals(Task *tasks, int taskCount) {
    for (int i = 0; i < taskCount; i++) {
        printf("%s -> [", tasks[i].name);
        if (tasks[i].startTime && tasks[i].endTime) {
            for (int j = 0; tasks[i].startTime[j] != -1; j++) { //-1 marks the end of intervals
                printf("%d-%d", tasks[i].startTime[j], tasks[i].endTime[j]);
                if (tasks[i].startTime[j + 1] != -1) {
                    printf(", ");
                }
            }
        }
        printf("]\n");
    }
}

int compareTasks(const void *a, const void *b) {
    Task *taskA = (Task *)a;
    Task *taskB = (Task *)b;
    return taskA->cpuBurst - taskB->cpuBurst;
}

void schedule_SJF(Task *tasks, int taskCount) {
    // Step 1: Sort tasks by cpuBurst
    qsort(tasks, taskCount, sizeof(Task), compareTasks);

    // Step 2: Simulate task execution
    int currentTime = 0;
    for (int i = 0; i < taskCount; i++) {
        // Allocate startTime and endTime arrays
        tasks[i].startTime = malloc(sizeof(int));
        tasks[i].endTime = malloc(sizeof(int));

        // Assign start and end times
        tasks[i].startTime[0] = currentTime;
        currentTime += tasks[i].cpuBurst;
        tasks[i].endTime[0] = currentTime;

        // Mark end of intervals
        tasks[i].startTime[1] = -1; // Sentinel value
        tasks[i].endTime[1] = -1;  // Sentinel value
    }

    // Step 3: Print task intervals
    printTaskIntervals(tasks, taskCount);

    // Step 4: Free allocated memory for intervals
    for (int i = 0; i < taskCount; i++) {
        free(tasks[i].startTime);
        free(tasks[i].endTime);
    }
}

void schedule_RR(Task *tasks, int taskCount) {
    // Implement Round Robin scheduling algorithm
}

void schedule_RRP(Task *tasks, int taskCount) {
    // Implement Round Robin with Priority scheduling algorithm
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <scheduling_algorithm>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    const char *algorithm = argv[2];

    // Open the file
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    // Count lines to determine the number of tasks
    int taskCount = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        taskCount++;
    }
    rewind(file); // Reset file pointer to the beginning

    // Allocate memory for tasks
    Task *tasks = malloc(taskCount * sizeof(Task));
    if (!tasks) {
        perror("Failed to allocate memory for tasks");
        fclose(file);
        return 1;
    }

    // Read tasks from file
    int i = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        tasks[i].name = malloc(20 * sizeof(char)); // Assuming task names are up to 20 chars
        sscanf(buffer, "%s %d %d", tasks[i].name, &tasks[i].priority, &tasks[i].cpuBurst);
        tasks[i].remainingBurst = tasks[i].cpuBurst;
        tasks[i].startTime = NULL; // Will be allocated later by scheduling algorithm
        tasks[i].endTime = NULL;   // Will be allocated later by scheduling algorithm
        i++;
    }

    fclose(file);

    // Call the appropriate scheduling function
    if (strcmp(algorithm, "SJF") == 0) {
        schedule_SJF(tasks, taskCount);
    } else if (strcmp(algorithm, "RR") == 0) {
        schedule_RR(tasks, taskCount);
    } else if (strcmp(algorithm, "RRP") == 0) {
        schedule_RRP(tasks, taskCount);
    } else {
        fprintf(stderr, "Unknown scheduling algorithm: %s\n", algorithm);
        free(tasks);
        return 1;
    }

    // Free allocated memory for tasks
    for (i = 0; i < taskCount; i++) {
        free(tasks[i].name);
    }
    free(tasks);

    return 0;
}