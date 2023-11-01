#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t child1, child2;
    int status;
    printf("parent (PID %d): process started\n", getpid());

    // Create child_1

    printf("parent (PID %d): forking child_1\n", getpid());
    child1 = fork();

    

    if (child1 == 0) {
        printf("parent (PID %d): fork successful for child_1 (PID %d)\n", getppid(), getpid());
        printf("parent (PID %d): waiting for child_1 (PID %d) to complete \n", getppid(), getpid());
        printf("child_1 (PID %d): process started from parent (PID %d)\n", getpid(), getppid());
        // In child_1

        // fork for child_1.1 and child_1.2
        pid_t child11, child12;

        printf("child_1 (PID %d): forking child_1.1\n", getpid());
        child11 = fork();
        
        
        if (child11 == 0) {
            printf("child_1.1 (PID %d): process started from child_1 (PID %d)\n", getpid(), getppid());
            // In child_1.1

            // Execute external_program1.out 
            printf("child_1.1 (PID %d): calling an external program [./external_program1.out]\n", getpid());
            execl("./external_program1.out", argv[2], NULL);
            exit(1);
        } else {
            // In child_1 
            waitpid(child11, NULL, 0);
            printf("child_1 (PID %d): completed child_1.1\n", getpid());
            printf("child_1 (PID %d): forking child_1.2\n", getpid());
            child12 = fork();
            

            if (child12 == 0) {
                printf("child_1.2 (PID %d): process started from child_1 (PID %d)\n", getpid(), getppid());
                // In child_1.2

                // Execute external_program1.out 
                printf("child_1.2 (PID %d): calling an external program [./external_program1.out]\n", getpid());
                execl("./external_program1.out", argv[1], NULL);
                exit(1);
            } else {
                // In child_1 

                // Wait for child_1.1 and child_1.2 to complete
                waitpid(child11, NULL, 0);

                waitpid(child12, NULL, 0);
                printf("child_1 (PID %d): completed child_1.2\n", getpid());

                exit(0);
            }
        }
    } else {
        // In the parent 

        // Wait for child_1 to complete
        waitpid(child1, NULL, 0);

        // Create child_2
        printf("parent (PID %d): forking child_2\n", getpid());
        child2 = fork();
        

        if (child2 == 0) {
            printf("parent (PID %d): fork successful for child_2 (PID %d)\n", getppid(), getpid());
            printf("child_2 (PID %d): process started from parent (PID %d)\n", getpid(), getppid());
            // In child_2

            // Execute external_program2.out 
            printf("child_2 (PID %d): calling an external program [./external_program2.out]\n", getpid());
            execl("./external_program2.out", argv[3], NULL);
            exit(1);
        } else {
            // In the parent 
            
            // Wait for child_2 to complete
            waitpid(child2, &status, 0);

            // Parent process stops
            printf("parent (PID %d): completed parent\n", getpid());
            exit(0);
        }
    }

    return 0;
}

