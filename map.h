#ifndef MAP_H
#define MAP_H

void setupTiles(SDL_Rect t[], int num);
int loadMap(char *filename);
int pxToTileX(int n);
int pxToTileY(int n);
void resetplayer(Player_t *plyr);
int emptyTiles(Player_t player, Bomb_t *bombList);

#endif