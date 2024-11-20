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
                    printf("%d-%d, ", start, end);
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
    //Sort tasks by cpuBurst
    qsort(tasks, taskCount, sizeof(Task), compareTasks);

    int currentTime = 0;
    for (int i = 0; i < taskCount; i++) {
        tasks[i].startTime = malloc(sizeof(int));
        tasks[i].endTime = malloc(sizeof(int));

        tasks[i].startTime[0] = currentTime;
        currentTime += tasks[i].cpuBurst;
        tasks[i].endTime[0] = currentTime;

        tasks[i].startTime[1] = -1; 
        tasks[i].endTime[1] = -1; 
    }

    printTaskIntervals(tasks, taskCount);
}

void schedule_RR(Task *tasks, int taskCount) {
    const int quantum = 5; 
    int currentTime = 0;   

    for (int i = 0; i < taskCount; i++) {
        tasks[i].startTime = malloc(taskCount * sizeof(int)); 
        tasks[i].endTime = malloc(taskCount * sizeof(int));
        tasks[i].startTime[0] = -1; 
        tasks[i].endTime[0] = -1;   
    }

    // Initialize a queue for Round Robin scheduling
    int *queue = malloc(taskCount * sizeof(int));
    int front = 0, rear = 0;

    // Enqueue all tasks initially
    for (int i = 0; i < taskCount; i++) {
        queue[rear++] = i; 
    }

    // Process tasks in the queue
    while (front < rear) {
        int taskIndex = queue[front++]; 
        int remainingTime = tasks[taskIndex].remainingBurst;

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

            tasks[taskIndex].endTime[intervalIndex] = currentTime;
        }

        tasks[taskIndex].startTime[intervalIndex + 1] = -1;
        tasks[taskIndex].endTime[intervalIndex + 1] = -1;
    }

    printTaskIntervals(tasks, taskCount);
    free(queue);
}

void schedule_RRP(Task *tasks, int taskCount) {
    for (int i = 0; i < taskCount; i++) {
    tasks[i].startTime = malloc(taskCount * sizeof(int)); 
    tasks[i].endTime = malloc(taskCount * sizeof(int));

    for (int j = 0; j < taskCount; j++) {
        tasks[i].startTime[j] = -1;
        tasks[i].endTime[j] = -1;
    }
}
    const int quantum = 5; 
    int currentTime = 0;   

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
            queue[rear++] = j; 
        }

        while (front < rear) {
            int taskIndex = queue[front++]; 
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

                tasks[taskIndex].endTime[intervalIndex] = currentTime;
            }

            tasks[taskIndex].startTime[intervalIndex + 1] = -1;
            tasks[taskIndex].endTime[intervalIndex + 1] = -1;
        }

        free(queue); 

        i = groupEnd;
    }

    printTaskIntervals(tasks, taskCount);
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
    rewind(file); 

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
        tasks[i].startTime = NULL; 
        tasks[i].endTime = NULL;   
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

    // Calculate WT and RT
    double totalWT = 0, totalRT = 0;
    for (i = 0; i < taskCount; i++) {
        totalRT += tasks[i].startTime[0];

        int lastEndTime = tasks[i].endTime[0];
        int j = 1;
        while (tasks[i].endTime[j] != -1) {
            lastEndTime = tasks[i].endTime[j];
            j++;
        }
        totalWT += (lastEndTime - tasks[i].cpuBurst);
    }

    // Print WT and RT averages
    printf("--- Stats ---\n");
    printf("WT : %.2f\n", totalWT / taskCount);
    printf("RT : %.2f\n", totalRT / taskCount);

    // Free allocated memory for tasks
    for (i = 0; i < taskCount; i++) {
        free(tasks[i].name);
        free(tasks[i].startTime);
        free(tasks[i].endTime);
    }
    free(tasks);

    return 0;
}