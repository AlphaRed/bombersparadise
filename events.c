#include"common.h"
#include "events.h"

int checkGameEvents(SDL_Event e, Player_t *p)
{
    if(win == 0)
    {
        if(e.type == SDL_EVENT_QUIT)
            return 0;
        else if(e.type == SDL_EVENT_KEY_DOWN)
        {
            switch(e.key.key)
            {
                case SDLK_ESCAPE:
                    return 0;
                    break;
                case SDLK_W:
                    p->y -= (1 * TILE_SIZE * TILE_SCALE);
                    p->lastDir = 3;
                    break;
                case SDLK_S:
                    p->y += (1 * TILE_SIZE * TILE_SCALE);
                    p->lastDir = 1;
                    break;
                case SDLK_A:
                    p->x -= (1 * TILE_SIZE * TILE_SCALE);
                    p->lastDir = 2;
                    break;
                case SDLK_D:
                    p->x += (1 * TILE_SIZE * TILE_SCALE);
                    p->lastDir = 4;
                    break;
                case SDLK_SPACE:
                    bombList = placeBomb(p, bombList);
                    break;
                case SDLK_RETURN:
                    //addBlocks(5, p);
                    break;
                default:
                    break;
            }
            return 1;    
        }
    }
    else
    {
        if(e.type == SDL_EVENT_QUIT)
            return 0;
        else if(e.type == SDL_EVENT_KEY_DOWN)
        {
            switch(e.key.key)
            {
                case SDLK_RETURN:
                    return 2;
                    break;
                default:
                    break;
            }
            return 1;
        }
    }

    return 1;
}

int checkMenuEvents(SDL_Event e, Cursor_t *c)
{
    if(e.type == SDL_EVENT_QUIT)
        return 0;
    else if(e.type == SDL_EVENT_KEY_DOWN)
    {
        switch(e.key.key)
        {
            case SDLK_RETURN:
                if(c->y == 255) // Start
                    return 2;
                else if(c->y == 305) // Exit
                    return 0;
                break;
            case SDLK_DOWN:
                c->y += 50;
                break;
            case SDLK_UP:
                c->y -= 50;
                break;
            default:
                break;
        }
        return 1;    
    }
    return 1;
}

void checkMCursorBounds(Cursor_t *c)
{
    // wraps the cursor around the menu
    if(c->y > 305) // 305 + 50
    {
        c->y = 255;
    }   
    else if(c->y < 255)
    {
        c->y = 305;
    }
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

void checkCollision(Player_t *p)
{
    int playerX = p->x / TILE_SIZE / TILE_SCALE;
    int playerY = (p->y - 2 * TILE_SIZE * TILE_SCALE) / TILE_SIZE / TILE_SCALE; // Arena is drawn two tiles down

    if(arena[playerX][playerY] == TILE_WALL || arena[playerX][playerY] == TILE_BLOCK)
    {
        switch (p->lastDir)
        {
            case 1:
                p->y -= 1 * TILE_SIZE * TILE_SCALE;
                break;
            case 2:
                p->x += 1 * TILE_SIZE * TILE_SCALE;
                break;
            case 3:
                p->y += 1 * TILE_SIZE * TILE_SCALE;
                break;
            case 4:
                p->x -= 1 * TILE_SIZE * TILE_SCALE;
                break;
            default:
                break;
        }
    }
    else if(arena[playerX][playerY] == TILE_EXIT)
    {
        win = 1;
    }
}

void addBlocks(int num, Player_t *p)
{
    int randX, randY;
    int playerX = p->x / TILE_SIZE / TILE_SCALE;
    int playerY = (p->y - 2 * TILE_SIZE * TILE_SCALE) / TILE_SIZE / TILE_SCALE;

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

Bomb_t *placeBomb(Player_t *p, Bomb_t *b)
{
    Bomb_t *newBomb = malloc(sizeof(struct Bomb_t));
    if(newBomb == NULL)
        printf("Error, malloc failed to create new bomb!\n");
    
    newBomb->imgIndex = 6;
    newBomb->x = p->x;
    newBomb->y = p->y;
    newBomb->timer = SDL_GetTicks();
    newBomb->exploded = 0;
    newBomb->next = b;
    return newBomb;
}

void bombTimers(Bomb_t *b)
{
    int currentTime = SDL_GetTicks();
    for(Bomb_t *p = b; p != NULL; p = p->next)
    {
        if(currentTime - p->timer > 3000) // 3 seconds for now
        {
            //printf("boom baby!\n");
            p->exploded = 1;
        }
    }
}

void bombExplode(Bomb_t *b)
{
    b->exploded = 1;
    b->imgIndex = 8;
}

void checkDestructible(int x, int y)
{
    extern int score;

    if(arena[x][y] == TILE_BLOCK)
    {
        arena[x][y] = TILE_EMPTY;
        score++;
    }
    // also need to damage the player!
}