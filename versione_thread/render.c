#include "main.h"
#include "render.h"
#include "timer.h"
#include "collisions.h"
#include "thread_manage.h"
#include <time.h>

void kill_Childs(pthread_t thread_ids[], int thread_index){
    for(int i=0;i<thread_index;i++){
        killProcess(thread_ids[i]);
    }
}

void renderBackGround(WINDOW* w){ // stampa i colori dello sfondo
    wattron(w, COLOR_PAIR(SIDEWALK));
    for(int i=2;i<=3;i++){
        mvwhline(w, HEIGHT-i, 1, ' ', WIDTH-2); // zona inferiore
    }
    wattroff(w, COLOR_PAIR(SIDEWALK));

    wattron(w, COLOR_PAIR(WATER));
    for(int i=1;i<=FLUSSI;i++){
        mvwhline(w, HEIGHT-3-i, 1, ' ', WIDTH-2); // fiume
    }
    wattroff(w, COLOR_PAIR(WATER));


    wattron(w, COLOR_PAIR(GRASS));
    for(int i=1;i<=6;i++){
        mvwhline(w, HEIGHT-3-FLUSSI-i, 1, ' ', WIDTH-2); // zona superiore
    }
    wattroff(w, COLOR_PAIR(GRASS));

}

void renderPlayer(pos player_pos, wchar_t player_sprite[], WINDOW* w){
    wattron(w, COLOR_PAIR(FROG));
    
    mvwprintw(w, player_pos.y, player_pos.x, "%ls",player_sprite); 

    wattroff(w, COLOR_PAIR(FROG));
}

void renderCrocodiles(crocodile_info crocodiles[], WINDOW* w){
    wchar_t *crocodile_sprite;  //sprite base
    wchar_t *render_sprite; //sprite modificato in base ai bordi
    
    for(int z=0;z<NUM_CROCODILE;z++){
        if(crocodiles[z].direction==0) // in base alla direzione del cocodrillo cambia il suo sprite
            crocodile_sprite=(wchar_t*){L"◀█████████████████:▶"};
        else
            crocodile_sprite=(wchar_t*){L"◀:█████████████████▶"};
        int len_sprite = wcslen(crocodile_sprite);
        render_sprite = malloc(len_sprite*sizeof(wchar_t)+8);

        int temp_x = crocodiles[z].position.x;
        
        // faccio i controlli per i bordi
        bool sliding = false;
        

        if(crocodiles[z].position.x<=2 && crocodiles[z].size == len_sprite){ //primo controllo se il cocodrillo è oltre la bandiera sinistra
            int i = (len_sprite+crocodiles[z].position.x);
            
            if(i>0&&i<=len_sprite)
            {
                if(temp_x<0) temp_x=0;
                //copio il primo array nel secondo, partendo dall'ultima cella meno i (i indica quanto il cocodrillo si sta "affaciando" da sinistra)
                memcpy(render_sprite, crocodile_sprite+len_sprite-i, (i+1) * sizeof(wchar_t));
                sliding = true; 
            }
        }
        if(!sliding && crocodiles[z].size == len_sprite){ // se non sta già apparendo e se non sta affondando
            if(temp_x+len_sprite-1 >= WIDTH){ // secondo controllo se il cocodrillo è oltre la bandiera destra
                int i=((temp_x+len_sprite-1)-WIDTH)+1; // distanza di quanto si è sorpassato il muro
                if(i >= 0 && i <= len_sprite){ // controllino per verificare che i reintri nella grandezza dello sprite
                    memcpy(render_sprite, crocodile_sprite, (len_sprite-i)  * sizeof(wchar_t)); //copio il primo array nel seconod partendo dall'inizio ed eliminando la fine quanto è grande i
                    *(render_sprite+(len_sprite-i)-1)='\0'; // metto uno \0 alla fine perchè sennò non sa quando fermarsi a printare xD
                    sliding = true;
                }
            }else{ // se non si trova semplicemente si copia l'intero sprite originale nel render array
                memcpy(render_sprite, crocodile_sprite, len_sprite * sizeof(wchar_t));
            }
        }

        if(crocodiles[z].sinking && !sliding){ //il cocodrillo sta affondando
            if(crocodiles[z].position.x<-17 || crocodiles[z].position.x>WIDTH) // se il cocdrillo si trova in un bordo e quindi sta apparendo/scomparendo non sprofonda
            {
                crocodiles[z].size=len_sprite;
                crocodiles[z].sinking = false;
                continue;
            }
            else{                
                len_sprite = wcslen(crocodile_sprite);
                render_sprite = malloc(len_sprite*sizeof(wchar_t)+8);
                
                if(crocodiles[z].size > 1){
                    if(crocodiles[z].direction==0){
                        memcpy(render_sprite, crocodile_sprite, (crocodiles[z].size)  * sizeof(wchar_t)); 
                    }   
                    else{
                        memcpy(render_sprite, crocodile_sprite+(len_sprite-crocodiles[z].size), (crocodiles[z].size) * sizeof(wchar_t));    
                    }
                    crocodiles[z].size-=1;
                }else{
                    memcpy(render_sprite, crocodile_sprite, (0)  * sizeof(wchar_t)); 
                }
            }
        }
        
        if(crocodiles[z].bad_crocodile)
            wattron(w, COLOR_PAIR(CROCODILE_BAD));
        else
            wattron(w, COLOR_PAIR(CROCODILE_GOOD));

        mvwprintw(w, crocodiles[z].position.y, temp_x, "%ls",render_sprite);
        
        if(crocodiles[z].bad_crocodile)
            wattroff(w, COLOR_PAIR(CROCODILE_BAD));
        else
            wattroff(w, COLOR_PAIR(CROCODILE_GOOD));

        free(render_sprite);
    }
}

