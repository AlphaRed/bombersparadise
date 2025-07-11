#ifndef MAP_H
#define MAP_H

void setupTiles(SDL_Rect t[], int num);
int loadMap(int levelNum);
int pxToTileX(int n);
int pxToTileY(int n);
void resetplayer(Player_t *plyr);
int isEmptyTile(int x, int y);
int emptyTiles(Player_t player, Bomb_t *bombList);
void clearArena();
void addBlocks(int num, Player_t *p);
void checkDestructible(int x, int y, Bomb_t *bombList);
void clearSpawn();

#endif