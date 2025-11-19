#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h> //pid
#include <unistd.h> //misc funcs POSIX
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdbool.h>


/*Semaphores names*/
#define SEM_1 "/sem1"
#define SEM_2 "/sem2"
#define SEM_3 "/sem3"
#define SEM_4 "/sem4"
#define SEM_5 "/sem5"
#define SEM_6 "/sem6"

/*Shared variables*/
int* global_count;
int* HACK_count;
int* SERF_count;
int* QUEUE_count;
int* HACK_p_count;
int* SERF_p_count;
int* HACK_voyage_count;
int* SERF_voyage_count;
int* barrier_count;

/*Semaphores*/
sem_t *gen_sem;
sem_t *mutex;
sem_t *hack_sem;
sem_t *serf_sem;
sem_t *pier_sem;
sem_t *barrier_sem;

void clear(FILE* fptr)
{
  shmdt(global_count);
  shmdt(HACK_count);
  shmdt(SERF_count);
  shmdt(QUEUE_count);
  shmdt(SERF_p_count);
  shmdt(HACK_p_count);
  shmdt(HACK_voyage_count);
  shmdt(SERF_voyage_count);
  shmdt(barrier_count);

  sem_close(gen_sem);
  sem_unlink(SEM_1);
  sem_close(mutex);
  sem_unlink(SEM_2);
  sem_close(hack_sem);
  sem_unlink(SEM_3);
  sem_close(serf_sem);
  sem_unlink(SEM_4);
  sem_close(pier_sem);
  sem_unlink(SEM_5);
  sem_close(barrier_sem);
  sem_unlink(SEM_6);
  /*shmctl(mem_global_count, IPC_RMID, NULL);
  shmctl(mem_HACK_count, IPC_RMID, NULL);
  shmctl(mem_SERF_count, IPC_RMID, NULL);*/
  fclose(fptr);
}

