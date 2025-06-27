#ifndef GFX_H
#define GFX_H

#define FONT_WIDTH  8
#define MAX_LEN     32

extern SDL_Renderer *renderer;
extern SDL_Texture  *font;
extern SDL_Texture  *bg;

extern SDL_Rect fontTiles[FONT_NUM];
extern SDL_Rect tileIndex[TILE_NUM];

SDL_Texture *loadImage(char *filename);
void blitImage(SDL_Texture *image, int x, int y, int w, int h, int s);
void blitTile(SDL_Texture *image, int x, int y, int w, int h, SDL_FRect destRect);
void drawLetter(char c, int x, int y, int s);
void setupFontTiles(SDL_Rect f[], int num);
void drawString(char *string, int x, int y, int scale);
void drawFPS(int fps);
void drawSprite(SDL_Texture *sprite, int index, int x, int y, int scale);
void drawTile(SDL_Texture *t, int index, int x, int y, int s);
void drawArena(SDL_Texture *t);
void drawMenu(SDL_Texture *t);
void drawCursor(Sprite_t c, int menuIndex, SDL_Texture *img);
void drawWin();
void drawGameOver();
void drawPlayer(Player_t p, SDL_Texture *t);
void drawBombs(Bomb_t *b, SDL_Texture *t);
void drawShockwave(SDL_Texture *tiles, int tileid, int startx, int starty, int dx, int dy);
void drawTimerBar(Uint64 lastSpawn);
void drawScore(int score);
void drawLives(int lives, SDL_Texture *texture);
void drawMobs(Mob_t *list, SDL_Texture* tex);
void drawPowerups(Powerup_t *list, SDL_Texture* tex);
void drawLevelTitleCard(int levelNum);
void drawNumBombs(SDL_Texture *tex);
void drawRange(SDL_Texture *tex);
void drawHighscores(Highscore_t *scores);
void drawHighscoreEntry(HighscoreEntry_t entry, int score);

#endif