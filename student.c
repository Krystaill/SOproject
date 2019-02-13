#include "libs_structs.h"
int npref[3], prob[100] = {0};

int main(int argc, char *argv[]){
  init();
  att(0);
  i=0;
  op.sem_num = 0;
  op.sem_op = 0;
  semop(semid, &op, 1);//waiting all
  while(1){
    while(msgrcv(mid, &msg, sizeof(msg)-sizeof(long), pid, IPC_NOWAIT) == sizeof(msg)-sizeof(long)){
      switch(msg.mode){
        case 1:{
          att(1);
          printf("\nSono(%d)invitato da_%d\t", pid, msg.mpid);
          if(sh_data->gruppi[msg.mpid%POP_SIZE].chiuso == 1 || ninvits==0) refuse();
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
      if(pid%2 == sh_data->info[i].matr%2 && sh_data->info[i].accettato == 0 && sh_data->gruppi[i].compagni[0] == 0 && pid != sh_data->info[i].matr && sh_data->info[i].pref == pref){
        msg.mtype = sh_data->info[i].matr;
        msg.mode = 1;
        ninvits--;
        if(msgsnd(mid, &msg, sizeof(msg)-sizeof(long), 0) < 0) TEST_ERROR;
      }
      i++;
      if(i==POP_SIZE) i = 0;
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
  att(1);
  sh_data->info[indx].matr = pid;
  sh_data->info[indx].vote = voto;
  sh_data->info[indx].pref = pref;
  printf("\tStudente #%d\tmatricola %d\tvoto %d\tpreferenza %d\n", indx, pid, voto, pref);
  sh_data->info[indx].accettato = 0;
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
  printf("RIFIUTO(%d)\n", msg.mpid);
}
void accept(){
  ninvits = 0;
  sh_data->info[indx].accettato = 1;
  sh_data->gruppi[indx].chiuso = 1;
  pidcapo = msg.mpid;
  printf("ACCETTO(%d)\n", pidcapo);
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
  pref = 2;
  cont = npref[j=0];
  for(i=0; i<100; ++i){
    if(i==cont){
      pref++;
      cont += npref[++j];
    }
    prob[i]=pref;
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
      if(sh_data->gruppi[indx].chiuso == 0 && sh_data->gruppi[indx].compagni[0] == pid) sh_data->gruppi[indx].chiuso=1;
      exit(EXIT_SUCCESS);
    }
  }
}
void aexit(){
  op.sem_num = 0;
  op.sem_op = 0;
  semop(semid, &op, 1);
  for(i=0; i<POP_SIZE; i++){
    j=0;
    if(sh_data->gruppi[i].compagni[0] != 0){
      while(j<sh_data->gruppi[i].cont){
        if(sh_data->gruppi[i].compagni[j] == pid){
          if(sh_data->gruppi[i].voto>17){
            printf("Matricola %d il mio esito finale_%d\n", pid, sh_data->gruppi[i].voto);
          }else printf("Matricola %d il mio esito finale_BOCCIATO\n", pid);
        }
        j++;
      }
    }
  }
  printf("\n");
  shmdt(sh_data);
}