void initialize(FILE *fptr)
{

  int mem_global_count = 0;
  int mem_HACK_count = 0;
  int mem_SERF_count = 0;
  int mem_QUEUE_count = 0;
  int mem_SERF_p_count = 0;
  int mem_HACK_p_count = 0;
  int mem_HACK_voyage_count = 0;
  int mem_SERF_voyage_count = 0;
  int mem_barrier_count = 0;

  mem_global_count = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  if (mem_global_count == 1){
    fprintf(stderr, "Shared memory segment creation failed.\n");
    clear(fptr);
    exit(1);
    }
  global_count = shmat(mem_global_count, NULL, 0);
  if (global_count == ((void *) -1)){
    fprintf(stderr, "Attaching memory segment to adress failed.\n");
    clear(fptr);
    exit(1);
  }
////////////////////////////////////////////////////////////////
  mem_HACK_count = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  if (mem_HACK_count == 1){
    fprintf(stderr, "Shared memory segment creation failed.\n");
    clear(fptr);
    exit(1);
    }
  HACK_count = shmat(mem_HACK_count, NULL, 0);
  if (HACK_count == ((void *) -1)){
    fprintf(stderr, "Attaching memory segment to adress failed.\n");
    clear(fptr);
    exit(1);
  }
////////////////////////////////////////////////////////////////
  mem_SERF_count = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  if (mem_SERF_count == 1){
    fprintf(stderr, "Shared memory segment creation failed.\n");
    clear(fptr);
    exit(1);
    }
  SERF_count = shmat(mem_SERF_count, NULL, 0);
  if (SERF_count == ((void *) -1)){
    fprintf(stderr, "Attaching memory segment to adress failed.\n");
    clear(fptr);
    exit(1);
  }
////////////////////////////////////////////////////////////////
  mem_QUEUE_count = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  if (mem_QUEUE_count == 1){
    fprintf(stderr, "Shared memory segment creation failed.\n");
    clear(fptr);
    exit(1);
    }
  QUEUE_count = shmat(mem_QUEUE_count, NULL, 0);
  if (QUEUE_count == ((void *) -1)){
    fprintf(stderr, "Attaching memory segment to adress failed.\n");
    clear(fptr);
    exit(1);
  }
////////////////////////////////////////////////////////////////
  mem_SERF_p_count = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  if (mem_SERF_p_count == 1){
    fprintf(stderr, "Shared memory segment creation failed.\n");
    clear(fptr);
    exit(1);
    }
  SERF_p_count = shmat(mem_SERF_p_count, NULL, 0);
  if (SERF_p_count == ((void *) -1)){
    fprintf(stderr, "Attaching memory segment to adress failed.\n");
    clear(fptr);
    exit(1);
  }
////////////////////////////////////////////////////////////////
  mem_HACK_p_count = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  if (mem_HACK_p_count == 1){
    fprintf(stderr, "Shared memory segment creation failed.\n");
    clear(fptr);
    exit(1);
    }
  HACK_p_count = shmat(mem_HACK_p_count, NULL, 0);
  if (HACK_p_count == ((void *) -1)){
    fprintf(stderr, "Attaching memory segment to adress failed.\n");
    clear(fptr);
    exit(1);
  }
////////////////////////////////////////////////////////////////
  mem_HACK_voyage_count = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  if (mem_HACK_voyage_count == 1){
    fprintf(stderr, "Shared memory segment creation failed.\n");
    clear(fptr);
    exit(1);
    }
  HACK_voyage_count = shmat(mem_HACK_voyage_count, NULL, 0);
  if (HACK_voyage_count == ((void *) -1)){
    fprintf(stderr, "Attaching memory segment to adress failed.\n");
    clear(fptr);
    exit(1);
  }
////////////////////////////////////////////////////////////////
  mem_SERF_voyage_count = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  if (mem_SERF_voyage_count == 1){
    fprintf(stderr, "Shared memory segment creation failed.\n");
    clear(fptr);
    exit(1);
    }
  SERF_voyage_count = shmat(mem_SERF_voyage_count, NULL, 0);
  if (SERF_voyage_count == ((void *) -1)){
    fprintf(stderr, "Attaching memory segment to adress failed.\n");
    clear(fptr);
    exit(1);
  }
////////////////////////////////////////////////////////////////
  mem_barrier_count = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  if (mem_barrier_count == 1){
    fprintf(stderr, "Shared memory segment creation failed.\n");
    clear(fptr);
    exit(1);
    }
  barrier_count = shmat(mem_barrier_count, NULL, 0);
  if (barrier_count == ((void *) -1)){
    fprintf(stderr, "Attaching memory segment to adress failed.\n");
    clear(fptr);
    exit(1);
  }
////////////////////////////////////////////////////////////////

  *global_count = 1;
  *HACK_count = 1;
  *SERF_count = 1;
  *barrier_count = 0;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
  gen_sem = sem_open(SEM_1, O_CREAT | O_EXCL, 0666, 1);
  if(gen_sem == SEM_FAILED){
      fprintf(stderr, "Opening semaphore failed.\n");
      clear(fptr);
      exit(1);
  }
  mutex = sem_open(SEM_2, O_CREAT | O_EXCL, 0666, 1);
  if(mutex == SEM_FAILED){
      fprintf(stderr, "Opening semaphore failed.\n");
      clear(fptr);
      exit(1);
  }
  hack_sem = sem_open(SEM_3, O_CREAT | O_EXCL, 0666, 0);
  if(hack_sem == SEM_FAILED){
      fprintf(stderr, "Opening semaphore failed.\n");
      clear(fptr);
      exit(1);
  }
  serf_sem = sem_open(SEM_4, O_CREAT | O_EXCL, 0666, 0);
  if(serf_sem == SEM_FAILED){
      fprintf(stderr, "Opening semaphore failed.\n");
      clear(fptr);
      exit(1);
  }
  pier_sem = sem_open(SEM_5, O_CREAT | O_EXCL, 0666, 1);
  if(pier_sem == SEM_FAILED){
      fprintf(stderr, "Opening semaphore failed.\n");
      clear(fptr);
      exit(1);
  }
  barrier_sem = sem_open(SEM_6, O_CREAT | O_EXCL, 0666, 0);
  if(barrier_sem == SEM_FAILED){
      fprintf(stderr, "Opening semaphore failed.\n");
      clear(fptr);
      exit(1);
  }

}



void generate(int person_num, int hacker_time, int serf_time, FILE* f, pid_t pid, int* Proc_ID)
{
  time_t t;
  srand((unsigned) time(&t));

  if (pid < 0) //Mistake
  {
    perror("fork");
    exit(-1);
  }
/////////////////////////////////////////////////////////////////////
  else if (pid != 0) //Povodny parent proces robí serfs
  {
    for (int i = 0; i < person_num; i++)
    {
      if (pid != 0)
      {
        if (serf_time == 0) usleep(0);
        else usleep(rand() % serf_time * 1000);

        pid = fork();
        if (pid < 0) //Error
        {
        perror("fork");
        exit(-1);
        }
      }
    }
     if (pid != 0)
      {
      exit(0);
      }
  }
/////////////////////////////////////////////////////////////////////
  else  // child proces
  {
    for (int i = 0; i < person_num; i++)
    {
      if (pid == 0)
      {
        if (hacker_time == 0) usleep(0);
        else usleep(rand() % hacker_time * 1000);

        pid = fork();
        if (pid < 0)
        {
          perror("fork");
          exit(-1);
        }
      }
    }
    if (pid == 0) exit(0);
  };

  if (pid != 0)//P nových procesov z parents procesov
  {
    sem_wait(gen_sem);
    fprintf(f,"%d\t: HACK %d\t: starts\n",*global_count, *SERF_count);
    fflush(f);
    *Proc_ID = *SERF_count;
    (*global_count)++;
    (*SERF_count)++;
    sem_post(gen_sem);
    //Koniec generovania SERFS, pokračujú dalej
  }

  if (pid == 0) //P nových procesov z children procesov
  {
    sem_wait(gen_sem);
    fprintf(f,"%d\t: SERF %d\t: starts\n",*global_count, *HACK_count);
    fflush(f);
    *Proc_ID = *HACK_count;
    (*global_count)++;
    (*HACK_count)++;
    sem_post(gen_sem);
    //Koniec generovania HACKS, pokračujú ďalej
  }



}

