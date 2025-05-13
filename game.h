#ifndef GAME_H
#define GAME_H

// Bomb Functions
void bombExplode(Bomb_t *b);
void bombTimers(Bomb_t *list);
void checkExplosions(Bomb_t* list);
Bomb_t *deleteBombs(Bomb_t *list);
Bomb_t *clearBombs(Bomb_t *list);
int isBombPresent(Bomb_t *bombList, int x, int y);
void printBombs(Bomb_t *list);

// Mob Functions
void moveMobs(Mob_t *list);
int checkCollisionMob(Mob_t* mob, int destX, int destY);
Mob_t* addMob(Mob_t* list, int x, int y, int direction);
void loadMobs(int lvlNum);
Mob_t* clearMobs(Mob_t* list);
int isMobPresent(Mob_t* list, int x, int y);
void killMobs(Mob_t* list);

// Powerup Functions
Powerup_t* addPowerup(Powerup_t* list, int x, int y, Poweruptype type);
Powerup_t *removePowerup(Powerup_t *list, int x, int y);
int isPowerupPresent(Powerup_t *list, int x, int y);

// Initialization Functions
void initPlayer(Player_t* p);
void initCursor(Cursor_t* c);
void initGame(Game_t* g);

#endif