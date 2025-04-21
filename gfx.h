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
void blitTile(SDL_Texture *image, int x, int y, int w, int h, SDL_Rect destRect);
void drawLetter(char c, int x, int y, int s);
void setupFontTiles(SDL_Rect f[], int num);
void drawString(char *string, int x, int y);
void drawFPS(int fps);
void drawTile(SDL_Texture *t, int index, int x, int y, int s);
void drawArena(SDL_Texture *t);
void drawMenu(SDL_Texture *t);
void drawCursor(Cursor_t c, SDL_Texture *img);
void drawWin(int n);
void drawPlayer(Player_t p, SDL_Texture *t);
void drawBombs(Bomb_t *b, SDL_Texture *t);
void drawScore(int s);

#endif