void pier(int waiting_time, int capacity, FILE* f, pid_t pid, int Proc_ID)
{
time_t t;
srand((unsigned) time(&t));

while ( 1 )
  {
    sem_wait(gen_sem);
    if (*QUEUE_count >= capacity)
    {
      if (pid != 0) //
      {
        fprintf(f,"%d\t: SERF %d\t: leaves queue\t: %d\t: %d \n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
        fflush(f);
        (*global_count)++;
        sem_post(gen_sem);
        usleep((rand() % waiting_time + 20)* 1000);
        sem_wait(gen_sem);
        fprintf(f,"%d\t: SERF %d\t: is back \n",*global_count, Proc_ID);
        fflush(f);
        (*global_count)++;
        sem_post(gen_sem);
        continue;
      }

      else /////////////////////////////////
      {
        fprintf(f,"%d\t: HACK %d\t: leaves queue\t: %d\t: %d \n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
        fflush(f);
        (*global_count)++;
        sem_post(gen_sem);
        usleep((rand() % waiting_time + 20)* 1000);
        sem_wait(gen_sem);
        fprintf(f,"%d\t: HACK %d\t: is back \n",*global_count, Proc_ID);
        fflush(f);
        (*global_count)++;
        sem_post(gen_sem);
        continue;
      }


    }
    else
    {
      (*QUEUE_count)++;
      if (pid != 0) //serf
      {
        (*SERF_p_count)++;
        fprintf(f,"%d\t: SERF %d\t: waits\t: %d\t: %d \n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
        fflush(f);
        (*global_count)++;
        sem_post(gen_sem);
      }

      else //hack////////////////////////////
      {
        (*HACK_p_count)++;
        fprintf(f,"%d\t: HACK %d\t: waits\t: %d\t: %d \n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
        fflush(f);
        (*global_count)++;
        sem_post(gen_sem);
      }
      break;
    }
  }
}

void voyage(int voyage_time, FILE* f, pid_t pid, int Proc_ID)
{
  time_t t;
  bool captain = false;
  srand((unsigned) time(&t));

  sem_wait(mutex);

  if (pid != 0) //serf
  {
    (*SERF_voyage_count)++;

    if (*SERF_voyage_count == 4)
    {
      *SERF_voyage_count = 0;
      captain = true;
      sem_wait(gen_sem);
      *SERF_p_count = *SERF_p_count -4;
      fprintf(f,"%d\t : SERF %d\t: boards\t: %d\t: %d\n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
      fflush(f);
      (*global_count)++;
      *QUEUE_count = *QUEUE_count - 4;
      sem_post(gen_sem);

      if (voyage_time == 0) usleep(0);
      else usleep(rand() % voyage_time * 1000);

      for (int i = 0; i < 4; i++) sem_post(serf_sem);
    }
    else if (*SERF_voyage_count == 2 && *HACK_voyage_count >= 2)
    {
      *SERF_voyage_count = 0;
      *HACK_voyage_count = *HACK_voyage_count -2;
      captain = true;
      sem_wait(gen_sem);
      *HACK_p_count = *HACK_p_count -2;
      *SERF_p_count = *SERF_p_count -2;
      fprintf(f,"%d\t: SERF %d\t: boards\t: %d\t: %d\n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
      fflush(f);
      (*global_count)++;
      *QUEUE_count = *QUEUE_count - 4;
      sem_post(gen_sem);

      if (voyage_time == 0) usleep(0);
      else usleep(rand() % voyage_time * 1000);

      for (int i = 0; i < 2; i++) {
        sem_post(hack_sem);
        sem_post(serf_sem);
      }
    }
    else sem_post(mutex);

    sem_wait(serf_sem);
  }
  else //hack///////////////////////////////////////
  {
    (*HACK_voyage_count)++;

    if (*HACK_voyage_count == 4)
    {
      *HACK_voyage_count = 0;
      captain = true;
      sem_wait(gen_sem);
      *HACK_p_count = *HACK_p_count -4;
      fprintf(f,"%d\t: HACK %d\t: boards\t: %d\t: %d\n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
      fflush(f);
      (*global_count)++;
      *QUEUE_count = *QUEUE_count - 4;
      sem_post(gen_sem);

      if (voyage_time == 0) usleep(0);
      else usleep(rand() % voyage_time * 1000);

      for (int i = 0; i < 4; i++) sem_post(hack_sem);

    }
    else if (*HACK_voyage_count == 2 && *SERF_voyage_count >= 2)
    {
      *HACK_voyage_count = 0;
      *SERF_voyage_count = *SERF_voyage_count - 2;
      captain = true;
      sem_wait(gen_sem);
      *HACK_p_count = *HACK_p_count -2;
      *SERF_p_count = *SERF_p_count -2;
      fprintf(f,"%d\t: HACK %d\t: boards\t: %d\t: %d\n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
      fflush(f);
      (*global_count)++;
      *QUEUE_count = *QUEUE_count - 4;
      sem_post(gen_sem);

      if (voyage_time == 0) usleep(0);
      else usleep(rand() % voyage_time * 1000);

      for (int i = 0; i < 2; i++) {
        sem_post(serf_sem);
        sem_post(hack_sem);
      }
    }
    else sem_post(mutex);

    sem_wait(hack_sem);
  }

if (pid != 0)
{//serf
  if (captain == false)
  {
    sem_wait(gen_sem);
    fprintf(f,"%d\t: SERF %d\t: member exits\t: %d\t: %d\n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
    fflush(f);
    (*global_count)++;
    (*barrier_count)++;
    sem_post(gen_sem);
    if (((*barrier_count) % 3) == 0)
        sem_post(barrier_sem);

  }
  if (captain == true)
  { sem_wait(barrier_sem);
    sem_wait(gen_sem);
    fprintf(f,"%d\t: SERF %d\t: captain exits\t: %d\t: %d\n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
    fflush(f);
    (*global_count)++;
    sem_post(gen_sem);
    sem_post(mutex);
  }
}
else //hack
if (captain == false)
{
  sem_wait(gen_sem);
  fprintf(f,"%d\t: HACK %d\t: member exits\t: %d\t: %d\n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
  fflush(f);
  (*global_count)++;
  (*barrier_count)++;
  sem_post(gen_sem);
  if (((*barrier_count) % 3) == 0)
      sem_post(barrier_sem);

}
if (captain == true)
{   sem_wait(barrier_sem);
    sem_wait(gen_sem);
    fprintf(f,"%d\t: HACK %d\t: captain exits\t: %d\t: %d\n",*global_count, Proc_ID, *HACK_p_count, *SERF_p_count);
    fflush(f);
    (*global_count)++;
    sem_post(gen_sem);
    sem_post(mutex);

}
}




int main(int argc, char const *argv[]) {

  int P, H, S, R, W, C;
  if (argc != 7) {
    fprintf(stderr, "Incorrect number of argumets\n");

  } else {
    P = atoi(argv[1]); //Počet osôb
    H = atoi(argv[2]); //Max doba pre generovanie Hackrov
    S = atoi(argv[3]); //Max doba pre generovanie Serfov
    R = atoi(argv[4]); //Max doba plavby
    W = atoi(argv[5]); //Max doba po ktorej sa osoba vráti na mólo
    C = atoi(argv[6]); //Kapacita móla
  }

  {
  if (P < 2 || (P%2) != 0) {
    fprintf(stderr, "%s is invalid, choose P > 2 && P is odd\n",argv[1]);
    exit(1);
  }

  if (H < 0 || H > 2000) {
    fprintf(stderr, "%s is invalid, choose from range 0 - 2000\n",argv[2]);
    exit(1);
  }

  if (S < 0 || S > 2000) {
    fprintf(stderr, "%s is invalid, choose from range 0 - 2000\n",argv[3]);
    exit(1);
  }

  if (R < 0 || R > 2000) {
    fprintf(stderr, "%s is invalid, choose from range 0 - 2000\n",argv[4]);
    exit(1);
  }

  if (W < 20 || W > 2000) {
    fprintf(stderr, "%s is invalid, choose from range 20 - 2000\n",argv[5]);
    exit(1);
  }

  if (C < 5) {
    fprintf(stderr, "%s is invalid, capacity cannot be less than 5\n",argv[6]);
    exit(1);
  }
}

  FILE *fptr;
  fptr = fopen("./proj2.out","w");
  if(fptr == NULL)
  {
    fprintf(stderr, "Could not open file\n");
    return 1;
  }

  pid_t pid;
  int Proc_ID = 0;

  initialize(fptr);

  if ((pid = fork()) < 0) {
    perror("fork");
    exit(2);
  }
  generate(P, H, S, fptr, pid, &Proc_ID);
  pier(W, C, fptr, pid, Proc_ID);
  voyage(R, fptr, pid, Proc_ID);

  clear(fptr);
  return 0;
}
