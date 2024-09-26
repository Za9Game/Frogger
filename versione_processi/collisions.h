int collisionBullets(int pipe_fds[], bullet_ bullets[], pos player, crocodile_info crocodiles[], pos bushes[], pid_t pids[], int *pid_index);

int index_croocodile_collision(pos* position, crocodile_info crocodiles[]);

bool collisionBushes(pos bushes[], int index);

bool collisionPlayer(pos* player_pos);

int playerEnterDoor(pos *player_pos, pos tane[]);