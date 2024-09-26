#include "main.h"
#include "bullet.h"
#include "thread_manage.h"

void *bullet(void* index){
    int index_ = *((int *)index);
    int c;
    int timer=0, death_time=HEIGHT*2;

    while(true){         
        usleep(370000); // fa un minimo di sleep
        
        if(timer>=death_time){
            break;
        }
        
        write_message((msg){'b', index_});
        

        timer+=1;
    }
}
