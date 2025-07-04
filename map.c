#include "common.h"
#include "events.h"
#include "game.h"
#include "map.h"


// Setup map tiles to tile array
void setupTiles(SDL_Rect t[], int num)
{
    for(int i = 0; i < num; i++)
    {
        t[i].x = (i % 8) * TILE_SIZE; // four tiles per row
        t[i].y = (i / 8) * TILE_SIZE; // four tiles per column
        t[i].w = TILE_SIZE;
        t[i].h = TILE_SIZE;
    }
}

int loadMap(int levelNum)
{
    FILE* file = NULL;

    switch (levelNum)
    {
    case 1:
        file = fopen("levels/level1.txt", "r");
        break;
    case 2:
        file = fopen("levels/level2.txt", "r");
        break;
    case 3:
        file = fopen("levels/level3.txt", "r");
        break;
    case 4:
        file = fopen("levels/level4.txt", "r");
        break;
    case 5:
        file = fopen("levels/level5.txt", "r");
        break;
    case 6:
        file = fopen("levels/level6.txt", "r");
        break;
    case 7:
        file = fopen("levels/level7.txt", "r");
        break;
    case 8:
        file = fopen("levels/level8.txt", "r");
        break;
    case 9:
        file = fopen("levels/level9.txt", "r");
        break;
    case 10:
        file = fopen("levels/level10.txt", "r");
        break;
    default:
        break;
    }

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
    plyr->numBombs = 0;
    plyr->invulnerable = INVULNERABLE_TIME;
}

// Returns zero if tile is empty and one if non-empty.
int isEmptyTile(int x, int y) {
    // check if tile is empty
    if (arena[x][y] != TILE_EMPTY)
        return 1;

    // check if player is present
    if ((player.x == x) && (player.y == y))
        return 1;

    // check if bomb is present
    if (isBombPresent(bombList, x, y) == 1)
        return 1;

    // check if mob is present
    if (isMobPresent(mobList, x, y) == 1)
        return 1;

    return 0;
}

// Return number of empty tiles with no player or bombs present
int emptyTiles(Player_t player, Bomb_t *bombList)
{
    int count = 0;

    for (int y = 0; y < ARENA_HEIGHT; y++)
    {
        for (int x = 0; x < ARENA_WIDTH; x++)
        {
            // check if tile is empty
            if (isEmptyTile(x, y) == 0)
                count++;
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
    //int playerX = p->x;
    //int playerY = p->y;

    // check for empty tiles
    if (freetiles < num)
        num = freetiles;

    while(num > 0)
    {
        randX = rand() % ARENA_WIDTH;
        randY = rand() % ARENA_HEIGHT;

        if (isEmptyTile(randX, randY) == 0) {
            arena[randX][randY] = TILE_BLOCK;
            num--;
        }
    }
}

void checkDestructible(int x, int y, Bomb_t *bombList)
{

    if(arena[x][y] == TILE_BLOCK)
    {
        arena[x][y] = TILE_EMPTY;
        player.score++;
    }
    else {
        for (Bomb_t* thisBomb = bombList; thisBomb != NULL; thisBomb = thisBomb->next)
        {
            if ((thisBomb->x == x) && (thisBomb->y == y) && (thisBomb->state == TICKING)) {
                thisBomb->state = EXPLODED;
                thisBomb->timer -= 3500 - (SDL_GetTicks() - thisBomb->timer);
                player.numBombs--;
            }
        }
    }

    // check mobs
    for (Mob_t* thisMob = mobList; thisMob != NULL; thisMob = thisMob->next)
    {
        if ((thisMob->x == x) && (thisMob->y == y))
            thisMob->state = KILLED;
    }

    // also need to damage the player!
    if ((player.invulnerable == 0) && (player.x == x) && (player.y == y))
    {
        if (player.lives == 0)
        {
            //win = 1;    // TODO: gameover screen instead of win
            game.state = GAMEOVER;
        }
        else
        {
            player.lives--;
            player.invulnerable = INVULNERABLE_TIME;
    
        }
    }
}

void clearSpawn()
{
    arena[1][1] = TILE_EMPTY;
    arena[2][1] = TILE_EMPTY;
    arena[1][2] = TILE_EMPTY;
}