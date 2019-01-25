#include "libs_structs.h"

int npref[3], prob[100] = {0};
void read_config();
int main(int argc, char *argv[]){
  read_config();
  openipc();
  pid = getpid();
  srand(pid);
  msg.mtype = pid;
  pref = prob[rand()%100];
  msg.mint=pref;
  msgsnd(msgid, &msg, 32, 0);
  voto = rand()%13+18;
  att(0);
  att(1);
  sh_data->info[sh_data->i].matr = pid;
  sh_data->info[sh_data->i].vote = voto;
  sh_data-> info[sh_data->i].accettato=0;
  sh_data->i++;
  sig(1);
  op.sem_num = 0;
  op.sem_op = 0;
  semop(semid, &op, 1);//waiting all

  while(1){
    while(msgrcv(mid, &msg, sizeof(msg), 0, 0) != -1){
      if(sh_data->info[i].accettato == 0 && msg.mtype == 1){
        if(nrejects==0){
          //accetta();
          //goto label;
        }
        att(1);
        sh_data->lett++;
        if(sh_data->lett==1) {
          att(3);
        }
        sig(1);
        for(i=0; i<POP_SIZE; ++i){
          if(sh_data->info[i].matr%2 == msg.mint%2){
            if(sh_data->info[i].vote > voto){
              //accetto();
            }else {//rifiuto();
            }
          }
        }
        //label;
        (sh_data->lett)--;
        op.sem_num = 1;
        op.sem_op = -1;
        semop(semid, &op, 1);
        if(sh_data->lett==0){
          op.sem_num = 3;
          op.sem_op = -1;
          semop(semid, &op, 1);
        }
        op.sem_num = 1;
        op.sem_op = 1;
        semop(semid, &op, 1);
      }
    }
    if(msg.mtype == 2){
      op.sem_num = 2;
      op.sem_op = -1;
      semop(semid, &op, 1);
      op.sem_num = 1;
      op.sem_op = -1;
      semop(semid, &op, 1);
      for(i=0; i<POP_SIZE; ++i){
        if(msg.mint == sh_data->info[i].matr || pid == sh_data->info[i].matr){
          sh_data->info[i].accettato=1;
        }
      }
      op.sem_num = 1;
      op.sem_op = 1;
      semop(semid, &op, 1);
      op.sem_num = 2;
      op.sem_op = 1;
      semop(semid, &op, 1);
      msg.mtype = pid;
    }
    if(msg.mtype == 3){
      printf("\tSono stato rifiutato(%d)\n", pid);
    }
    if(sh_data->info[i].accettato == 1){
      //rifiuto();
    }
  }
  shmdt(sh_data);
  TEST_ERROR;
  exit(voto);
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

void read_config(){
  FILE* f = fopen("file.config", "r");
  int i, j = 0;
  char s[10];
  while(fscanf(f, "%s", s) != -1){
    if(i%2==1){
      if(i < 6){
        npref[j++] = atoi(s);
      }
    }
    if(i == 7) ninvits = atoi(s);
    if(i == 9) nrejects = atoi(s);
    ++i;
  }
  cont = npref[j];
  val = 2;
  cont = npref[j=0];
  for(i=0; i<100; ++i){
    if(i==cont){
        val++;
        cont += npref[++j];
      }
    prob[i]=val;
  }
  fclose(f);
}

void openipc(){
  msgid = msgget(getppid(), IPC_CREAT);//msgqueue
  mid = msgget(getpid(), IPC_CREAT|0666);//mmsgqueue
  semid = semget(getppid(), 0, IPC_CREAT);//sems set
  shrmem = shmget(getppid(), 0, IPC_CREAT);//shrmem area
  sh_data = (struct shared_data*)shmat(shrmem, NULL, 0);//shrmem attacched
}
