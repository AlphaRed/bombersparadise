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

void bombTimers(Bomb_t *list)
{
    int currentTime = SDL_GetTicks();
    for(Bomb_t *thisBomb = list; thisBomb != NULL; thisBomb = thisBomb->next)
    {
        if((thisBomb->state != EXPLODED) && (currentTime - thisBomb->timer > 3500)) // 3.5? seconds for now
        {
            thisBomb->state = EXPLODED;
        }
        else if (currentTime - thisBomb->timer >= 5000)
        {
            thisBomb->state = DEAD;
        }

    }
}

void checkExplosions(Bomb_t *list)
{
    for (Bomb_t* thisBomb = list; thisBomb != NULL; thisBomb = thisBomb->next)
    {
        if (thisBomb->state == EXPLODED)
        {
            // destroy bricks and other bombs around bomb
            if (arena[thisBomb->x][thisBomb->y - 1] != TILE_WALL) // above
                checkDestructible(thisBomb->x, thisBomb->y - 1, list);
            if (arena[thisBomb->x][thisBomb->y + 1] != TILE_WALL) // below
                checkDestructible(thisBomb->x, thisBomb->y + 1, list);
            if (arena[thisBomb->x - 1][thisBomb->y] != TILE_WALL) // left
                checkDestructible(thisBomb->x - 1, thisBomb->y, list);
            if (arena[thisBomb->x + 1][thisBomb->y] != TILE_WALL) // right
                checkDestructible(thisBomb->x + 1, thisBomb->y, list);
        }
    }
}

