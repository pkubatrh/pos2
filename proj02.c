/*  POS project 2: simple ticket algorithm
 *  Author: xkubat11@stud.fit.vutbr.cz
 *  Description:
 *      The appliaction creates a given number of threads which then
 * 		are synchronized using a simple ticket algorithm using
 * 		posix mutexes and conditions.
 *
 */

#define _POSIX_C_SOURCE 199506L
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

pthread_mutex_t mutex;
pthread_cond_t cond;
int current = 0;
int tCount = 0;
int passCounter = 0;

/* random sleep function */
void my_sleep(int id) {

struct timespec ts;
unsigned int seed = getpid() * (id + 1); /* id can be 0! */

ts.tv_nsec = (int)(rand_r(&seed) / ((double)RAND_MAX + 1) * 500000000);
ts.tv_sec = 0;

nanosleep(&ts,NULL);
}

/* a function used to assign a ticket */
int getticket () {
    int ticketNum = 0;

    pthread_mutex_lock(&mutex);

    ticketNum = tCount++;

    pthread_mutex_unlock(&mutex);
    return ticketNum;
}

/* a function used to enter the critical section */
void await(int aenter) {
    pthread_mutex_lock(&mutex);

    while (current != aenter)
        pthread_cond_wait(&cond, &mutex);

    pthread_mutex_unlock(&mutex);

    return;
}

/* function used to leave the critical section */
void advance () {
    pthread_mutex_lock(&mutex);
	/* increment the ticket counter */
    current++;
    pthread_cond_broadcast(&cond);

    pthread_mutex_unlock(&mutex);

    return;
}

/* thread function
 * Prints its id and ticket number
 */
void *thread_foo(void *id) {

    int myTicket;

    while((myTicket = getticket()) < passCounter) {
        my_sleep((int)id);
        await(myTicket);
        /*Crit Section */
        printf("%d\t(%d)\n", myTicket, (int)id);
        /*Crit Section end*/
        advance();
        my_sleep((int)id);
    }

    return (void *) 0;
}



int main(int argc, char* argv[]) {

    pthread_t *threadArr;
    pthread_attr_t attr;
    int threads = 0;
    int i = 0;
    int retval;

    /* arguments */
    threads = atoi(argv[1]);
    passCounter = atoi(argv[2]);
    /* mutex init */
    pthread_mutex_init(&mutex, NULL);

    /* attr init */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* cond init */
    pthread_cond_init(&cond, NULL);

    threadArr = (pthread_t *) malloc(sizeof(pthread_t) * threads);

    /* Create threads */
    for (i = 0; i<threads; i++) {
        if ((retval = pthread_create(&threadArr[i], &attr, thread_foo, (void *) i)) != 0 ) {
            printf("Cannot create the set number of threads. Exiting.\n");
            return 1;
        }
    }

    /* Join threads */
    for (i = 0; i<threads; i++)
        retval = pthread_join(threadArr[i], NULL);

    /* Cleanup */
    free(threadArr);
    pthread_mutex_destroy(&mutex);
    pthread_attr_destroy(&attr);
    pthread_cond_destroy(&cond);

    return 0;
}

