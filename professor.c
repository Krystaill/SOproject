#include "libs_structs.h"
struct gruppo{
  pid_t capo;
  pid_t compagni[3];
  int voto;
};
struct gruppo gruppi[POP_SIZE];
int main(int argc, char const *argv[]){
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
  }
  TEST_ERROR;
  while(msgrcv(msgid, &msg, sizeof(msg), 0, 0) != -1){

  }
}

void openipc(){
  msgid = msgget(getpid(), IPC_CREAT|0666);//mmsgqueue
  semid = semget(getpid(), 4, IPC_CREAT|0666);//sems set
  shrmem = shmget(getpid(), sizeof(struct shared_data), IPC_CREAT|0666);//shrmem area
  sh_data = (struct shared_data*)shmat(shrmem, NULL, 0);//shrmem attacched
  semctl(semid, 0, SETVAL, POP_SIZE);
  semctl(semid, 1, SETVAL, 1);
  semctl(semid, 2, SETVAL, 1);
  semctl(semid, 3, SETVAL, 1);
  TEST_ERROR;
}

void aexit(){
  //for(i=0; i < POP_SIZE; ++i) printf("\tWaited:%d with score:%d\n", wait(&status), WEXITSTATUS(status));
  semctl(semid, 4, IPC_RMID);
  free(str);
  shmctl(shrmem, IPC_RMID , NULL);
  msgctl(msgid, IPC_RMID, NULL);
}

void signal_handler(int signalvalue){
  switch(signalvalue){
    case SIGALRM:{
      printf("\thandle alarm signal\n");
      for(i=0; i<POP_SIZE; ++i){
        kill(sh_data->info[i], SIGTERM);
        TEST_ERROR;
      }
      for(i=0; i<POP_SIZE; ++i){
        wait(&status);
        TEST_ERROR;
      }
      semctl(semid, 4, IPC_RMID);
      free(str);
      shmctl(shrmem, IPC_RMID , NULL);
      msgctl(msgid, IPC_RMID, NULL);
      printf("-_-_-\tEND_SIMULATION\t-_-_-\n");
      exit(EXIT_SUCCESS);
    }
    default: break;
  }
}
