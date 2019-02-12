#include "libs_structs.h"
#define POP_SIZE 500
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
  alarm(10);
  while(msgrcv(msgid, &msg, sizeof(msg)-sizeof(long), 0, 0) == sizeof(msg)-sizeof(long)){
    printf("\nRICEVUTO MSG:\t (%d) ha chiuso il gruppo[%d]\n", (int)msg.mtype, (int)msg.mtype%POP_SIZE);
  }
}
void init(){
  mid0 = msgget(K0, IPC_CREAT|0666);//mmsgqueue
  mid1 = msgget(K1, IPC_CREAT|0666);//mmsgqueue
  msgid = msgget(pid, IPC_CREAT|0666);//mmsgqueue
  semid = semget(pid, 2, IPC_CREAT|0666);//sems set
  shrmem = shmget(pid, sizeof(struct shared_data), IPC_CREAT|0666);//shrmem area
  sh_data = (struct shared_data*)shmat(shrmem, NULL, 0);//shrmem attacched
  for(i=0; i<POP_SIZE; i++){
    sh_data->gruppi[i].voto = 0;
    sh_data->gruppi[i].chiuso = 0;
    sh_data->gruppi[i].cont = 0;
    for(j=0; j<4; j++) sh_data->gruppi[i].compagni[j] = 0;
  }
  sh_data->ngruppi=0;
  semctl(semid, 0, SETVAL, POP_SIZE);
  semctl(semid, 1, SETVAL, 1);
  sa.sa_handler = &signal_handler;
  sigaction(SIGALRM, &sa, &saold);
  TEST_ERROR;
  printf("\n-_-_-INIZIALIONE-_-_-\n\n");
  atexit(aexit);
}
void aexit(){
  semctl(semid, 2, IPC_RMID);
  msgctl(msgid, IPC_RMID, NULL);
  shmctl(shrmem, IPC_RMID , NULL);
  for(i=0; i<POP_SIZE; i++){
    wait(&status);
  }
  printf("\n\t_-_-_-MEDIA_STUDENTI-_-_-_\n\n");
  int cont=0;
  for(j=18; j<=30; j++){
    for(i=0; i<POP_SIZE; ++i){
      if(sh_data->info[i].vote == j) cont++;
    }
    if(cont) printf("media studenti:%d con voto:%d\n", cont, j);
    cont = 0;
  }
  printf("\n-_-_-_END_SIMULATION_-_-_-\n");
}
void signal_handler(int signalvalue){
  switch(signalvalue){
    case SIGALRM:{
      semctl(semid, 0, SETVAL, POP_SIZE);
      printf("\n\t_-_-_-GRUPPI-_-_-_\n");
      for(i=0; i<POP_SIZE; i++){
        kill(sh_data->info[i].matr, SIGINT);
      }
      for(i=0; i<POP_SIZE; i++){
        sig(1);
        if(sh_data->info[i].accettato == 0){
          sh_data->gruppi[i].compagni[0] = sh_data->info[i].matr;
          sh_data->gruppi[i].voto = 0;
          sh_data->gruppi[i].chiuso = 0;
          sh_data->gruppi[i].cont = 1;
        }
        j=0;
        if(sh_data->gruppi[i].compagni[0] != 0){
          while(j<sh_data->gruppi[i].cont){
            if(j == 0) printf("\ncapo: %d", sh_data->gruppi[i].compagni[j]);
            if(j > 0) printf("\tcompagno[%d]: %d ", j, sh_data->gruppi[i].compagni[j]);
            if(sh_data->gruppi[i].chiuso == 1 && sh_data->info[sh_data->gruppi[i].compagni[j]%POP_SIZE].pref != sh_data->gruppi[i].cont){
              sh_data->gruppi[i].voto -= 3;
            }
            j++;
          }
        }
        printf("\n");
      }
      printf("\n");
      msgctl(mid0, IPC_RMID, NULL);
      msgctl(mid1, IPC_RMID, NULL);
      exit(EXIT_SUCCESS);
    }
  }
}
void att(x){
  op.sem_num = x;
  op.sem_op = -1;
  semop(semid, &op, 1);
}
void sig(x){
  op.sem_num = x;
  op.sem_op = 1;
  semop(semid, &op, 1);
}
