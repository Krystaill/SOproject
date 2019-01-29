#include "libs_structs.h"

int npref[3], prob[100] = {0};
void read_config();
#define POP_SIZE 15

int main(int argc, char *argv[]){
  init();
  pid_t contatti[ninvits];
  op.sem_num = 0;
  op.sem_op = 0;
  semop(semid, &op, 1);//waiting all
  while(1){
    while(msgrcv(mid, &msg, sizeof(pid_t), 0, IPC_NOWAIT) == sizeof(pid_t)){
      switch(msg.mtype){
        case 1:{
          if(ninvits==0){
            refuse();
          }else if(nrejects==0 && sh_data->info[pid%POP_SIZE].accettato == 0){
            accept();
          }else if(sh_data->info[pid%POP_SIZE].accettato == 0){
            att(1);
            sh_data->lett++;
            if(sh_data->lett==1){
              att(2);
            }
            sig(1);
            if(sh_data->info[msg.mint%POP_SIZE].vote > voto){
              accept();
            }else{
      		    refuse();
            }
            sh_data->lett--;
            att(1);
            if(sh_data->lett==0){
              sig(2);
            }
            sig(1);
          }else if(ninvits==0){
            refuse();
          }else refuse();
          break;
        }
        case 2:{
          att(2);
          att(1);
          for(i=0; i<POP_SIZE; ++i){
            if(msg.mint == sh_data->info[i].matr || pid == sh_data->info[i].matr){
              sh_data->info[i].accettato=1;
            }
          }
          sig(1);
          sig(2);
          msg.mtype = pid;
          msgsnd(msgid, &msg, sizeof(pid_t), 0); //scrivo al professore
          TEST_ERROR;
          break;
        }
        case 3:{
  		    printf("\tSono stato rifiutato(%d)\n", pid);
          break;
        }
      }
    }
    while(sh_data->info[indx].accettato == 0 && ninvits != 0){
      att(1);
      sh_data->lett++;
      if(sh_data->lett == 1){
        att(2);
      }
      sig(1);
	    for(; i<ninvits; ++i){
        if(i == ninvits) break;
        if(pid%2 == sh_data->info[i].matr%2 && sh_data->info[i].accettato == 0){
          if(pref <= sh_data->info[i].pref){
          }
	      }
      }
      sh_data->lett--;
      att(1);
      if(sh_data->lett==0){
        sig(2);
      }
      sig(1);
	    msg.mtype=1;
      msg.mint=pid;
	    id = msgget(contatti[i], IPC_CREAT);
		  TEST_ERROR;
		  msgsnd(id, &msg, sizeof(pid_t), 0);
		  --ninvits;
	    }
  }
  exit(EXIT_SUCCESS);
}
void init(){
  read_config();
  openipc();
  srand(pid = getpid());
  pref = prob[rand()%100];
  voto = rand()%13+18;
  indx = pid%POP_SIZE;
  att(0);
  att(1);
  sh_data->info[indx].matr = pid;
  sh_data->info[indx].vote = voto;
  sh_data-> info[indx].pref = pref;
  sh_data-> info[indx].accettato = 0;
  //printf("\tStudente #%d\n\t\tmatricola %d\n\t\tvoto %d\n\t\tpreferenza %d\n", indx, pid, voto, pref);
  sig(1);
  atexit(aexit);
  TEST_ERROR;
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
void refuse(){
  id = msgget(msg.mint, IPC_CREAT);
  msg.mtype = 3;
  msgsnd(id, &msg, sizeof(pid_t), 0);
  nrejects--;
}
void accept(){
  id = msgget(msg.mint, IPC_CREAT);
  msg.mtype = 2;
  msg.mint = pid;
  msgsnd(id, &msg, sizeof(pid_t), 0);
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
  val = 2;
  cont = npref[j=0];
  for(i=0; i<100; ++i){
    if(i==cont){
      val++;
      cont += npref[++j];
    }
    prob[i]=val;
  }
  TEST_ERROR;
  fclose(f);
}
void openipc(){
  sa.sa_handler = &signal_handler;
  sigaction(SIGINT, &sa, &saold);
  msgid = msgget(getppid(), IPC_CREAT);//msgqueue
  TEST_ERROR;
  mid = msgget(pid, IPC_CREAT|0666);//mmsgqueue
  TEST_ERROR;
  semid = semget(getppid(), 0, IPC_CREAT);//sems set
  TEST_ERROR;
  shrmem = shmget(getppid(), 0, IPC_CREAT);//shrmem area
  TEST_ERROR;
  sh_data = (struct shared_data*)shmat(shrmem, NULL, 0);//shrmem attacched
  TEST_ERROR;
}
void signal_handler(signalvalue){
  switch (signalvalue){
    case SIGINT:{
        shmdt(sh_data);
        exit(EXIT_SUCCESS);
    }
  }
}
void aexit(){
  msgctl(mid, IPC_RMID, NULL);
}