void renderBullets(bullet_ bullets[], crocodile_info crocodiles[], pos tane[], WINDOW* w){
    wchar_t *sprite=(wchar_t*){L"↧"};
    for(int i=0;i<NUM_BULLETS;i++){
        if(bullets[i].direction != -1 && bullets[i].position.y >= OFFSET_Y+2 && bullets[i].position.y <= HEIGHT+5){
        // se il proiettile è stato sparato, e si trova all'interno della scena, lo renderizzo
            if(bullets[i].direction == 0)
                sprite=(wchar_t*){L"↥"};
            else
                sprite=(wchar_t*){L"↧"};

            if(bullets[i].position.y>HEIGHT-4){ //marciapiede
                init_pair(BULLET+i, GRIGIO_SCURO, GRIGIO_CHIARO); 
            }
            else if(bullets[i].position.y<HEIGHT-3-FLUSSI){ //sponda sopra
                init_pair(BULLET+i, GRIGIO_SCURO, VERDE);  
                if(bullets[i].position.y==3 || bullets[i].position.y==4){
                    for(int h = 0; h<NUMBER_DOORS; h++){ // controllo se il proiettile sta passando sopra una tana
                        if(bullets[i].position.x >= tane[h].x && bullets[i].position.x <= tane[h].x+7)
                        {
                            init_pair(BULLET+i, GRIGIO_SCURO, COLOR_BLACK);
                        }
                    }
                }

            }
            else{
                int index = index_croocodile_collision(&bullets[i].position, crocodiles);
                if(index != -1 && crocodiles[index].size > 2){ // controllo se il proiettile sta passando sopra un cocodrillo
                    if(!crocodiles[index].bad_crocodile )
                        init_pair(BULLET+i, GRIGIO_SCURO, VERDE_SCURO);
                    else
                        init_pair(BULLET+i, GRIGIO_SCURO, VERDE_DARK);
                }
                else
                    init_pair(BULLET+i, GRIGIO_SCURO, COLOR_BLUE);
            }

            wattron(w, COLOR_PAIR(BULLET+i));
            mvwprintw(w, bullets[i].position.y, bullets[i].position.x, "%ls",sprite);
            wattroff(w, COLOR_PAIR(BULLET+i));
        }
    }

}


int playerAboveCroocodile(pos* player_pos, crocodile_info crocodiles[]){
    int index_crocodile=index_croocodile_collision(player_pos, crocodiles);    

    int speed = crocodiles[index_crocodile].velocity; 
    if(crocodiles[index_crocodile].direction == 0)
        player_pos->x+=speed;
    else
        player_pos->x-=speed;
    return index_crocodile;
}

