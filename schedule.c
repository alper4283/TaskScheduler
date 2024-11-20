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
    int originalIndex;
} Task;

void printTaskIntervals(Task *tasks, int taskCount) {
    // Create an array of pointers to tasks for sorting
    Task *sortedTasks[taskCount];
    for (int i = 0; i < taskCount; i++) {
        sortedTasks[i] = &tasks[i];
    }

    // Sort tasks by their original index
    for (int i = 0; i < taskCount - 1; i++) {
        for (int j = i + 1; j < taskCount; j++) {
            if (sortedTasks[i]->originalIndex > sortedTasks[j]->originalIndex) {
                Task *temp = sortedTasks[i];
                sortedTasks[i] = sortedTasks[j];
                sortedTasks[j] = temp;
            }
        }
    }

    // Print intervals for tasks in original order
    for (int i = 0; i < taskCount; i++) {
        Task *task = sortedTasks[i];
        printf("%s -> [", task->name);
        if (task->startTime && task->endTime) {
            int start = task->startTime[0];
            int end = task->endTime[0];
            for (int j = 1; task->startTime[j] != -1; j++) {
                if (task->startTime[j] == end) {
                    // Extend the current interval
                    end = task->endTime[j];
                } else {
                    // Print the current interval
                    printf("%d-%d, ", start, end);
                    // Start a new interval
                    start = task->startTime[j];
                    end = task->endTime[j];
                }
            }
            // Print the last interval
            printf("%d-%d", start, end);
        }
        printf("]\n");
    }
}


int compareTasks(const void *a, const void *b) {
    Task *taskA = (Task *)a;
    Task *taskB = (Task *)b;
    return taskA->cpuBurst - taskB->cpuBurst;
}

int compareByPriority(const void *a, const void *b) {
    Task *taskA = (Task *)a;
    Task *taskB = (Task *)b;
    return taskA->priority - taskB->priority; // Ascending order of priority
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
    const int quantum = 5; // Fixed quantum time
    int currentTime = 0;   // Tracks the current time in the simulation

    // Allocate memory for start and end times for each task
    for (int i = 0; i < taskCount; i++) {
        tasks[i].startTime = malloc(taskCount * sizeof(int)); // Maximum intervals = taskCount
        tasks[i].endTime = malloc(taskCount * sizeof(int));
        tasks[i].startTime[0] = -1; // Sentinel value for end of intervals
        tasks[i].endTime[0] = -1;   // Sentinel value for end of intervals
    }

    // Initialize a queue for Round Robin scheduling
    int *queue = malloc(taskCount * sizeof(int));
    int front = 0, rear = 0;

    // Enqueue all tasks initially
    for (int i = 0; i < taskCount; i++) {
        queue[rear++] = i; // Add task index to the queue
    }

    // Process tasks in the queue
    while (front < rear) {
        int taskIndex = queue[front++]; // Dequeue the first task
        int remainingTime = tasks[taskIndex].remainingBurst;

        // Update start time for the interval
        int intervalIndex = 0;
        while (tasks[taskIndex].startTime[intervalIndex] != -1) {
            intervalIndex++;
        }
        tasks[taskIndex].startTime[intervalIndex] = currentTime;

        if (remainingTime > quantum) {
            // Task gets a quantum slice
            currentTime += quantum;
            tasks[taskIndex].remainingBurst -= quantum;

            // Update end time for the interval
            tasks[taskIndex].endTime[intervalIndex] = currentTime;

            // Re-enqueue the task
            queue[rear++] = taskIndex;
        } else {
            // Task finishes within this quantum
            currentTime += remainingTime;
            tasks[taskIndex].remainingBurst = 0;

            // Update end time for the interval
            tasks[taskIndex].endTime[intervalIndex] = currentTime;
        }

        // Mark the end of intervals
        tasks[taskIndex].startTime[intervalIndex + 1] = -1;
        tasks[taskIndex].endTime[intervalIndex + 1] = -1;
    }

    // Print task intervals
    printTaskIntervals(tasks, taskCount);

    // Free allocated memory
    for (int i = 0; i < taskCount; i++) {
        free(tasks[i].startTime);
        free(tasks[i].endTime);
    }
    free(queue);
}

void schedule_RRP(Task *tasks, int taskCount) {
    for (int i = 0; i < taskCount; i++) {
    tasks[i].startTime = malloc(taskCount * sizeof(int)); // Max intervals = taskCount
    tasks[i].endTime = malloc(taskCount * sizeof(int));

    // Initialize all elements to -1
    for (int j = 0; j < taskCount; j++) {
        tasks[i].startTime[j] = -1;
        tasks[i].endTime[j] = -1;
    }
}
    const int quantum = 5; // Fixed quantum time
    int currentTime = 0;   // Tracks the current time in the simulation

    // Step 1: Sort tasks by priority
    qsort(tasks, taskCount, sizeof(Task), compareByPriority);


    // Step 2: Execute Round Robin within each priority group
    for (int i = 0; i < taskCount; ) {
        int priority = tasks[i].priority;
        int groupEnd = i;

        // Find the end of the current priority group
        while (groupEnd < taskCount && tasks[groupEnd].priority == priority) {
            groupEnd++;
        }

        // Perform Round Robin on this priority group
        int *queue = malloc((groupEnd - i) * sizeof(int));
        int front = 0, rear = 0;

        // Enqueue all tasks in this priority group
        for (int j = i; j < groupEnd; j++) {
            queue[rear++] = j; // Add task index to the queue
        }

        while (front < rear) {
            int taskIndex = queue[front++]; // Dequeue the first task
            int remainingTime = tasks[taskIndex].remainingBurst;

            // Find the next available interval index
            int intervalIndex = 0;
            while (tasks[taskIndex].startTime[intervalIndex] != -1) {
                intervalIndex++;
            }
            tasks[taskIndex].startTime[intervalIndex] = currentTime;

            if (remainingTime > quantum) {
                // Task gets a quantum slice
                currentTime += quantum;
                tasks[taskIndex].remainingBurst -= quantum;

                // Update end time for the interval
                tasks[taskIndex].endTime[intervalIndex] = currentTime;

                // Re-enqueue the task
                queue[rear++] = taskIndex;
            } else {
                // Task finishes within this quantum
                currentTime += remainingTime;
                tasks[taskIndex].remainingBurst = 0;

                // Update end time for the interval
                tasks[taskIndex].endTime[intervalIndex] = currentTime;
            }

            // Mark the end of intervals
            tasks[taskIndex].startTime[intervalIndex + 1] = -1;
            tasks[taskIndex].endTime[intervalIndex + 1] = -1;
        }

        free(queue); // Free memory for the queue

        // Move to the next priority group
        i = groupEnd;
    }

    // Step 3: Print task intervals
    printTaskIntervals(tasks, taskCount);

    // Free memory for start and end times
    for (int i = 0; i < taskCount; i++) {
        free(tasks[i].startTime);
        free(tasks[i].endTime);
    }
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
        tasks[i].originalIndex = i;
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