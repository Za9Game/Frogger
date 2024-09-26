#include "main.h"
#include "render.h"
#include "collisions.h"

int index_croocodile_collision(pos* position, crocodile_info crocodiles[]){
    int flusso = (HEIGHT-4)-position->y; //flusso nel quale si trova il player
    if(flusso>=0&&flusso<FLUSSI){
        flusso*=2;
        for(int i=0;i<CROCODILE_PER_FLUSSO;i++){ //controllo per ogni cocodrillo nel flusso del player se è sotto il player
            int index_crocodile=flusso+i;
            int lungh_=20;

            if(lungh_+crocodiles[index_crocodile].position.x >= 0 //controllo che il cocodrillo non sia troppo a sinistra
            && crocodiles[index_crocodile].position.x <= WIDTH // o troppo a destra
            && position->x+3>=crocodiles[index_crocodile].position.x  // poi controllo che il lato sinistro del player sia oltre il lato sinistro del cocodrillo
            && position->x+2<=lungh_+crocodiles[index_crocodile].position.x) // e che il lato destro del player sia a sinistra del cocodrillo
            {
                return index_crocodile;
            }
        }
    }
    return -1;
}

int collisionBullets(bullet_ bullets[], pos player, crocodile_info crocodiles[], pos bushes[], pthread_t thread_ids[], int *thread_index){
    for(int i=0;i<NUM_BULLETS-1;i++){
        if(bullets[i].direction != -1){
            for(int j=i+1;j<NUM_BULLETS;j++){
                if(bullets[j].direction != -1){
                    if(bullets[i].position.y == bullets[j].position.y
                    && bullets[i].position.x >= bullets[j].position.x-1
                    && bullets[i].position.x <= bullets[j].position.x+1){
                    // se i due proiettili si trovano sulle stesse cordinate, si autodistrugono entrambi
                        for(int h=j;h<NUM_BULLETS -1; h++){
                            bullets[h] = bullets[h+1];
                            bullets[h+1].direction = -1;
                        }
                        for(int h=i;h<NUM_BULLETS -1; h++){
                            bullets[h] = bullets[h+1];
                            bullets[h+1].direction = -1;
                        }
                        break;
                    }
                }
            }
        }
    }
    
    for(int i=0;i<NUM_BULLETS;i++){
        if(bullets[i].direction == 0){ // se il proiettile è sparato dal player
            // primo controllo è per collisione su cocodrilli
            int index= index_croocodile_collision(&bullets[i].position, crocodiles);
            if(index !=-1 && crocodiles[index].size == 20){ //colpisce il cocdrillo se è sopra un cocodrillo e il cocdrillo non è sott'acqua
                for(int h = i; h < NUM_BULLETS - 1; h++){
                    bullets[h] = bullets[h + 1]; // ok tolgo il proiettile shiftando tutti i proiettili a destra di sinistra di 1
                    bullets[h+1].direction = -1;
                }
                if(crocodiles[index].bad_crocodile){
                    crocodiles[index].bad_crocodile = false;
                    crocodiles[index].sinking = false;
                    
                    return -1;
                }
            }
            // secondo controllo collisione su piante

            for(int h=0;h<NUM_BUSHES;h++){
                if(bullets[i].position.y == bushes[h].y){
                    if(bullets[i].position.x>=bushes[h].x
                    && bullets[i].position.x<=bushes[h].x+3){
                        for(int z = i; z < NUM_BULLETS - 1; z++){
                            bullets[z] = bullets[z + 1]; // ok tolgo il proiettile shiftando tutti i proiettili a destra di sinistra di 1
                            bullets[z+1].direction = -1;
                        }
                        
                        return h;
                    }
                }
            }
        }
        else if(bullets[i].direction == 1){ // controllo se il proiettile è sparato da pianta
            if(bullets[i].position.y == player.y){
                if(bullets[i].position.x >= player.x && bullets[i].position.x < player.x+6){
                    return -2;
                }
            }
        }   
    }
    return -3;
}

bool collisionBushes(pos bushes[], int index){ // controllo se le cordinate x di una pianta index collidono con un'altra pianta
    for(int i=0;i<NUM_BUSHES;i++){
        if(i!=index){
            if((bushes[index].x>=bushes[i].x && bushes[index].x<=bushes[i].x+3)
            || (bushes[index].x<=bushes[i].x && bushes[index].x+3>=bushes[i].x))
            {//controllo che, a prescindere dall'altezza, nessuno sia allineato sopra l'altro
                return true;
            }
            
        }
    }
    return false;
}

bool collisionPlayer(pos* player_pos){
    bool res = false;
    if(player_pos->y>HEIGHT-2) {player_pos->y=HEIGHT-2; res=false;} 
    if(player_pos->y<OFFSET_Y+2) {player_pos->y=OFFSET_Y+2; res=false;} 
    if(player_pos->x<OFFSET_X-2) {player_pos->x=OFFSET_X-2; res=true;}
    if(player_pos->x>WIDTH-6) {player_pos->x=WIDTH-6; res=true;}
    return res;
}

int playerEnterDoor(pos *player_pos, pos tane[]){
    if(player_pos->y == tane[0].y || player_pos->y == tane[0].y+1){
        for(int i=0;i<NUMBER_DOORS;i++){
            if((player_pos->x>=tane[i].x && player_pos->x<=tane[i].x+3)
            || (player_pos->x<=tane[i].x && player_pos->x+3>=tane[i].x)){
                return i+1;
            }
        }
    }
    return -1;
}