Bomb_t *clearBombs(Bomb_t *list) {
    Bomb_t *current = list;
    Bomb_t *previous = NULL;

    while (current != NULL) {
        if (current->state == DEAD) {
            if (previous == NULL) {
                player.numBombs--;
                free(current);
                return previous;
            }
            else {
                previous->next = current->next;
                player.numBombs--;
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

void moveMobs(Mob_t *list)
{
    for (Mob_t *thisMob = list; thisMob != NULL; thisMob = thisMob->next) {
        int destX = thisMob->x;
        int destY = thisMob->y;
        int deltaTime = SDL_GetTicks() - thisMob->lastMove;
        int revX = thisMob->x;
        int revY = thisMob->y;

        if (deltaTime > 2000) {
            switch (thisMob->dir)
            {
            case 1:     // moving north
                destY--;
                revY++;
                break;
            case 2:     // moving east
                destX++;
                revX--;
                break;
            case 3:     // moving south
                destY++;
                revY--;
                break;
            case 4:     // moving west
                destX--;
                revX++;
                break;
            default:
                break;
            }

            if (checkCollisionMob(thisMob, destX, destY) == 0)
            {
                thisMob->x = destX;
                thisMob->y = destY;
                thisMob->lastMove = SDL_GetTicks();
            }
            else if (checkCollisionMob(thisMob, revX, revY) == 1)
            {
                switch (thisMob->dir) {
                case 1:
                    thisMob->dir = 2;
                    break;
                case 2:
                    thisMob->dir = 3;
                    break;
                case 3:
                    thisMob->dir = 4;
                    break;
                case 4:
                    thisMob->dir = 1;
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch (thisMob->dir) {
                case 1:
                    thisMob->dir = 3;
                    break;
                case 2:
                    thisMob->dir = 4;
                    break;
                case 3:
                    thisMob->dir = 1;
                    break;
                case 4:
                    thisMob->dir = 2;
                    break;
                default:
                    break;
                }
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

Mob_t* addMob(Mob_t* list, int x, int y, int direction) {
    Mob_t* newMob = malloc(sizeof(struct Mob_t));
    if (newMob == NULL)
    {
        printf("Error, malloc failed to create new mob!\n");
        return NULL;
    }

    newMob->imgIndex = 16;
    newMob->x = x;
    newMob->y = y;
    newMob->dir = direction;
    newMob->lastMove = SDL_GetTicks();
    newMob->state = ALIVE;
    newMob->next = list;

    return newMob;
}

void loadMobs(int lvlNum) {
    switch (lvlNum) {
    case 1:
        mobList = addMob(mobList, 11, 3, 1);
        mobList = addMob(mobList, 5, 9, 2);
        break;
    case 2:
        mobList = addMob(mobList, 5, 2, 3);
        mobList = addMob(mobList, 1, 6, 1);
        mobList = addMob(mobList, 9, 7, 2);
        break;
    case 3:
        mobList = addMob(mobList, 10, 1, 4);
        mobList = addMob(mobList, 4, 9, 2);
        mobList = addMob(mobList, 7, 5, 2);
        mobList = addMob(mobList, 1, 6, 1);
        break;
    case 4:
        mobList = addMob(mobList, 5, 1, 4);
        mobList = addMob(mobList, 13, 5, 4);
        mobList = addMob(mobList, 5, 10, 1);
        mobList = addMob(mobList, 9, 6, 3);
        break;
    case 5:
        mobList = addMob(mobList, 9, 1, 4);
        mobList = addMob(mobList, 5, 8, 1);
        mobList = addMob(mobList, 2, 5, 2);
        mobList = addMob(mobList, 13, 4, 3);
        break;
    case 6:
        mobList = addMob(mobList, 4, 4, 4);
        mobList = addMob(mobList, 1, 11, 2);
        mobList = addMob(mobList, 9, 9, 4);
        mobList = addMob(mobList, 7, 2, 3);
        mobList = addMob(mobList, 10, 2, 3);
        break;
    case 7:
        mobList = addMob(mobList, 1, 8, 1);
        mobList = addMob(mobList, 5, 2, 3);
        mobList = addMob(mobList, 5, 5, 2);
        mobList = addMob(mobList, 7, 11, 4);
    case 8:
        mobList = addMob(mobList, 5, 1, 4);
        mobList = addMob(mobList, 5, 5, 2);
        mobList = addMob(mobList, 1, 11, 2);
        mobList = addMob(mobList, 9, 6, 1);
        mobList = addMob(mobList, 13, 1, 3);
    case 9:
        mobList = addMob(mobList, 13, 1, 4);
        mobList = addMob(mobList, 2, 5, 2);
        mobList = addMob(mobList, 13, 7, 4);
        mobList = addMob(mobList, 3, 11, 2);
        mobList = addMob(mobList, 1, 6, 1);
        mobList = addMob(mobList, 5, 2, 3);
        mobList = addMob(mobList, 9, 11, 1);
        mobList = addMob(mobList, 13, 11, 1);
    case 10:
        mobList = addMob(mobList, 1, 3, 2);
        mobList = addMob(mobList, 7, 7, 2);
        mobList = addMob(mobList, 1, 5, 3);
        mobList = addMob(mobList, 7, 11, 1);
        mobList = addMob(mobList, 9, 11, 1);
        break;
    default:
        break;
    }
}

Mob_t* clearMobs(Mob_t* list) {
    Mob_t* current = list;
    Mob_t* previous = NULL;

    while (current != NULL) {
        if (current->state == KILLED) {
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

int isMobPresent(Mob_t* list, int x, int y)
{
    for (Mob_t* thisMob = list; thisMob != NULL; thisMob = thisMob->next)
    {
        if ((thisMob->x == x) && (thisMob->y == y))
            return 1;
    }

    return 0;
}

void killMobs(Mob_t* list)
{
    for (Mob_t* thisMob = list; thisMob != NULL; thisMob = thisMob->next)
    {
        thisMob->state = KILLED;
    }
}

void initPlayer(Player_t *p)
{
    p->imgIndex = 5;
    p->x = 1;
    p->y = 1;
    p->moveDir = 0;
    p->lives = 3;
    p->invulnerable = 0;
    p->maxBombs = 1;
    p->numBombs = 0;
}

void initCursor(Cursor_t* c)
{
    c->imgIndex = 15;
    c->x = 5;
    c->y = 255;
}

void initGame(Game_t* g)
{
    g->state = MENU;
    g->level = 1;
    g->titleCardTimer = SDL_GetTicks();
}