#include "main.h"
#include "bullet.h"

void bullet(int pipe_fds[], int index){
    close(pipe_fds[0]);  // Chiusura lettura, il figlio scrive
    msg m = {'b', index};
    int c;
    int timer=0, death_time=HEIGHT*2;

    while(true){         
        usleep(370000); // fa un minimo di sleep
        
        if(timer>=death_time){
            _exit(0);
        }

        m = (msg){'b', index};
        write(pipe_fds[1], &m, sizeof(m));
        

        timer+=1;
    }
}
