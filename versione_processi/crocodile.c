#include "main.h"
#include "crocodile.h"

void crocodile(int pipe_fds[], pos start_pos, int id, int direzione, int fast);
void shuffle(int *arr, size_t n);

void spawnCrocodile(int pipe_fds[]){ // si occupa di gestire lo spawn dei cocodrilli
    close(pipe_fds[0]);

    pid_t pid;
    pos startPos = {-6, HEIGHT-4};
    int id=0, direzione;
    int flussi_direction[FLUSSI];
    int id_for_flusso[FLUSSI];
    int ordine_flussi[FLUSSI];
    int fast_flusso[FLUSSI];

    int random_velocitY=RAND_RANGE(0, 1);
    for(int i=0;i<FLUSSI;i++){
        flussi_direction[i] = RAND_RANGE(0, 1);//scelgo una direzoine random per ogni flusso
        fast_flusso[i] = (i+random_velocitY)%2; //i flussi si alteranano nella velocità
        ordine_flussi[i]=i;// array usato per disordinare l'ordine con qui vengono generati nei flussi
        id_for_flusso[i]=0; // uso sto array per riuscire a dare in ordine gli id ai cocodrilli
    }

    shuffle(ordine_flussi, FLUSSI); //faccio lo shuffle in modo da non spawnare prima il flusso più in baasso verso l'alto
    for(int h=0;h<CROCODILE_PER_FLUSSO;h++){
        for(int i=0;i<FLUSSI;i++){
            
            if(id_for_flusso[ordine_flussi[i]]==0){
                id=ordine_flussi[i]*2;
                id_for_flusso[ordine_flussi[i]]=1;
            }else{
                id=(ordine_flussi[i]*2)+1;
            }
            direzione = flussi_direction[ordine_flussi[i]];
            if(direzione == 0){
                startPos.x = -25;
            }else{
                startPos.x = WIDTH+10;
            }
            startPos.y = HEIGHT-4-ordine_flussi[i];

            pid=fork();
            if(pid < 0) { perror("fork call"); _exit(2);}  // Check fork a buon fine
            else if (pid == 0) {  // Processo figlio 
                crocodile(pipe_fds, startPos, id, direzione, fast_flusso[ordine_flussi[i]]); // genero cocodrillo
            }
            else{
                write(pipe_fds[1], &(msg){'P', pid}, sizeof(msg));
            }

            usleep(RAND_RANGE(350000, 550000));
        }
        usleep(4500000);
    }
    
    _exit(0);
}

void crocodile(int pipe_fds[], pos start_pos, int id, int direzione, int fast){
    close(pipe_fds[0]);

    bool spawned=true;
    int velocity=2;
    if(fast == 0) velocity = 2;
    else velocity = 3;
    
    bool bad_crocodile = false;
    if(RAND_RANGE(0,10)==-1)
        bad_crocodile = true;
    bool sinking = false;

    while(true){
        if(bad_crocodile && RAND_RANGE(0, 9)==0)
            sinking = true;

        if(direzione == 0){
            if((WIDTH-start_pos.x) <= 0){ // se il cocodrillo va oltre la barriera 
                start_pos.x = -25;
                usleep(SECOND/3);

                if(RAND_RANGE(0,9)<=3)
                    bad_crocodile = true;
                else
                    bad_crocodile = false;
                
                spawned = true;
            }
            start_pos.x += velocity;
        }
        else{
            if(start_pos.x <= -15){ // se il cocodrillo va oltre la barriera 
                start_pos.x = WIDTH+10;
                usleep(SECOND/3);

                if(RAND_RANGE(0,9)<=3)
                    bad_crocodile = true;
                else
                    bad_crocodile = false;

                spawned = true;
            }
            
            start_pos.x -= velocity;
        }
        
        write(pipe_fds[1], &(msg){'c', id, direzione, start_pos, spawned, velocity, bad_crocodile, sinking}, sizeof(msg));
        usleep(SECOND/1.5);

        sinking = false;
        spawned = false;
    }

}

void shuffle(int *arr, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        srand(time(NULL));
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = arr[j];
          arr[j] = arr[i];
          arr[i] = t;
        }
    }
}