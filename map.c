#include "common.h"
#include "events.h"
#include "game.h"


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

int emptyTiles(Player_t player, Bomb_t *bombList)
{
    int count = 0;

    for (int y = 0; y < ARENA_HEIGHT; y++)
    {
        for (int x = 0; x < ARENA_WIDTH; x++)
        {
            // check if tile is empty
            if (arena[x][y] == TILE_EMPTY)
            {
                // check if player is present
                if ((player.x != x) && (player.y != y))
                {
                    // check if bomb present
                    if (isBombPresent(bombList, x, y) == 0)
                        count++;
                }
            }
        }
    }

    return count;
}

void clearArena()
{
    for(int i = 0; i < ARENA_HEIGHT; i++) // clear board it for good measure
    {
        for(int j = 0; j < ARENA_WIDTH; j++)
        {
            arena[j][i] = TILE_EMPTY;
        }
    }
}

void addBlocks(int num, Player_t *p)
{
    int freetiles = emptyTiles(*p, bombList);
    
    int randX, randY;
    int playerX = p->x;
    int playerY = p->y;

    // check for empty tiles
    if (freetiles < num)
        num = freetiles;

    while(num > 0)
    {
        randX = rand() % ARENA_WIDTH;
        randY = rand() % ARENA_HEIGHT;

        if(arena[randX][randY] == TILE_EMPTY) // if it's an empty square!
        {
            if(!((randX == playerX) && (randY == playerY))) // make sure the player isn't there
            {
                arena[randX][randY] = TILE_BLOCK;
                num--;
            }   
        }
    }
}

void checkDestructible(int x, int y, Bomb_t *bombList)
{
    extern int score;

    if(arena[x][y] == TILE_BLOCK)
    {
        arena[x][y] = TILE_EMPTY;
        score++;
    }
    else {
        for (Bomb_t* thisBomb = bombList; thisBomb != NULL; thisBomb = thisBomb->next)
        {
            if ((thisBomb->x == x) && (thisBomb->y == y) && (thisBomb->state == TICKING)) {
                thisBomb->state = EXPLODED;
                thisBomb->timer -= 3500 - (SDL_GetTicks() - thisBomb->timer);
            }
        }
    }

    // also need to damage the player!
}