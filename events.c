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
                    p->moveDir = 1;
                    break;
                case SDLK_S:
                    p->moveDir = 3;
                    break;
                case SDLK_A:
                    p->moveDir = 4;
                    break;
                case SDLK_D:
                    p->moveDir = 2;
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
            case SDLK_S:
                c->y += 50;
                break;
            case SDLK_UP:
            case SDLK_W:
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

void movePlayer(Player_t *player)
{
    int destX, destY;

    destX = player->x;
    destY = player->y;

    switch (player->moveDir)
    {
        case 1:     // moving up
            destY--;
            break;
        case 2:     // moving right
            destX++;
            break;
        case 3:     // moving down
            destY++;
            break;
        case 4:     // moving left
            destX--;
            break;
        default:
            break;
    }

    // check for collision
    if (checkCollision(player, destX, destY) == 0)
    {
        player->x = destX;
        player->y = destY;
        player->moveDir = 0;    // we've handled the movement
    }

    // check for win
    if (arena[player->x][player->y] == TILE_EXIT)
        win = 1;
}

int checkCollision(Player_t *player, int destX, int destY)
{
    int destTile = arena[destX][destY];
    // check for unpassable blocks
    if (destTile == TILE_BLOCK)
        return 1;
    if (destTile == TILE_WALL)
        return 1;
    
    // check for bombs
    //if (isBombPresent(bombList, destX, destY))
    //    return 1;

    return 0;   // no collisions found
}

void addBlocks(int num, Player_t *p)
{
    int randX, randY;
    int playerX = p->x;
    int playerY = p->y;

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
    if (newBomb == NULL)
    {
        printf("Error, malloc failed to create new bomb!\n");
        return NULL;
    }
    
    newBomb->imgIndex = 6;
    newBomb->x = p->x;
    newBomb->y = p->y;
    newBomb->timer = SDL_GetTicks();
    newBomb->exploded = 0;
    newBomb->next = b;
    return newBomb;
}

void bombTimers(Bomb_t *bombList)
{
    int currentTime = SDL_GetTicks();
    for(Bomb_t *thisBomb = bombList; thisBomb != NULL; thisBomb = thisBomb->next)
    {
        if((!thisBomb->exploded) && (currentTime - thisBomb->timer > 3500)) // 3.5? seconds for now
        {
            thisBomb->exploded = 1;

            // destroy bricks around bomb
            if (arena[thisBomb->x][thisBomb->y - 1] != TILE_WALL) // above
                checkDestructible(thisBomb->x, thisBomb->y - 1);
            if (arena[thisBomb->x][thisBomb->y + 1] != TILE_WALL) // below
                checkDestructible(thisBomb->x, thisBomb->y + 1);
            if (arena[thisBomb->x - 1][thisBomb->y] != TILE_WALL) // left
                checkDestructible(thisBomb->x - 1, thisBomb->y);
            if (arena[thisBomb->x + 1][thisBomb->y] != TILE_WALL) // right
                checkDestructible(thisBomb->x + 1, thisBomb->y);
        }
        else if (currentTime - thisBomb->timer >= 5000)
        {
            // TODO destroy bomb entry in linked list
            thisBomb->exploded = 2;
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

int isBombPresent(Bomb_t *bombList, int x, int y)
{
    for (Bomb_t *thisBomb = bombList; thisBomb != NULL; thisBomb = thisBomb->next)
    {
        if ((thisBomb->x == x) && (thisBomb->y == y))
            return 1;
    }

    return 0;
}