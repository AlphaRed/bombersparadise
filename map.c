#include"common.h"

// Setup map tiles to tile array
void setupTiles(SDL_Rect t[], int num)
{
    for(int i = 0; i < num; i++)
    {
        t[i].x = (i % 4) * TILE_SIZE; // four tiles per row
        t[i].y = (i / 4) * TILE_SIZE; // four tiles per column
        t[i].w = TILE_SIZE;
        t[i].h = TILE_SIZE;
    }
}

int loadMap(char *filename)
{
    FILE *file = fopen(filename, "r");

    if(file == NULL)
    {
        printf("Error opening map file!\n");
        return 1;
    }
    
    for(int i = 0; i < ARENA_HEIGHT; i++)
    {
        for(int j = 0; j < ARENA_WIDTH; j++)
        {
            if(fscanf(file, "%d", &arena[j][i]) == EOF)
                return 1;
        }
    }
    fclose(file);
    return 0;
}

int pxToTileX(int n)
{
    return n / TILE_SIZE / TILE_SCALE;
}

int pxToTileY(int n)
{
    return (n - 2 * TILE_SIZE * TILE_SCALE) / TILE_SIZE / TILE_SCALE;
}

void resetplayer(Player_t *plyr)
{
    plyr->x = 1;
    plyr->y = 1;
    plyr->moveDir = 0;
}