#include "main.h"
#include "timer.h"

void timer(int pipe_fds[]){
    close(pipe_fds[0]);  // Chiusura lettura, il figlio scrive
    msg m = {'t', 0};
    
    while(true){         
        usleep(1000000); // fa un minimo di sleep
        
        m = (msg){'t', 0};
        write(pipe_fds[1], &m, sizeof(m));       
    }
}
