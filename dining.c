#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

// Globals for shared resources
pthread_mutex_t forks[5];
pthread_cond_t cond[5];
bool forkTaken[5];

// Define function signature before main
int pickup_forks(int philosopherNumber);
int return_forks(int philosopherNumber);
void* new_philosopher(void* philosopherNumber);

int main() {
    // 5 threads, one for each philosopher
    pthread_t philosopher[5];

    // Initialize mutex and conditional variables
    for (int i = 0; i < 5; i++) {

        pthread_mutex_init(&forks[i], NULL);
        pthread_cond_init(&cond[i], NULL);
        forkTaken[i] = false;
    }

    // Create the 5 threads each running the new_philosopher function
    int arg[5] = {0, 1, 2, 3, 4};
    for (int i = 0; i < 5; i++) {
        pthread_create(&philosopher[i], NULL, new_philosopher, &arg[i]);
    }

    pthread_join(philosopher[0], NULL);
    return 0;
}

// Alternates between picking up 2 adjacent forks (eating)
// And sleeping for 1-3 seconds (thinking)
void* new_philosopher(void *philosopherNumber) {
    srand(time(0));
    // Cast void pointer argument as int
    int pNum = *(int *)philosopherNumber;

    // Philosopher begins by thinking
    printf("philosopher %d is thinking\n", pNum);
    sleep((rand()%3)+1);

    // Loop forever, alternating between eating and thinking
    while (true) {
        // try to eat
        pickup_forks(pNum);
        sleep((rand()%3)+1);

        // Return to thinking
        return_forks(pNum);
        sleep((rand()%3)+1);
    }
}

int pickup_forks(int philosopherNumber) {
    // Determine which forks that philosopher uses
    int fork1 = philosopherNumber;
    int fork2 = (philosopherNumber + 1) % 5;

    // Lock mutex to protect writing to forkTaken buffer
    pthread_mutex_lock(&forks[fork1]);
    while (forkTaken[fork1] == true) {
        pthread_cond_wait(&cond[fork1], &forks[fork1]);
    }
    pthread_mutex_lock(&forks[fork2]);
    while (forkTaken[fork2] == true) {
        pthread_cond_wait(&cond[fork2], &forks[fork2]);
    }

    // Write to buffer
    forkTaken[fork1] = true;
    forkTaken[fork2] = true;

    // Release mutex
    pthread_mutex_unlock(&forks[fork2]);
    pthread_mutex_unlock(&forks[fork1]);

    // Philospher is now eating
    printf("Philosopher %d is eating.\n", philosopherNumber);
    return 0;
}

int return_forks(int philosopherNumber) {
    // Determine which forks that philosopher uses
    int fork1 = philosopherNumber;
    int fork2 = (philosopherNumber + 1) % 5;

    // Lock mutex
    pthread_mutex_lock(&forks[fork1]);
    pthread_mutex_lock(&forks[fork2]);

    // Write to buffer
    forkTaken[fork1] = false;
    forkTaken[fork2] = false;

    // Release mutex
    pthread_mutex_unlock(&forks[fork1]);
    pthread_mutex_unlock(&forks[fork2]);

    // Philosopher is now thinking
    printf("Philosopher %d is thinking.\n", philosopherNumber);
    return 0;
}
