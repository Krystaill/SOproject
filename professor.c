#include "libs_structs.h"

#define POP_SIZE 15

struct gruppo{
  pid_t capo;
  pid_t compagni[3];
  int voto;
};
struct gruppo gruppi[POP_SIZE];
int main(int argc, char const *argv[]){
  pid = getpid();
  init();
  for(i=0; i < POP_SIZE; i++){
    if(fork()){
    }else{
      execve("student", 0, 0);
      TEST_ERROR;
    }
  }
  op.sem_num = 0;
  op.sem_op = 0;
  semop(semid, &op, 1);
  printf(">>> Sono tutti a lavoro. . .\n");
  i=0;
  alarm(120);
  while(msgrcv(msgid, &msg, sizeof(pid_t), 0, 0) == sizeof(msg)){
    printf("lettura msg\n");
    for(; i<POP_SIZE; i++){
      if(gruppi[i].capo == 0){
        gruppi[i].capo = msg.mtype;
        gruppi[i].voto = sh_data->info[msg.mtype%POP_SIZE].vote;
      }
      if(gruppi[i].capo == msg.mtype){
        if(gruppi[i].voto < sh_data->info[msg.mint%POP_SIZE].vote)
          gruppi[i].voto = sh_data->info[msg.mint%POP_SIZE].vote;
        for(j=0; j < sh_data->info[msg.mtype%POP_SIZE].pref; j++){
          if(gruppi[i].compagni[j] == 0)
            gruppi[i].compagni[j] = msg.mint;
            break;
        }
        break;
      }
    }
  }
}

void init(){
  TEST_ERROR;
  for(i=0; i<POP_SIZE; i++){
    gruppi[i].capo = 0;
    gruppi[i].voto = 0;
    for(j=0; j<3; j++) gruppi[i].compagni[j] = 0;
  }
  TEST_ERROR;
  msgid = msgget(pid, IPC_CREAT|0666);//mmsgqueue
  TEST_ERROR;
  semid = semget(pid, 4, IPC_CREAT|0666);//sems set
  TEST_ERROR;
  shrmem = shmget(pid, sizeof(struct shared_data), IPC_CREAT|0666);//shrmem area
  TEST_ERROR;
  sh_data = (struct shared_data*)shmat(shrmem, NULL, 0);//shrmem attacched
  TEST_ERROR;
  semctl(semid, 0, SETVAL, POP_SIZE);
  semctl(semid, 1, SETVAL, 1);
  semctl(semid, 2, SETVAL, 1);
  semctl(semid, 3, SETVAL, 1);
  TEST_ERROR;
  sa.sa_handler = &signal_handler;
  sigaction(SIGALRM, &sa, &saold);
  TEST_ERROR;
  atexit(aexit);
}

void aexit(){
  semctl(semid, 4, IPC_RMID);
  msgctl(msgid, IPC_RMID, NULL);
  shmctl(shrmem, IPC_RMID , NULL);
}

void signal_handler(int signalvalue){
  switch(signalvalue){
    case SIGALRM:{
      for(i=0; i<POP_SIZE; i++){
        kill(sh_data->info[i].matr, SIGINT);
        wait(&status);
        TEST_ERROR;
      }
      for(i=0; i<POP_SIZE; i++){
        if(gruppi[i].capo!=0){
            printf("\tcapo: %d\n", gruppi[i].capo);
            for(j=0; j<POP_SIZE; j++){
              if(gruppi[i].compagni[j]!=0) printf("\t%d: %d\t", j, gruppi[i].compagni[j]);
            }
            printf("\t\tvoto finale _%d_\n", gruppi[i].voto);
        }
        TEST_ERROR;
      }
      printf("-_-_-_END_SIMULATION_-_-_-\n");
      exit(EXIT_SUCCESS);
    }
  }
}
