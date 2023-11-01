#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>   
#include <pthread.h> 

int input_array[4];

void *sum(void *thread_id) {

    printf("thread_1 (TID %d) reads X = %d and Y = %d from the input_array[]\n", thread_id, input_array[0], input_array[1]);

    int tot = input_array[0] + input_array[1];

    printf("thread_1 (TID %d) writes X + Y = %d to the input_array[2]\n", thread_id, tot);

    input_array[2] = tot;

}

void *even_odd(void *thread_id) {

    int num = input_array[2];
    printf("thread_2 (TID %d) reads %d from the input_array[2]\n", thread_id, num);
    
    if (num % 2 == 0) {
        printf("thread_2 (TID %d) identifies that %d is an even number\n", thread_id, num); // even
    } else {
        printf("thread_2 (TID %d) identifies that %d is an odd number\n", thread_id, num); // odd
    }

}

void *multiplication(void *thread_id) {
    input_array[3] = input_array[0] * input_array[1];

    printf("thread_2 (TID %d) reads X and Y from input_array[],writes X * Y = %d to input_array[3]\n", thread_id, input_array[3]);
}

void *reverse_num(void *thread_id) {
    printf("thread_3 (TID %d) reads %d from the input_array[3]\n", thread_id, input_array[3]);

    int reversed = 0;
    int num = input_array[3];
    while (num != 0) {
        reversed = reversed * 10 + num % 10;
        num /= 10;
    }

    printf("thread_3 (TID %d) reverses the number %d -> %d\n", thread_id, input_array[3], reversed);
}

void *thread_2_tasks(void *thread_id) {
    even_odd(&thread_id);
    multiplication(&thread_id);

}

int main(int argc, char **argv) {

    pthread_t thread_1, thread_2, thread_3;
    int X = atoi(argv[1]);
    int Y = atoi(argv[2]);

    printf("parent(PID %d) receives X = %d and Y = %d from the user\n", getpid(), X, Y);

    input_array[0] = X;
    input_array[1] = Y;

    printf("parent(PID %d) writes X = %d and Y = %d to input_array[]\n", getpid(), X, Y);

    pthread_create(&thread_1, NULL, sum, &thread_1);
    pthread_join(thread_1, NULL);

    pthread_create(&thread_2, NULL, thread_2_tasks, &thread_2);
    pthread_join(thread_2, NULL);

    pthread_create(&thread_3, NULL, reverse_num, &thread_3);
    pthread_join(thread_3, NULL);

    return 0;

}