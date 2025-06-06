#include "common.h"
#include "game.h"

#include "map.h"
#include "events.h"


// Bomb Functions

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
    int ticks = SDL_GetTicks();

    for (Bomb_t* thisBomb = list; thisBomb != NULL; thisBomb = thisBomb->next) {
        if ((thisBomb->state == EXPLODED) && (ticks - thisBomb->timer < 4500)) {
            checkDestructible(thisBomb->x, thisBomb->y, list);
            bombShockwave(thisBomb->x, thisBomb->y, 1, 0);
            bombShockwave(thisBomb->x, thisBomb->y, 0, 1);
            bombShockwave(thisBomb->x, thisBomb->y, -1, 0);
            bombShockwave(thisBomb->x, thisBomb->y, 0, -1);
        }
    }
}

// Handles logic for a single shockwave from the bomb in a single direction
void bombShockwave(int startx, int starty, int dx, int dy) {
    int x = startx + dx;
    int y = starty + dy;

    for (int length = player.range; length > 0; length--) {
        // is tile a wall?
        if (arena[x][y] == TILE_WALL)
            return; // dealing with shockwave ends

        // is tile a destructable block?
        if (arena[x][y] == TILE_BLOCK) {
            arena[x][y] = TILE_WRECK;
            wreckingList = addWreck(wreckingList, x, y, SDL_GetTicks());
            score++;
            return; // dealing with shockwave ends
        }

        // is tile a breaking block?
        if (arena[x][y] == TILE_WRECK)
            return;

        // is the player on it?
        if ((player.invulnerable == 0) && (player.x == x) && (player.y == y)) {
            if (player.lives == 0)
                game.state = GAMEOVER;
            else {
                player.lives--;
                player.invulnerable = INVULNERABLE_TIME;
            }
        }

        // is an enemy on it?
        for (Mob_t *thisMob = mobList; thisMob != NULL; thisMob = thisMob->next) {
            if ((thisMob->x == x) && (thisMob->y == y)) {
                thisMob->state = KILLED;
                score += 2;
            }
        }

        // is there a bomb on it?
        for (Bomb_t *thisBomb = bombList; thisBomb != NULL; thisBomb = thisBomb->next) {
            if ((thisBomb->x == x) && (thisBomb->y == y) && (thisBomb->state == TICKING)) {
                thisBomb->state = EXPLODED;
                thisBomb->timer -= 3500 - (SDL_GetTicks() - thisBomb->timer);
                player.numBombs--;
            }
        }

        // remove powerup if one exists on tile
        powerupList = removePowerup(powerupList, x, y);

        x += dx;
        y += dy;
    }
}

Bomb_t *deleteBombs(Bomb_t *list) {
    Bomb_t *temp;

    // check if list is already empty
    if (list == NULL)
        return list;
    
    while (list != NULL) {
        temp = list->next;
        free(list);
        list = temp;
    }
    return list; // should be NULL
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

// Check if an unexploded bomb is present on arena tile
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
    switch (destTile) {
        // impassable blocks
        case TILE_BLOCK:
        case TILE_WALL:
        case TILE_WRECK:
            return 1;
            break;
        default:
            break;
    }
    // check for bombs
    if (isBombPresent(bombList, destX, destY))
        return 1;
    //check for other mobs
    if (isMobPresent(mobList, destX, destY))
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

    newMob->imgIndex = TILE_MOB;
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
                previous = current->next;
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


// Powerup Functions

Powerup_t* addPowerup(Powerup_t* list, int x, int y, Poweruptype type) {
    Powerup_t* newPowerup = malloc(sizeof(struct Powerup_t));
    if (newPowerup == NULL) {
        printf("Error, malloc failed to create new powerup!\n");
        return NULL;
    }

    newPowerup->type = type;
    newPowerup->x = x;
    newPowerup->y = y;
    newPowerup->next = list;

    return newPowerup;
}

// remove power up at specific x, y
Powerup_t *removePowerup(Powerup_t *list, int x, int y) {
    Powerup_t *previous = NULL;
    Powerup_t *current = list;

    while (current != NULL) {
        if ((current->x == x) && (current->y == y)) {
            if (previous == NULL) {
                previous = current->next;
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

Powerup_t *deletePowerups(Powerup_t *list) {
    Powerup_t *temp;

    // check if list is already empty
    if (list == NULL)
        return list;
    
    while (list != NULL) {
        temp = list->next;
        free(list);
        list = temp;
    }
    return list; // should be NULL
 }

// Check if a powerup is present on arena tile
int isPowerupPresent(Powerup_t *list, int x, int y)
{
    for (Powerup_t *thisPowerup = list; thisPowerup != NULL; thisPowerup = thisPowerup->next)
    {
        if ((thisPowerup->x == x) && (thisPowerup->y == y))
            return thisPowerup->type;
    }

    return 0;
}

// Randomly spawn a powerup
Powerup_t *randPowerup(Powerup_t *list, int x, int y) {
    if (rand() % 100 <= 5) {
        int ptype = rand() % 3;
        if (ptype == 2)
            list = addPowerup(list, x, y, BOMB);
        else if (ptype == 1)
            list = addPowerup(list, x, y, RANGE);
        else if (ptype == 0)
            list = addPowerup(list, x, y, LIFE);
    }

    return list;
}


// Wreck Functions

Wreck_t *addWreck(Wreck_t *list, int x, int y, int timer) {
    Wreck_t *newWreck = malloc(sizeof(struct Wreck_t));
    if (newWreck == NULL) {
        printf("Error, malloc failed to create new wreck!\n");
        return NULL;
    }

    newWreck->x = x;
    newWreck->y = y;
    newWreck->timer = timer;
    newWreck->next = list;

    return newWreck;
}

// remove wreck up at specific x, y
Wreck_t *removeWreck(Wreck_t *list, int x, int y) {
    Wreck_t *previous = NULL;
    Wreck_t *current = list;

    while (current != NULL) {
        if ((current->x == x) && (current->y == y)) {
            if (previous == NULL) {
                previous = current->next;
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

// Delete all entries in the wrecking list
Wreck_t *deleteWrecks(Wreck_t *list) {
    Wreck_t *temp;

    // check if list is already empty
    if (list == NULL)
        return list;
    
    while (list != NULL) {
        temp = list->next;
        free(list);
        list = temp;
    }
    return list; // should be NULL
 }

// checks if wrecks need to be removed entirely
Wreck_t *checkWrecks(Wreck_t *list) {
    int ticks = SDL_GetTicks();

    Wreck_t* current = list;
    Wreck_t* previous = NULL;

    while (current != NULL) {
        if (ticks - current->timer > 1000) {
            arena[current->x][current->y] = TILE_EMPTY;

            // powerup drop?
            powerupList = randPowerup(powerupList, current->x, current->y);

            // remove wreck
            if (previous == NULL) {
                previous = current->next;
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

void initPlayer(Player_t *p)
{
    p->imgIndex = TILE_PLAYER;
    p->x = 1;
    p->y = 1;
    p->moveDir = 0;
    p->lives = 3;
    p->invulnerable = 0;
    p->maxBombs = 1;
    p->numBombs = 0;
    p->range = 1;
}

void initCursor(Cursor_t* c)
{
    c->imgIndex = 15;
    c->x = 5;
    c->y = MENU_START_Y;
}

void initGame(Game_t* g)
{
    g->state = MENU;
    g->level = 1;
    g->titleCardTimer = SDL_GetTicks();
}