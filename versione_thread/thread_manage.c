#include "main.h"
#include "thread_manage.h"

extern msg buffer[DIM_BUFFER];
extern pthread_mutex_t lock;
extern int buffer_counter;

void write_message(msg m){
    while(buffer_counter == DIM_BUFFER);
    
    pthread_mutex_lock(&lock);
    
    buffer[buffer_counter] = m;
    buffer_counter+=1;

    pthread_mutex_unlock(&lock);
}

msg read_message(){
    while(buffer_counter == 0);

    pthread_mutex_lock(&lock);
    
    buffer_counter-=1;
    msg m = buffer[buffer_counter];
    buffer[buffer_counter] = (msg){0};
    
    pthread_mutex_unlock(&lock);

    return m;
}