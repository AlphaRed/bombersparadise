#ifndef EVENTS_H
#define EVENTS_H

extern int win;

int checkGameEvents(SDL_Event e, Player_t *p);
int checkMenuEvents(SDL_Event e, int *menuIndex);
int checkWinEvents(SDL_Event e);
int checkGameOverEvents(SDL_Event e);
int checkHighscoreEntryEvents(SDL_Event e, HighscoreEntry_t *entry);

void checkMCursorBounds(Cursor_t *c);
void movePlayer(Player_t *player);
int checkCollision(Player_t *player, int destX, int destY);
Bomb_t *placeBomb(Player_t *p, Bomb_t *b);

#endif