int changePlayerPosition(pos* player_pos, int *index, crocodile_info crocodiles[], pos tane[], pos bushes[]){
    bool outOfBound=collisionPlayer(player_pos); 
    int status = -1;
    status = playerEnterDoor(player_pos, tane);
    if(status != -1)
        return status;

    bool dead = false;
    
    if(*index != -1){  
        if(crocodiles[*index].size != 20)
            dead = true;
    }
    if(*index==-1 || outOfBound || dead){ // controllo morte
        if((HEIGHT-4)-player_pos->y >= 0 && (HEIGHT-4)-player_pos->y < FLUSSI){ //se il player si trova nel fiume
            status = 0;
        }
    }
    if(player_pos->y<HEIGHT-3-FLUSSI){ // controllo se il player collide con un cespuglio oppure va troppo in alto
        if(player_pos->y<=OFFSET_Y+2){
            status = 0;
        }
        for(int i=0;i<NUM_BUSHES;i++){
            if(player_pos->y==bushes[i].y){
                if((player_pos->x>bushes[i].x && player_pos->x<bushes[i].x+3)
                || (player_pos->x<bushes[i].x && player_pos->x+6>bushes[i].x))
                    status = 0;
            }
        }
    }

    if(player_pos->y>HEIGHT-4){ //marciapiede
        init_pair(FROG, GIALLO, GRIGIO_CHIARO); 
    }
    else if(player_pos->y<HEIGHT-3-FLUSSI){ //sponda sopra
        init_pair(FROG, GIALLO, VERDE); 
    }
    else{
        if(*index!=-1){
            if(!crocodiles[*index].bad_crocodile)
                init_pair(FROG, GIALLO, VERDE_SCURO); //fiume su cocdrillo buono
            else
                init_pair(FROG, GIALLO, VERDE_DARK); //fiume su cocdrillo buono
        }
        
    }
    
    return status;
}

void playerMoved(msg m, pos* player_pos){
    switch(m.m) {
        case 0: player_pos->y+=1; break; 
        case 1: player_pos->y-=1; break; 
        case 2: player_pos->x-=3; break; 
        case 3: player_pos->x+=3; break;
    }
    
}

void initialize_sprite_crocodiles(crocodile_info crocodiles[]){
    for(int i=0;i<NUM_CROCODILE;i++){
        crocodiles[i].position=(pos){-100,-100};
        crocodiles[i].crocodile_sprite=(wchar_t*){L"◀█████████████████:▶"};
        crocodiles[i].sinking = false;
        crocodiles[i].size = wcslen(crocodiles[i].crocodile_sprite);
    }
}

void initialize_bullets(bullet_ bullets[]){
    for(int i=0;i<NUM_BULLETS;i++){
        bullets[i].direction=-1;
        bullets[i].position=(pos){0, 0};
    }
}

void move_bullets(bullet_ bullets[]){
    for(int i=0;i<NUM_BULLETS;i++){
        if(bullets[i].direction==0)
            bullets[i].position=(pos){bullets[i].position.x, bullets[i].position.y-1};
        else if(bullets[i].direction==1){
            bullets[i].position=(pos){bullets[i].position.x, bullets[i].position.y+1};
        }
    }
}

void initializeBushes(pos bushes[], pthread_t thread_ids[], int *thread_index){
    pthread_t thread_id;
    int h;
    for(int i=0;i<NUM_BUSHES;i++){
        do{
            bushes[i]=(pos){RAND_RANGE(1, WIDTH-5), RAND_RANGE(5, 7)};
        }while(collisionBushes(bushes, i)); // controllo che il nuovo cespuglio non spawni sopra un altro cespuglio
        
        h=i;
        pthread_create(&thread_id, NULL, &bush, &h); // spawno il nuovo cespuglio
        
        thread_ids[*thread_index] = thread_id;
        *thread_index+=1;
        if(*thread_index >= NUM_THREADS)
            *thread_index=0;
    }
}

void initializeBushe_Index(pos bushes[], int index){
    do{
        bushes[index]=(pos){RAND_RANGE(1, WIDTH-5), RAND_RANGE(5, 7)};
    }while(collisionBushes(bushes, index));
}
 
void renderBushes(pos bushes[], WINDOW *w){
    for(int i=0;i<NUM_BUSHES;i++){
        wattron(w, COLOR_PAIR(BUSH));
        mvwprintw(w, bushes[i].y, bushes[i].x, "⚘⚘⚘");
        wattroff(w, COLOR_PAIR(BUSH));
    }
}

