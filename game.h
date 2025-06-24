#ifndef GAME_H
#define GAME_H

// Bomb Functions
void bombTimers(Bomb_t *list);
void checkExplosions(Bomb_t* list);
void bombShockwave(int startx, int starty, int dx, int dy);
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
Powerup_t *addPowerup(Powerup_t* list, int x, int y, Poweruptype type);
Powerup_t *removePowerup(Powerup_t *list, int x, int y);
Powerup_t *deletePowerups(Powerup_t *list);
int isPowerupPresent(Powerup_t *list, int x, int y);
Powerup_t *randPowerup(Powerup_t *list, int x, int y);


// Wreck Functions
Wreck_t *addWreck(Wreck_t *list, int x, int y, Uint64 timer);
Wreck_t *removeWreck(Wreck_t *list, int x, int y);
Wreck_t *deleteWrecks(Wreck_t *list);
Wreck_t *checkWrecks(Wreck_t *list);

// Highscore Functions
int submitHighscore(Highscore_t *highscore, Highscore_t newscore);

// Initialization Functions
void initPlayer(Player_t* p);
void initCursor(Sprite_t* c);
void initGame(Game_t* g);
void initHighscores(Highscore_t *scores);

#endif