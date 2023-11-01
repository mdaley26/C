#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

int summation(int start, int end)
{
	int sum = 0;
	if (start < end)
	{
		sum = ((end * (end + 1)) - (start * (start - 1))) / 2;
	}
	return sum;
}

int ith_part_start(int i, int N, int M)
{
	int part_size = N / M;
	int start = i * part_size;
	return start;
}
int ith_part_end(int i, int N, int M)
{
	int part_size = N / M;
	int end = (i < M - 1) ? ((i + 1) * part_size - 1) : N;
	return end;
}
int main(int argc, char **argv)
{
    printf("parent(PID %d): process started\n", getpid());

    int N = atoi(argv[1]);
    int M = atoi(argv[2]);

    if (N < 1 || N > 100 || M < 1 || M > 10) {
        fprintf(stderr, "Non-valid input. (1 <= N <= 100; 1 <= M <= 10)\n");
    }

    printf("parent(PID %d): forking child_1\n", getpid());
    int piped[2];
    if (pipe(piped) == -1) {
        perror("pipe");
        return 1;
    }
    pid_t child1 = fork();


    if (child1 == 0) {
        // In Child_1
        int final_sum = 0;
        int part_sum;
        
        printf("parent(PID %d): fork successful for child_1 (PID %d)\n", getppid(), getpid());
        printf("parent(PID %d): waiting for child_1 (PID %d) to complete \n", getppid(), getpid());
        printf("child_1(PID %d): process started from parent (PID %d)\n", getpid(), getppid());
        printf("child_1(PID %d): forking child_1.1....child_1.%d\n", getpid(), M);
        for (int i = 0; i < M; i++) {
            pid_t children = fork(); // child 1.1 to 1.M
            if (children == 0) {
                printf("child_1.%d(PID %d): fork() successful\n", i+1, getpid());
                int start = ith_part_start(i, N, M);
                int end = ith_part_end(i, N, M);
                part_sum = summation(start, end);
                printf("child_1.%d(PID %d): partial sum: [%d - %d] = %d\n", i+1, getpid(), start, end, part_sum);
                write(piped[1], &part_sum, sizeof(part_sum));
                exit(0);
            }
            wait(NULL);
        }
        wait(NULL);
        for (int i = 0; i < M; i++) {
            read(piped[0], &part_sum, sizeof(part_sum));
            final_sum += part_sum; 
        }
    
        printf("child_1(PID %d): total sum: %d\n", getpid(), final_sum);
        printf("child_1(PID %d): child_1 completed\n", getpid());
    } else {
        // In Parent
        waitpid(child1, NULL, 0);

        // Parent process stops
        printf("parent(PID %d): parent completed\n", getpid());
        exit(0);
    }

    return 0;
}