void renderTane(pos tane[], WINDOW* w){
    for(int i=0;i<5;i++){
        wattron(w, COLOR_PAIR(TANA));
        
        mvwprintw(w, tane[i].y, tane[i].x, "🭁█████🭌");
        mvwprintw(w, tane[i].y+1, tane[i].x, "▀▀▀▀▀▀▀");
        
        wattroff(w, COLOR_PAIR(TANA));
    }
}

void initialize_tane(pos tane[], int doors[]){
    for(int i=0;i<5;i++){
        if(doors[i]==0){
            tane[i].y = 3;
            tane[i].x = (10*(i))+4;
        }else{
            tane[i].y = -10;
            tane[i].x = -10;
        }
    }
}

void renderHearts(int heart, WINDOW* w){
    for(int i=0;i<heart;i++){
        wattron(w, COLOR_PAIR(HEART));
        
        mvwprintw(w, 1, WIDTH-3-(i*2), "♥");
        
        wattroff(w, COLOR_PAIR(HEART));
    }
}

void renderHeader(game_info gi, WINDOW* w){
    renderHearts(gi.health, w);
    
    mvwprintw(w, 1, CENTER_X, "SCORE: %d", gi.score);
    mvwprintw(w, 1, 3, "TIME: %d", gi.time);
}

