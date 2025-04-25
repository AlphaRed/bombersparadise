#ifndef EVENTS_H
#define EVENTS_H

extern int win;

int checkGameEvents(SDL_Event e, Player_t *p);
int checkMenuEvents(SDL_Event e, Cursor_t *c);
void checkMCursorBounds(Cursor_t *c);
void clearArena();
void checkCollision(Player_t *p);
void addBlocks(int num, Player_t *p);
Bomb_t *placeBomb(Player_t *p, Bomb_t *b);
void bombTimers(Bomb_t *bombList);
void bombExplode(Bomb_t *b);
void checkDestructible(int x, int y);

#endif