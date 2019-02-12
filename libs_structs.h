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
#define POP_SIZE 500
#define K0 44
#define K1 33
#define TEST_ERROR if(errno){\
        dprintf(STDERR_FILENO,"%s:%d: PID=%5d: Error %d (%s)\n", \
					       __FILE__, __LINE__, getpid(), errno, strerror(errno));}
int id, i, j, cont, indx, val, msgid, pref, ninvits, nrejects, status, voto, semid, shrmem, mid, mid0, mid1 = 0;
pid_t pid, pidcapo = 0;
sigset_t my_mask;
struct sigaction sa, saold;
struct sembuf op;
struct studente{
  pid_t matr;
  int accettato;
  int vote;
  int pref;
};
struct gruppo{
  pid_t compagni[4];
  int voto;
  int chiuso;
  int cont;
};
struct shared_data{
  struct studente info[POP_SIZE];
  struct gruppo gruppi[POP_SIZE];
  int ngruppi;
};
struct shared_data* sh_data;
struct msgbuf{
  long mtype; //to who
  int mode; //mode
  pid_t mpid; //from who
};
struct msgbuf msg;
void init();
void openipc();
void signal_handler(int);
void aexit();
void att(int);
void sig(int);
void refuse();
void accept();
void read_config();