// returna -1 se il gioco è finito, 0 se è il player è morto e da 1-5 se è entrato in una porta
int render(WINDOW* w, game_info* gi, int client_socket){
    pthread_t thread_ids[NUM_THREADS];
    int thread_index=0;
    pthread_t thread_id;

    msg m=(msg){0};

    wchar_t player_sprite[]={L"◂|^-^|▸"};
    pos player_pos={CENTER_X-1, HEIGHT-1};
    bool player_moving=false; // è true se il player si è spostato o se è sopra un cocodrillo e quel cocodrillo si è spostato
    int id_croocodile_above=-1; // id del cocodrillo sopra il quale è
    
    crocodile_info crocodiles[NUM_CROCODILE];
    initialize_sprite_crocodiles(crocodiles);
    
    bullet_ bullets[NUM_BULLETS];
    int index_bullets=0; //indica a quale index siamo arrivati (quindi la prossima cella dell'array libera)
    initialize_bullets(bullets);
    
    pos bushes[NUM_BUSHES]; // piante
    initializeBushes(bushes, thread_ids, &thread_index);

    pos tane[NUMBER_DOORS]; // tane
    initialize_tane(tane, (*gi).doors);

    pthread_create(&thread_id, NULL, &timer, NULL);
    
    thread_ids[thread_index] = thread_id;
    thread_index++;

    int time=0, timer=1, index_bushes=-1; int bush_respawn[NUM_BUSHES]={-1, -1, -1};

    wclear(w);
    int first_render=0;
    while(true){
        int a = recv(client_socket, &m, sizeof(m), 0);
        
        if(&m!=(msg*){0}){       
            if(a<=0){
                m = read_message();
            }
            if(m.t == 'P'){ // ha ricevuto un pid di un cocodrillo
                thread_ids[thread_index] = m.m;
                thread_index++;
                if(thread_index >= NUM_THREADS)
                    thread_index=0;
            }
            else if(m.t=='p'){ //messaggio da player
                playerMoved(m, &player_pos);
                player_moving = true;
            }
            else if(m.t=='c'){ //messaggio da cocodrillo
                if(m.sinking && crocodiles[m.m].bad_crocodile)
                    crocodiles[m.m].sinking = true;

                if(id_croocodile_above==m.m){
                    if(playerAboveCroocodile(&player_pos, crocodiles) != -1){    
                        player_moving=true;
                    }else{
                        id_croocodile_above=-1;
                    }
                }
                crocodiles[m.m].position = m.position;
                if(m.spawned){
                    crocodiles[m.m].direction = m.direzione;
                    crocodiles[m.m].velocity = m.velocity;
                    crocodiles[m.m].bad_crocodile = m.bad_crocodile;
                }
            }
            else if(m.t == 'b'){ // messaggio da proiettile
                if(bullets[m.m].direction==0){
                    bullets[m.m].position=(pos){bullets[m.m].position.x, bullets[m.m].position.y-1};
                }else{
                    bullets[m.m].position=(pos){bullets[m.m].position.x, bullets[m.m].position.y+1};
                }
                
                int collision = collisionBullets(bullets, player_pos, crocodiles, bushes, thread_ids, &thread_index); 
                if(collision == -2){ // è uguale a zeor quando il priettile colpisce il player
                    kill_Childs(thread_ids, thread_index);
                    return 0;
                }
                else if(collision == -1){ // proiettile del player colpisce cocodrillo cattivo
                    (*gi).score += 10;
                }
                else if(collision >= 0){ // proiettile del player colpisce pianta
                    time=0;
                    timer=RAND_RANGE(1, 5);
                    index_bushes++;
                    bush_respawn[index_bushes] = collision;
                    bushes[collision].y = -10;

                    (*gi).score += 100;
                }
            }
            else if(m.t == '>'){ //messaggio di sparo
                if(((m.m < NUM_BUSHES && m.direzione==1 && bushes[m.m].y!=-10) // con questi due if sto rimuovendo i casi in qui il cespuglio non sia ancora respawnato oppure l'id sia errato
                && (m.m < NUM_BUSHES && m.direzione == 1))
                || m.direzione == 0){
                    int i = index_bullets;
                    pthread_create(&thread_id, NULL, &bullet, &i);
                    
                    thread_ids[thread_index] = thread_id;
                    thread_index++;
                    if(thread_index >= NUM_THREADS)
                        thread_index=0;
                    
                    if(m.direzione==0){ //sparata da player (va verso l'alto)
                        bullets[index_bullets] = (bullet_){(pos){player_pos.x+3, player_pos.y-1}, m.direzione};
                    }else if(m.direzione == 1){
                        if(m.m < NUM_BUSHES)
                            bullets[index_bullets] = (bullet_){(pos){bushes[m.m].x+2, bushes[m.m].y+1}, m.direzione};
                    }
                    index_bullets++;
                    if(index_bullets>=NUM_BULLETS)
                        index_bullets=0;
                }
            }
            else if(m.t == 't'){
                if(index_bushes != -1){
                    time++;
                    if(time>=timer){
                        time=0;
                        for(int h=0;h<NUM_BUSHES;h++){
                            if(bush_respawn[h] != -1)
                            {
                                initializeBushe_Index(bushes, bush_respawn[h]);
                                bush_respawn[h] = -1;
                            }
                        }
                        index_bushes=-1;
                    }
                }
                (*gi).time--;
                if((*gi).time <= 0){
                    kill_Childs(thread_ids, thread_index);
                    return 0;
                }
            }
            else if(m.t == 'q'){
                kill_Childs(thread_ids, thread_index);
                return -1;
            }

            if(player_moving){
                player_moving=false;
                id_croocodile_above = index_croocodile_collision(&player_pos, crocodiles);
                int status = changePlayerPosition(&player_pos, &id_croocodile_above, crocodiles, tane, bushes);
                if(status >= 0){
                    kill_Childs(thread_ids, thread_index);
                    return status;
                }
            }
        }
        werase(w);

        renderBackGround(w);
        renderTane(tane, w);
        
        renderCrocodiles(crocodiles, w);
        renderPlayer(player_pos, player_sprite, w);
        renderBullets(bullets, crocodiles, tane, w);
        renderBushes(bushes, w);
        
        renderHeader(*gi, w);
        
        wattron(w, COLOR_PAIR(BACKGROUND));
        box(w, ACS_VLINE, ACS_HLINE); // bordo
        wattroff(w, COLOR_PAIR(BACKGROUND));

        wrefresh(w);
        if(first_render<10){ //questo serve per far fare dei wclear all'inizio per il seguente problema:
//se utilizzo solo wclear lo schermo fa dei frame neri ogni tanto a causa di clearok() che viene chiamata da wclear
//se utilizzo solo werase non renderizza perfettamente tutto lo spazio di gioco, con zone vuote avvolte
//quindi la soluzione che ho trovato è di utilizzare wclear un paio di volte all'inizio, e poi da allora utilizzare solo erase
//così ho l'intero campo da gioco renderizzato e non ho frame neri ogni tanto 
            wclear(w);
            first_render++;
        }
        m=(msg){0};
    }
    return -1;
}
