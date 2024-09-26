#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h> 
#include <sys/wait.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>
#include <wchar.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "bullet.h"
#include "bush.h"

#define LEN_ARRAY(x)  ((sizeof(x) / sizeof((x)[0]))-1)

#define UDELAY 235000
#define MSGSIZE 15
#define SECOND 1000000

#define OFFSET_X 3  
#define OFFSET_Y 0
#define WIDTH 55
#define HEIGHT 23
#define CENTER_X ((WIDTH-OFFSET_X)/2)
#define CENTER_Y ((HEIGHT-OFFSET_Y)/2)
                                                                                                                                                           
#define FLUSSI 12
#define CROCODILE_PER_FLUSSO 2
#define NUM_CROCODILE (FLUSSI*CROCODILE_PER_FLUSSO)

#define NUM_THREADS 1000
#define NUM_BULLETS 100
#define NUM_BUSHES 3
#define NUMBER_DOORS 5

#define START_HEARTS 6
#define TIME_AMOUNT 55

#define RAND_RANGE(min, max) rand() % (max + 1 - min) + min
#define RAND_FLOAT(value) (float)rand() / (RAND_MAX) + value

#define DIM_BUFFER 50

struct pos{
    int x;
    int y;
}typedef pos;

struct crocodile_info{
    pos position;
    int direction;
    wchar_t *crocodile_sprite;
    int velocity;
    bool bad_crocodile;
    bool sinking;
    int size;
}typedef crocodile_info;

struct bullet_{
    pos position;
    int direction;
}typedef bullet_;

struct game_info{
    int doors[NUMBER_DOORS];
    int health;
    int score;
    int time;
}typedef game_info;

struct msg{
    char t;
    long m;
    int direzione;
    pos position;
    bool spawned;
    int velocity;
    bool bad_crocodile;
    bool sinking;
}typedef msg;
 
enum Colors_Pair{
    BACKGROUND=1,
    SIDEWALK,
    WATER,
    CROCODILE_GOOD,
    CROCODILE_BAD,
    FROG,
    GRASS,
    BUSH,
    TANA,
    HEART,
    NORMAL_TEXT,
    HOVER_TEXT,
    BULLET=100 //dev'essere sempre ultimo
};
enum Colors_{
    VERDE=9,
    VERDE_SCURO,
    GIALLO,
    GRIGIO_CHIARO,
    GRIGIO_SCURO,
    VERDE_DARK
};

void killProcess(pthread_t thread_id);

#define MAXPENDING 1