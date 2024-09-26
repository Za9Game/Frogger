#include "main.h"
#include "timer.h"
#include "thread_manage.h"

void *timer(){
    while(true){         
        usleep(1000000); // fa un minimo di sleep
        
        write_message((msg){'t', 0});       
    }
}
