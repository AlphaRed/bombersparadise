#ifndef EVENTS_H
#define EVENTS_H

extern int win;

int checkGameEvents(SDL_Event e, Player_t *p);
int checkMenuEvents(SDL_Event e, Cursor_t *c);
void checkMCursorBounds(Cursor_t *c);
void clearArena();
void movePlayer(Player_t *player);
int checkCollision(Player_t *player, int destX, int destY);
void addBlocks(int num, Player_t *p);
Bomb_t *placeBomb(Player_t *p, Bomb_t *b);
void bombTimers(Bomb_t *bombList);
void bombExplode(Bomb_t *b);
void checkDestructible(int x, int y, Bomb_t *bombList);
int isBombPresent(Bomb_t *bombList, int x, int y);

#endif