#include "libs_structs.h"
#define POP_SIZE 20
int npref[3], prob[100] = {0};
int main(int argc, char *argv[]){
  init();
  i=0;
  j=ninvits;
  op.sem_num = 0;
  op.sem_op = 0;
  semop(semid, &op, 1);//waiting all
  while(1){
    while(msgrcv(mid, &msg, sizeof(msg)-sizeof(long), pid, IPC_NOWAIT) == sizeof(msg)-sizeof(long)){
      switch(msg.mode){
        case 1:{
          att(1);
          //printf("\nSono(%d)invitato da_%d\t", pid, msg.mpid);
          if(sh_data->gruppi[msg.mpid%POP_SIZE].chiuso == 1) refuse();
          else if(ninvits == 0 && sh_data->info[indx].accettato == 0){
            refuse();
          }
          else if(sh_data->info[indx].accettato == 0){
            if(nrejects==0) accept();
            else if(voto <= sh_data->info[msg.mpid%POP_SIZE].vote){
              accept();
            }else{
      		    refuse();
            }
          }else refuse();
          sig(1);
          break;
        }
        case 2:{
          att(1);
          if(sh_data->gruppi[indx].chiuso == 0){
            sh_data->info[indx].accettato = 1;
            if(sh_data->gruppi[indx].cont == 0){
              sh_data->gruppi[indx].compagni[sh_data->gruppi[indx].cont++] = pid;
              sh_data->gruppi[indx].voto = sh_data->info[indx].vote;
              sh_data->gruppi[indx].compagni[sh_data->gruppi[indx].cont++] = msg.mpid;
            }else if(sh_data->gruppi[indx].cont > 0 && sh_data->gruppi[indx].cont <= pref){
              sh_data->gruppi[indx].compagni[sh_data->gruppi[indx].cont++] = msg.mpid;
              if(sh_data->gruppi[indx].voto < sh_data->info[msg.mpid%POP_SIZE].vote)
                sh_data->gruppi[indx].voto = sh_data->info[msg.mpid%POP_SIZE].vote;
            }
            if(sh_data->gruppi[indx].cont == pref){
              sh_data->gruppi[indx].chiuso = 1;
              ninvits=0;
              if(msgsnd(msgid, &msg, sizeof(msg)-sizeof(long), 0) < 0) TEST_ERROR;
            }
          }
          sig(1);
          break;
        }
        default: break;
      }
    }
    msg.mpid = pid;
    att(1);
    while(ninvits>0 && (sh_data->info[indx].accettato == 0 || (sh_data->gruppi[indx].compagni[0] == pid && sh_data->gruppi[indx].chiuso == 0))){
      if(i==POP_SIZE) i = 0;
      if(pid%2 == sh_data->info[i].matr%2 && sh_data->info[i].accettato == 0 && sh_data->gruppi[i].compagni[0] == 0 && pid != sh_data->info[i].matr && sh_data->info[i].pref <= pref){
        msg.mtype = sh_data->info[i].matr;
        msg.mode = 1;
        ninvits--;
        if(msgsnd(mid, &msg, sizeof(msg)-sizeof(long), 0) < 0) TEST_ERROR;
      }
      i++;
      break;
	  }
    sig(1);
  }
  exit(EXIT_SUCCESS);

}

void init(){
  read_config();
  srand(pid = getpid());
  openipc();
  pref = prob[rand()%100];
  voto = rand()%13+18;
  indx = pid%POP_SIZE;
  att(0);
  att(1);
  sh_data->info[indx].matr = pid;
  sh_data->info[indx].vote = voto;
  sh_data->info[indx].pref = pref;
  sh_data->info[indx].accettato = 0;
  printf("\tStudente #%d\tmatricola %d\tvoto %d\tpreferenza %d\n", indx, pid, voto, pref);
  sig(1);
  atexit(aexit);
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
  nrejects--;
  //printf("RIFIUTO(%d)\n", msg.mpid);
}
void accept(){
  sh_data->info[indx].accettato = 1;
  ninvits = 0;
  //printf("ACCETTO(%d)\n", msg.mpid);
  pidcapo = msg.mpid;
  msg.mode = 2;
  msg.mtype = msg.mpid;
  msg.mpid = pid;
  if(msgsnd(mid, &msg, sizeof(msg)-sizeof(long), 0) < 0) TEST_ERROR;
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
  if(pid%2){
    mid = msgget(K1, 0666);
  }else {
    mid = msgget(K0, 0666);
  }
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
      att(1);
      att(0);
      if(sh_data->gruppi[indx].chiuso == 0 && sh_data->gruppi[indx].compagni[0] == pid) sh_data->gruppi[indx].chiuso=1;
      //if(msgrcv(msgid, &msg, sizeof(msg)-sizeof(long), pid, 0) == sizeof(msg)-sizeof(long))
      exit(EXIT_SUCCESS);
    }
  }
}
void aexit(){
  op.sem_num = 0;
  op.sem_op = 0;
  semop(semid, &op, 1);
  if(pidcapo) printf("%d voto finale:%d\n", pid, sh_data->gruppi[pidcapo%POP_SIZE].voto);
  else printf("%d voto finale:BOCCIATO\n", pid);
  shmdt(sh_data);
}
