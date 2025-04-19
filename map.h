#ifndef MAP_H
#define MAP_H

void setupTiles(SDL_Rect t[], int num);
int loadMap(char *filename);
int pxToTileX(int n);
int pxToTileY(int n);

#endif