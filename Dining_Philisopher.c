#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (i + 4) % N
#define RIGHT (i + 1) % N
int state[N];
int phil[N] = {0, 1, 2, 3, 4};
sem_t mutex;
sem_t S[N];
int mysem;
struct shm
{
    int state[N];
} * shared_memory;

/* arg for semctl system calls. */
union semun
{
    int val;               //value for SETVAL
    struct semid_ds *buf;  //buffer for IPC_STAT & IPC_SET
    ushort *array;         // array for GETALL & SETALL
    struct seminfo *__buf; //buffer for IPC_INFO
} arg;

void Initialize_shared_memory()
{
    int shmid;
    //key_t key = 99999;
    //key = ftok(".", 'a');
    //Create the segment.
    shmid = shmget(IPC_PRIVATE, sizeof(*shared_memory), IPC_CREAT | 0666);
    printf("memory attached at shmid %d\n", shmid);
    if (shmid < 0)
    {
        perror("shmget: ");
        exit(1);
    }
    shared_memory = (struct shm *)shmat(shmid, NULL, 0); //attach memory
    printf("shmat succeed\n");
    if (shared_memory == (void *)-1)
    {
        printf("shmat errror\n");
        exit(1);
    }
}
void intialize_semaphores()
{
    //initialize all semaphores value=1
    int j;
    int retval;
    ////create a semaphore set with N semaphores using IPC private
    mysem = semget(IPC_PRIVATE, N + 1, 0666 | IPC_CREAT);
    if (mysem < 0)
    {
        printf("Error semget\n");
    }
    printf("created semaphore id %d\n", mysem);

    for (j = 0; j <= N; j++) // N+1 intializing for each semaphores and one mutex
    {
        arg.val = 1;
        retval = semctl(mysem, j, SETVAL, arg);
    }
    if (retval < 0)
    {
        printf("Error intializing semaphore");
        exit(1);
    }
    printf("succefully intialized semaphores \n");
}

void up_down(int i, int act)
{
    struct sembuf op;
    int retval;
    op.sem_num = i;  //          number of semaphores
    op.sem_op = act; //1=V, -1=P  semaphore operation
    op.sem_flg = 0;  //will wait   operation flags
    retval = semop(mysem, &op, 1);
    if (retval != 0)
    {
        printf("error:semop: \n", act);
    }
}
void down(int i)
{
    up_down(i, -1); //operation p in semaphore i
}
void up(int i) //operation v in semaphore i
{
    up_down(i, 1);
}

void test(int i)
{
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        // state that eating
        state[i] = EATING;
        sleep(2);
        printf("Philosopher %d takes fork %d and %d\n",
               i + 1, LEFT + 1, i + 1);
        printf("Philosopher %d is Eating\n", i + 1);
        up(&S[i]);
    }
}
// take up forks
void grab_forks(int i)
{
    down(&mutex);
    // state of hungry
    state[i] = HUNGRY;
    printf("Philosopher %d is Hungry\n", i + 1);
    // eat if neighbours are not eating
    test(i);
    up(&mutex);
    // if unable to eat wait to be signalled
    down(&S[i]);
    sleep(1);
}

// put down forks
void put_away_forks(int i)
{
    down(&mutex);
    // state of thinking
    state[i] = THINKING;
    printf("Philosopher %d putting fork %d and %d down\n",
           i + 1, LEFT + 1, i + 1);
    printf("Philosopher %d is thinking\n", i + 1);
    test(LEFT);
    test(RIGHT);
    up(&mutex);
}
// thread displaying information
void *philospher(void *num)
{
    while (1)
    {
        int *j = num;
        sleep(1);
        grab_forks(*j);
        sleep(0);
        put_away_forks(*j);
    }
}
int main()
{
    Initialize_shared_memory();
    intialize_semaphores();
    int k;
    pthread_t thread_id[N];
    for (k = 0; k < N; k++)
    {
        // create philosopher processes
        pthread_create(&thread_id[k], NULL,
                       philospher, &phil[k]);
        printf("Philosopher %d is thinking\n", k + 1);
    }
    for (k = 0; k < N; k++)
        pthread_join(thread_id[k], NULL); // waiting to thread still to end
    return 0;
}
