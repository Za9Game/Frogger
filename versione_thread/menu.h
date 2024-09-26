int mainMenu(WINDOW* w);
int end_menu(WINDOW* w, char game_text[], int score);

#define NUM_SOTTO_MENU 3

static const char *MENU_STRING[] = {
    "Star Game", "Credit","Quit"
};