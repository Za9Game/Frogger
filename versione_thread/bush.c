#include "main.h"
#include "bush.h"
#include "thread_manage.h"

void *bush(void *index){
    int index_ = *((int *)index);
    while(true){         
        usleep(RAND_RANGE(6500000, 10000000)); // fa un minimo di sleep
        
        write_message((msg){'>', index_, 1});    
    }
}
