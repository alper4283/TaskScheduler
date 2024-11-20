# TaskScheduler

## How to Run

### On Linux:
1. Open the terminal.
2. Run the following command to build the program:
   make
3. Execute the program with:
   ./schedule tasks.txt RR/SJF/RRP
   Replace `RR/SJF/RRP` with the desired scheduling algorithm.

### On Windows:
1. Open a terminal or command prompt.
2. Compile the program with:
   gcc schedule.c -o schedule
3. Run the program with:
   ./schedule tasks.txt RR/SJF/RRP
   Replace `RR/SJF/RRP` with the desired scheduling algorithm.

### Example:
To run the Round Robin algorithm on `tasks.txt`:
   ./schedule tasks.txt RR

To run the Shortest Job First algorithm:
   ./schedule tasks.txt SJF
