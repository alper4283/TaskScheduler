#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Task structure
typedef struct {
    char *name;
    int cpuBurst;
    int startTime;
    int endTime;
} Task;

void schedule_SJF(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char taskName[100];
    int priority, cpuBurst;
    int taskCount = 0;

    // First pass to count tasks
    while (fscanf(file, "%s %d %d", taskName, &priority, &cpuBurst) == 3) {
        taskCount++;
    }

    // Allocate arrays for tasks
    Task *originalTasks = malloc(taskCount * sizeof(Task));
    Task *sortedTasks = malloc(taskCount * sizeof(Task));

    // Reset file pointer to beginning
    fseek(file, 0, SEEK_SET);

    // Second pass to read tasks into arrays
    int index = 0;
    while (fscanf(file, "%s %d %d", taskName, &priority, &cpuBurst) == 3) {
        originalTasks[index].name = strdup(taskName);
        originalTasks[index].cpuBurst = cpuBurst;
        originalTasks[index].startTime = -1; 
        originalTasks[index].endTime = -1;  

        // Copy data to sorted array
        sortedTasks[index] = originalTasks[index];
        index++;
    }

    fclose(file);

    // Sort the `sortedTasks` array by CPU burst time (SJF)
    for (int i = 0; i < taskCount - 1; i++) {
        for (int j = i + 1; j < taskCount; j++) {
            if (sortedTasks[i].cpuBurst > sortedTasks[j].cpuBurst) {
                Task temp = sortedTasks[i];
                sortedTasks[i] = sortedTasks[j];
                sortedTasks[j] = temp;
            }
        }
    }

    int currentTime = 0;
    for (int i = 0; i < taskCount; i++) {
        sortedTasks[i].startTime = currentTime;
        currentTime += sortedTasks[i].cpuBurst;
        sortedTasks[i].endTime = currentTime;
    }

    for (int i = 0; i < taskCount; i++) {
        for (int j = 0; j < taskCount; j++) {
            if (strcmp(originalTasks[i].name, sortedTasks[j].name) == 0) {
                originalTasks[i].startTime = sortedTasks[j].startTime;
                originalTasks[i].endTime = sortedTasks[j].endTime;
                break;
            }
        }
    }
    // Print tasks
    for (int i = 0; i < taskCount; i++) {
        printf("%s -> [%d, %d]\n", originalTasks[i].name, originalTasks[i].startTime, originalTasks[i].endTime);
    }

    // Free allocated memory
    for (int i = 0; i < taskCount; i++) {
        free(originalTasks[i].name);
    }
    free(originalTasks);
    free(sortedTasks);
}


void schedule_RR(const char *filename) {
    // Implement Round Robin scheduling algorithm
    printf("Scheduling using RR from file: %s\n", filename);
}

void schedule_RRP(const char *filename) {
    // Implement Round Robin with Priority scheduling algorithm
    printf("Scheduling using RRP from file: %s\n", filename);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <scheduling_algorithm>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    const char *algorithm = argv[2];

    if (strcmp(algorithm, "SJF") == 0) {
        schedule_SJF(filename);
    } else if (strcmp(algorithm, "RR") == 0) {
        schedule_RR(filename);
    } else if (strcmp(algorithm, "RRP") == 0) {
        schedule_RRP(filename);
    }
     else {
        fprintf(stderr, "Unknown scheduling algorithm: %s\n", algorithm);
        return 1;
    }

    return 0;
}