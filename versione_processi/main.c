#include "main.h"
#include "render.h"
#include "menu.h"
#include "player.h"
#include "crocodile.h"
#include "collisions.h"

void initialize();

void initializeColors();

void initialize_server();

int startGame(WINDOW* w);

int end_score = 0;

int server_socket;
int client_socket;
struct sockaddr_in server_addr;

int main(){
    srand(time(NULL));
    initialize(); // inizializzo impostazioni e colori
    initialize_server();
    WINDOW* w = newwin(HEIGHT, WIDTH, OFFSET_Y, OFFSET_X);;
        
    int menuChoice, gameStatus = 0;
    
    while(true){  
        menuChoice = mainMenu(w); // avvia main menu
        if(menuChoice == 0){
            gameStatus = startGame(w); // inizio gioco

            if(gameStatus == -1) // quit
                break;
            else if(gameStatus == 0){ // gameOver
                menuChoice = end_menu(w, "GAME OVER!", end_score);
                if(menuChoice == 1){ // quit
                    break;
                }  
            }
            else if(gameStatus == 1){ // win
                menuChoice = end_menu(w, "WINNN!", end_score);
                if(menuChoice == 1){ // quit
                    break;
                }    
            }
        }
        else if(menuChoice == NUM_SOTTO_MENU-1){ // quit
            break;
        }
    }
    
    close(server_socket);
    endwin();
    _exit(0);
    return 0;
}

void initialize(){
    setlocale(LC_ALL, ""); // permette di metter in output tutti i caratteri unicode
    initscr(); // inizializza ncurses e schermo
    noecho(); // non visualizza a schermo i caratteri che vengono premuti
    cbreak(); // disattiva il buffering quindi non bisogna premere enter per inviare l'input
    curs_set(0); // passando 0 nasconde il cursore, 1 lo mostra staticamente e 2 lo fa lampeggiare 
    initializeColors();
}

void initializeColors(){
    start_color(); // inizializza i colore
    
    init_color(9, 246, 363, 189); //VERDE
    init_color(10, 15, 330, 20); //VERDE SCURO
    init_color(11, 655, 655, 0); //GIALLO
    init_color(12, 400, 400, 400); //GRIGIO
    init_color(13, 250, 250, 250); //GRIGIO SCURO
    init_color(14, 1, 180, 5); //VERDE MOLTO SCURO
    
    init_pair(BACKGROUND, VERDE, COLOR_BLACK); // sfondo dietro e bordi
    init_pair(SIDEWALK, COLOR_YELLOW, GRIGIO_CHIARO); //SIDEWALK inizializzo questa copia colore (il primo attributo è l'id della copia, il secondo il colore carattere e l'ultimo il colore sfondo)
    init_pair(WATER, COLOR_BLUE, COLOR_BLUE);
    init_pair(CROCODILE_GOOD, VERDE_SCURO, COLOR_BLUE); 
    init_pair(CROCODILE_BAD, VERDE_DARK, COLOR_BLUE); 
    init_pair(FROG, GIALLO, GRIGIO_CHIARO);
    init_pair(GRASS, GIALLO, VERDE); 
    init_pair(BUSH, VERDE_DARK, VERDE); 
    init_pair(TANA, COLOR_BLACK, VERDE); 
    init_pair(HEART, COLOR_RED, COLOR_BLACK);
    init_pair(NORMAL_TEXT, COLOR_WHITE, COLOR_BLACK);
    init_pair(HOVER_TEXT, COLOR_BLACK, COLOR_WHITE);
    init_pair(BULLET, GRIGIO_SCURO, GRIGIO_CHIARO);    
}

void initialize_server(){
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // creo il socket
    if(server_socket<0)
        perror("socket() failed");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // indirizzo dev'essere quello locale
    server_addr.sin_port = htons(9090); // definisco la porta

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("bind() failed");
    }
    if(listen(server_socket, MAXPENDING)<0){
        perror("listen() failed");
    }
}

void killProcess(pid_t pid){
    int status;
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);
}

int startGame(WINDOW* w){
    int pipe_fds[2]; pid_t pid, pid2;

    if(pipe(pipe_fds) == -1) { // creo la pipe, ho un'unica pipe per l'intero gioco
        perror("Pipe call");
        exit(1);
    }

    game_info gi; // le informazioni sulla sessione di gioco
    for(int i=0;i<NUMBER_DOORS;i++)
        gi.doors[i]=0;
    gi.score = 0;
    gi.health = START_HEARTS;

    int status = 0, prev_score = 0, level = 0;
    while(true){
    // Chiede la creazione di un processo figlio
        pid=fork(); // prima fork per la creazione del player
        if(pid < 0) { perror("fork call"); _exit(2);}  // Check fork a buon fine
        else if (pid == 0) {  // Processo figlio 
            player(pipe_fds); 
        }
        else{
            pid2=fork(); // seconda fork per la craezione dello spawner di cocodrilli
            if(pid2 < 0){ perror("fork call"); _exit(2);}
            else if(pid2 == 0){
                spawnCrocodile(pipe_fds);
            }
            else{            
                client_socket = accept(server_socket, NULL, NULL);
                int flags=fcntl(client_socket, F_GETFL); // queste due righe di codice mi permettono che il server non si blocchi in attesa di ricevere messaggi
                fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

                gi.time = TIME_AMOUNT;
                prev_score = gi.score;
                status = render(pipe_fds, w, &gi, client_socket); // status rappresenta come si è conclusa la partita (rana morta oppure passato una porta)
                
                killProcess(pid); //uccido i processi creati
                killProcess(pid2); 
            }
        }
        if(status == -1) // quit
            return -1;
        else if(status == 0){ // lose life
            gi.health--;
            gi.score -= 500;
            if(gi.score < 0)
                gi.score = 0;
            
            if(gi.health <= 0){
                end_score = gi.score;

                return 0; // game over vite finite
            }
        }
        else if(status > 0 && status <= 5){ // next door
            level += 1;
            gi.doors[status-1] = 1;
            gi.score += (int)((((gi.score-prev_score+1)+100)*level*gi.time)/2);

            if(level >= NUMBER_DOORS)
            {
                end_score = gi.score;
                return 1; // VITTORIA
            } 
        }
    }

    return -1;
}