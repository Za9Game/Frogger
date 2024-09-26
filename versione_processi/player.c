#include "main.h"
#include "player.h"

int client_socket_;
struct sockaddr_in server_addr_;

void connect_server(){ // connessione con il server 
    client_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(client_socket_<0){
        perror("socket() failed");
    }

    server_addr_.sin_family = AF_INET;
    server_addr_.sin_addr.s_addr = INADDR_ANY;
    server_addr_.sin_port = htons(9090);

    if(connect(client_socket_, (struct sockaddr*) &server_addr_, sizeof(server_addr_))<0){
        perror("connection() failed");
    }
}

void player(){
    connect_server();
    keypad(stdscr, true);

    send(client_socket_, &((msg){'p', 0}), sizeof(msg), 0);
    int c;
    
    while(true){
        c = getch();
        if(258 <= c && c <= 261){            
            send(client_socket_, &((msg){'p', c%258}), sizeof(msg), 0); // ho semplicemente semplificato, prima nello switch si passava c modulato e in base a quello davo un valore ad un'altra variabile che poi mandavo, ora mando direttamente c 
        }
        else if(c=='q'){
            send(client_socket_, &((msg){'q', -1}), sizeof(msg), 0);
            
        }else if(c == 32){ // barra spaziatrice che tira proietile
            send(client_socket_, &((msg){'>', 0, 0}), sizeof(msg), 0);
        }
        
        flushinp();
        usleep(UDELAY); // fa un minimo di sleep
    }
    
    close(client_socket_);
}
