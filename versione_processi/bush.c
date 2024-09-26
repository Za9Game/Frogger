#include "main.h"
#include "bush.h"

void bush(int pipe_fds[], int index){ // cespuglio, dopo tot di tempo spara
    close(pipe_fds[0]);  // Chiusura lettura, il figlio scrive
    msg m = {'>', index, 1};
    
    while(true){         
        usleep(RAND_RANGE(6500000, 10000000)); // fa un minimo di sleep
        
        write(pipe_fds[1], &m, sizeof(m));
    }
}
