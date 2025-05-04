#include "common.h"
#include "events.h"
#include "game.h"
#include "map.h"

int checkGameEvents(SDL_Event e, Player_t *p)
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

int checkWinEvents(SDL_Event e)
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
    return 1;
}

int checkGameOverEvents(SDL_Event e)
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
        game.state = WIN;
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
    if (isBombPresent(bombList, destX, destY))
        return 1;
    // check for mobs
    if (isMobPresent(mobList, destX, destY))
    {
        // also need to damage the player!
        if ((player->invulnerable == 0) && (player->x == destX) && (player->y == destY))
        {
            if (player->lives == 0)
            {
                //win = 1;    // TODO: gameover screen instead of win
                game.state = GAMEOVER;
            }
            else
            {
                player->lives--;
                player->invulnerable = INVULNERABLE_TIME;
            }
        }
    }

    return 0;   // no collisions found
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
    newBomb->state = TICKING;
    newBomb->next = b;
    return newBomb;
}