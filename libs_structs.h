#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#define POP_SIZE 2
#define DEBUG 0
#define TEST_ERROR if(errno){\
        dprintf(STDERR_FILENO,"%s:%d: PID=%5d: Error %d (%s)\n", \
					       __FILE__, __LINE__, getpid(), errno, strerror(errno));}

int id, i, j, cont, indx, val, msgid, pref, ninvits, nrejects, sim_time, status, voto, semid, shrmem, mid = 0;
pid_t pid;
sigset_t my_mask;
struct sigaction sa, saold;
struct sembuf op;
struct studente{
  pid_t matr;
  int accettato;
  int vote;
  int pref;
};
struct shared_data{
  struct studente info[POP_SIZE];
  int lett;
};
struct shared_data* sh_data;
struct msgbuf{
  pid_t mtype;
  pid_t mint;
};
struct msgbuf msg;
void openipc();
void aexit();
void signal_handler(int);
void att(int);
void sig(int);
void refuse();
void accept();
