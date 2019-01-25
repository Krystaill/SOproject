#include "libs_structs.h"

int npref[3], prob[100] = {0};
void read_config();
int main(int argc, char *argv[]){
  read_config();
  openipc();
  pid = getpid();
  srand(pid);
  pref = prob[rand()%100];
  voto = rand()%13+18;
  att(0);
  att(1);
  sh_data->info[sh_data->i].matr = pid;
  sh_data->info[sh_data->i].vote = voto;
  sh_data-> info[sh_data->i].pref = pref;
  sh_data-> info[sh_data->i].accettato=0;
  sh_data->i++;
  sig(1);
  op.sem_num = 0;
  op.sem_op = 0;
  semop(semid, &op, 1);//waiting all
  int contatti[pref]={0};
  j=0;
  while(1){
	//ricevo
    while(msgrcv(mid, &msg, sizeof(msg), 0, 0) != -1){
      if(sh_data->info[i].accettato == 0 && msg.mtype == 1){
        if(nrejects==0){
          accept();
          goto label;
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
               accept();
            }else{
			   refuse();
            }
          }
        }
        (sh_data->lett)--;
        att(1);
        if(sh_data->lett==0){
          att(3);
        }
        sig(1);
      }
      label: if(msg.mtype == 2){
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
      msgsnd(msgid, &msg, sizeof(msg), 0); //scrivo al professore
      TEST_ERROR;
     }
     if(msg.mtype == 3){
		if(msg.mint == pid) printf("\tSono stato rifiutato(%d)\n", pid);
     }
     if(sh_data->info[i].accettato == 1){
       refuse();
     }
   }
   //invito
   while(sh_data->info[i].accettato == 0 && ninvites!=0){
	 for(i=0; i<POP_SIZE; ++i){
		if(pid%2 == sh_data->info[i].matr%2){
			if(pref<=sh_data->info[i].pref){
				contatti[j++]= sh_data-> info[sh_data->i].matr;
				if(j==invites) break;
			}
		}
	 }
	 msg.mtype=1;
     msg.mint=pid;
	 for(i=0; i<j; ++i){
		 id = msgget(contatti[i], IPC_CREAT);
		 TEST_ERROR;
		 msgsnd(id, &msg, sizeof(msg), 0);
		 --nvites;
	 }
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
  TEST_ERROR
}
void sig(x){
  op.sem_num = x;
  op.sem_op = 1;
  semop(semid, &op, 1);
  TEST_ERROR;
}
void refuse(){
  msg.mtype=3;
  msgsnd(msg.mint, &msg, sizeof(msg), 0);
  TEST_ERROR;
}
void accepted(){
  id = msg.mtype;
  msg.mtype = 2;
  msg.mint = pid;
  msgsnd(id, &msg, sizeof(msg), 0);
  TEST_ERROR;
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
  TEST_ERROR;
  fclose(f);
}

void openipc(){
  msgid = msgget(getppid(), IPC_CREAT);//msgqueue
  TEST_ERROR;
  mid = msgget(getpid(), IPC_CREAT|0666);//mmsgqueue
  TEST_ERROR;
  semid = semget(getppid(), 0, IPC_CREAT);//sems set
  TEST_ERROR;
  shrmem = shmget(getppid(), 0, IPC_CREAT);//shrmem area
  TEST_ERROR;
  sh_data = (struct shared_data*)shmat(shrmem, NULL, 0);//shrmem attacched
  TEST_ERROR;
}
