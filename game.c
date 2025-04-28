#include "common.h"
#include "game.h"

#include "map.h"
#include "events.h"


// Bomb Functions

void bombExplode(Bomb_t *b)
{
    b->state = EXPLODED;
    b->imgIndex = 8;
}

void bombTimers(Bomb_t *bombList)
{
    int currentTime = SDL_GetTicks();
    for(Bomb_t *thisBomb = bombList; thisBomb != NULL; thisBomb = thisBomb->next)
    {
        if((thisBomb->state != EXPLODED) && (currentTime - thisBomb->timer > 3500)) // 3.5? seconds for now
        {
            thisBomb->state = EXPLODED;

            // destroy bricks and other bombs around bomb
            if (arena[thisBomb->x][thisBomb->y - 1] != TILE_WALL) // above
                checkDestructible(thisBomb->x, thisBomb->y - 1, bombList);
            if (arena[thisBomb->x][thisBomb->y + 1] != TILE_WALL) // below
                checkDestructible(thisBomb->x, thisBomb->y + 1, bombList);
            if (arena[thisBomb->x - 1][thisBomb->y] != TILE_WALL) // left
                checkDestructible(thisBomb->x - 1, thisBomb->y, bombList);
            if (arena[thisBomb->x + 1][thisBomb->y] != TILE_WALL) // right
                checkDestructible(thisBomb->x + 1, thisBomb->y, bombList);
        }
        else if (currentTime - thisBomb->timer >= 5000)
        {
            thisBomb->state = DEAD;
        }

    }
}

Bomb_t *clearBombs(Bomb_t *list) {
    Bomb_t *current = list;
    Bomb_t *previous = NULL;

    while (current != NULL) {
        if (current->state == DEAD) {
            if (previous == NULL) {
                free(current);
                return previous;
            }
            else {
                previous->next = current->next;
                free(current);
                current = previous->next;
            }
        }
        else {
            previous = current;
            current = current->next;
        }
    }
    return list;
}

// Check if an unexploded bomb is present in arena tile
int isBombPresent(Bomb_t *bombList, int x, int y)
{
    for (Bomb_t *thisBomb = bombList; thisBomb != NULL; thisBomb = thisBomb->next)
    {
        if ((thisBomb->x == x) && (thisBomb->y == y) && (thisBomb->state == TICKING))
            return 1;
    }

    return 0;
}

void printBombs(Bomb_t *list) {
    int num = 0;
    for (; list != NULL; list = list->next) {
        num++;
    }
    printf("There are %d bombs placed\n", num);
}


// Mob Functions

void moveMobs(Mob_t* mob)
{
    int destX = mob->x;
    int destY = mob->y;
    int deltaTime = SDL_GetTicks() - mob->lastMove;

    if (deltaTime > 2000) {
        switch (mob->dir)
        {
        case 1:     // moving north
            destY--;
            break;
        case 2:     // moving east
            destX++;
            break;
        case 3:     // moving south
            destY++;
            break;
        case 4:     // moving west
            destX--;
            break;
        default:
            break;
        }

        if (checkCollisionMob(mob, destX, destY) == 0)
        {
            mob->x = destX;
            mob->y = destY;
            mob->lastMove = SDL_GetTicks();
        }
        else
        {
            switch (mob->dir) {
            case 1:
                mob->dir = 3;
                break;
            case 2:
                mob->dir = 4;
                break;
            case 3:
                mob->dir = 1;
                break;
            case 4:
                mob->dir = 2;
                break;
            default:
                break;
            }
        }
    }
}

int checkCollisionMob(Mob_t* mob, int destX, int destY)
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

    return 0;   // no collisions found
}