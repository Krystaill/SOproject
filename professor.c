#include "libs_structs.h"
struct gruppo{
  int i;
  pid_t capo;
  pid_t compagni[pref];
  int voto;
};
int main(int argc, char const *argv[]) {
  atexit(aexit);
  sigemptyset(&my_mask); //Do not mask any signal
	sa.sa_mask = my_mask;
  sa.sa_handler = &signal_handler;
  //sa.sa_flags = SA_NODEFER; da rivedere
  sigaction(SIGUSR1, &sa, &saold);
  openipc();
  op.sem_num = 0;
  sh_data->i = 0;
  for(; i < POP_SIZE; ++i){
    if(fork()){
    }else{
      execve("student", 0, 0);
      exit(-1);
    }
  }
  op.sem_op = 0;
  semop(semid, &op, 1);
  printf("Sono tutti sulla linea di partenza\n");
  for(i=0;  i < POP_SIZE; i++) {
    TEST_ERROR;
    //sprintf(str, "%s", sh_data->info[i].name);
    //printf("\t%d %d\n", sh_data->info[i].matr, sh_data->info[i].vote);
  }
  TEST_ERROR;
  while(msgrcv(msgid, &msg, 32, 0, IPC_NOWAIT)){
      //printf("\t%d %d\n", msg.mtype, atoi(msg.mtext));
  }
  printf("\n");
  TEST_ERROR;
  exit(EXIT_SUCCESS);
}

void openipc(){
  msgid = msgget(getpid(), IPC_CREAT|0666);//mmsgqueue
  semid = semget(getpid(), 4, IPC_CREAT|0666);//sems set
  shrmem = shmget(getpid(), sizeof(struct shared_data), IPC_CREAT|0666);//shrmem area
  shrlett = shmget(getpid(), sizeof(int), IPC_CREAT|0666);//shrmem area 1
  lett = (int *)shmat(shrlett, NULL, 0);//shrmem1 attacched
  sh_data = (struct shared_data*)shmat(shrmem, NULL, 0);//shrmem attacched
  semctl(semid, 0, SETVAL, POP_SIZE);
  semctl(semid, 1, SETVAL, 1);
  semctl(semid, 2, SETVAL, 1);
  semctl(semid, 3, SETVAL, 1);
  TEST_ERROR;
}

void aexit(){
  sleep(2);
  for(i=0; i < POP_SIZE; ++i) printf("\tWaited:%d with score:%d\n", wait(&status), WEXITSTATUS(status));
  semctl(semid, 2, IPC_RMID);
  free(str);
  shmctl(shrmem, IPC_RMID , NULL);
  shmctl(shrlett, IPC_RMID , NULL);
  msgctl(msgid, IPC_RMID, NULL);
}

void signal_handler(int signalvalue){
  switch(signalvalue){
    case SIGUSR1:{
      printf("\thandle signal\n");
      break;
    }
  }
}
