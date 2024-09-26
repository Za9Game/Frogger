#include "main.h"
#include "menu.h"


void creditMenu(WINDOW* w){
    wclear(w);
    box(w, ACS_VLINE, ACS_HLINE);
    
    mvwprintw(w, CENTER_Y-2, CENTER_X-12, "Created By Sanna Massimo"); 
    mvwprintw(w, CENTER_Y+5, CENTER_X-12, "Press any key to go back"); 
    
    wrefresh(w);
    flushinp();

    wgetch(w);
}

int mainMenu(WINDOW* w){
    nodelay(w, false);
    keypad(w, true);

    char c;
    
    int position = 0;
    while(true){
        wclear(w);
        box(w, ACS_VLINE, ACS_HLINE);
        
        mvwprintw(w, CENTER_Y-5, CENTER_X-4, "FROG GAME!"); 
        for(int i=0;i<NUM_SOTTO_MENU;i++){ // printa le varie opzioni del menu principale
            if(position == i){
                wattron(w, COLOR_PAIR(HOVER_TEXT));
            }else{
                wattron(w, COLOR_PAIR(NORMAL_TEXT));
            }
            
            int x = CENTER_X-(strlen(MENU_STRING[i])/2);
            mvwprintw(w, CENTER_Y-1+i, x, "%s", MENU_STRING[i]);
            
            if(position == i){
                wattroff(w, COLOR_PAIR(HOVER_TEXT));
            }else{
                wattroff(w, COLOR_PAIR(NORMAL_TEXT));
            }
        }
        c = wgetch(w);
        switch(c) {
            case 2: // freccia in basso
                position+=1; 
                if(position>=NUM_SOTTO_MENU)
                    position = 0;
                break; 
            case 3: // freccia in alto
                position-=1;
                if(position < 0)
                    position = NUM_SOTTO_MENU-1; 
                break; 
            case 5: // freccia destra
            case 10: // invio
                if(position == 0 || position == NUM_SOTTO_MENU-1){
                    wrefresh(w);
                    flushinp();
                    nodelay(w, true);
                    keypad(w, false);
                    
                    return position;

                }
                else{
                    switch (position)
                    {
                        case NUM_SOTTO_MENU-2:
                            creditMenu(w);
                            break;
                        
                        default:
                            return position;
                    }
                }
                break;
        }

        wrefresh(w);
        flushinp();
    }

    nodelay(w, true);
    keypad(w, false);
    return 0;
}

int end_menu(WINDOW* w, char game_text[], int score){
    nodelay(w, false);
    keypad(w, true);

    char c;
    
    while(true){
        wclear(w);
        box(w, ACS_VLINE, ACS_HLINE);
        
        mvwprintw(w, CENTER_Y-5, CENTER_X-4, "%s", game_text); 
        mvwprintw(w, CENTER_Y-3, CENTER_X-13, "Press R to go back to menu");
        mvwprintw(w, CENTER_Y-2, CENTER_X-9, "Press Q to go quit");
        mvwprintw(w, CENTER_Y+3, CENTER_X-4, "Score: %d", score);
        
        c = wgetch(w);
        switch(c) {
            case 114: // restart
                return 0;
            case 113: // quit
                return 1;
        }

        wrefresh(w);
        flushinp();
    }

    nodelay(w, true);
    keypad(w, false);
    return 0;
}