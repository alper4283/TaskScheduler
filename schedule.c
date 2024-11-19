#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void schedule_SJF(const char *filename) {
    // Implement Shortest Job First scheduling algorithm
    printf("Scheduling using SJF from file: %s\n", filename);
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