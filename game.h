#ifndef GAME_H
#define GAME_H

// Bomb Functions
void bombExplode(Bomb_t *b);
void bombTimers(Bomb_t *bombList);
Bomb_t *clearBombs(Bomb_t *list);
int isBombPresent(Bomb_t *bombList, int x, int y);
void printBombs(Bomb_t *list);

// Mob Functions
void moveMobs(Mob_t* mob);
int checkCollisionMob(Mob_t* mob, int destX, int destY);

#endif