#include "common.h"
#include "events.h"
#include "gfx.h"

SDL_Texture *loadImage(char *filename)
{
    SDL_Texture *t = NULL;
    SDL_Surface *img = IMG_Load(filename);
    if(!img)
    {
        printf("Image failed to load: %s\n", SDL_GetError());
        return NULL;
    }
    t = SDL_CreateTextureFromSurface(renderer, img);
    SDL_SetTextureScaleMode(t, SDL_SCALEMODE_NEAREST);
    return t;
}

void blitImage(SDL_Texture *image, int x, int y, int w, int h, int s)
{
    SDL_FRect destRect;
    destRect.x = x;
    destRect.y = y;
    destRect.w = w * s;
    destRect.h = h * s;

    SDL_RenderTexture(renderer, image, NULL, &destRect);
}

void blitTile(SDL_Texture *image, int x, int y, int w, int h, SDL_FRect destRect)
{
    SDL_FRect srcRect;
    srcRect.x = x;
    srcRect.y = y;
    srcRect.w = w;
    srcRect.h = h;

    SDL_RenderTexture(renderer, image, &srcRect, &destRect);
}


// TO DO: move with other text related functions to new file
void drawLetter(char c, int x, int y, int s)
{
    //int x = ((c - 32) % 8) * 8; // ASCII starts at char = 32, font width = 8px
    //int y = ((c - 32) / 16) * 12; // ASCII starts at char = 32, num of chars in width = 16, font height = 12px

    SDL_FRect destRect;
    destRect.x = x;
    destRect.y = y;
    destRect.w = 8 * s;
    destRect.h = 12 * s;
    
    int fontIndex = c - 32; // ASCII starts at character #32
    blitTile(font, fontTiles[fontIndex].x, fontTiles[fontIndex].y, fontTiles[fontIndex].w, fontTiles[fontIndex].h, destRect);
}

// TO DO: move with other text related functions to new file
void setupFontTiles(SDL_Rect f[], int num)
{
    for(int i = 0; i < num; i++)
    {
        f[i].x = (i % 16) * 8;
        f[i].y = (i / 16) * 12;
        f[i].w = 8;
        f[i].h = 12;
    }
}

// TO DO: move with other text related functions to new file
void drawString(char *string, int x, int y, int scale)
{
    // only draw a string max length of 40 characters! (1,280 res width / (8 * 4) don't forget the upscale of 4x
    // technically can be drawn closer due to the way I've drawn the pixel art!
    //int x = 0;
    int len = strlen(string);
    if(len > MAX_LEN)
        len = MAX_LEN; // max out at 40

    for(int i = 0; i < len; i++)
    {
        drawLetter(string[i], x, y, scale);
        if(string[i] == 'i')
            x += ((FONT_WIDTH - 4) * scale); // more for i...maybe l?
        else
            x += ((FONT_WIDTH - 2) * scale); // Minus two for distancing...kerning(?)
    }
}

// TO DO: move with other text related functions to new file
void drawFPS(int fps)
{
    char c;
    int x;

    drawString("FPS:", 400, 0, 1);

    if(fps > 1000)
        fps = 1000;
    
    for(int i = 0; i < 4; i++)
    {
        c = (fps % 10) + 48; // need 48 to convert to ASCII numbers
        fps = fps / 10;
        x = 464 - (i * 8);
        drawLetter(c, x, 0, 1);
    }   
}

// For drawing single tiles as sprites, freely on the screen using screen coordinates
void drawSprite(SDL_Texture *sprite, int index, int x, int y, int scale)
{
    SDL_FRect destRect;
    destRect.x = x;
    destRect.y = y;
    destRect.w = TILE_SIZE * scale;
    destRect.h = TILE_SIZE * scale;

    blitTile(sprite, tileIndex[index].x, tileIndex[index].y, tileIndex[index].w, tileIndex[index].h, destRect);
}

// For drawing single tiles on the arena using arena coordinates
void drawTile(SDL_Texture *t, int index, int x, int y, int s)
{
    SDL_FRect destRect;
    destRect.x = x * TILE_SCALE * TILE_SIZE;
    destRect.y = (y+2) * TILE_SCALE * TILE_SIZE;
    destRect.w = TILE_SIZE * s;
    destRect.h = TILE_SIZE * s;
    
    blitTile(t, tileIndex[index].x, tileIndex[index].y, tileIndex[index].w, tileIndex[index].h, destRect);
}

void drawArena(SDL_Texture *t)
{
    for(int i = 0; i < ARENA_HEIGHT; i++)
    {
        for(int j = 0; j < ARENA_WIDTH; j++)
        {
            drawTile(t, arena[j][i], j, i, TILE_SCALE);
        }
    }
}

void drawMenu(SDL_Texture *t)
{
    drawString("Bomber's Paradise", 50, 20, 4);
    drawString("Start", 50, 250, 4);
    drawString("Exit", 50, 300, 4);
    drawString("By Isaac", 50, 420, 4);
    drawSprite(t, 12, 192, 160, TILE_SCALE * 2);
    drawSprite(t, 13, 256, 160, TILE_SCALE * 2);
    drawSprite(t, 14, 192, 96, TILE_SCALE * 2);
}

void drawCursor(Cursor_t c, SDL_Texture *t)
{
    drawSprite(t, c.imgIndex, c.x, c.y, TILE_SCALE);
}

void drawWin(int n)
{
    if(n == 1)
    {
        blitImage(bg, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1); // clear screen
        drawString("Good job!", 50, 40, 4);
        drawString("Press enter to", 50, 150, 4);
        drawString("continue.", 50, 200, 4);
    }
    else
        return;
}

void drawPlayer(Player_t p, SDL_Texture *t)
{
    drawTile(t, p.imgIndex, p.x, p.y, TILE_SCALE);
}

void drawBombs(Bomb_t *b, SDL_Texture *t)
{
    int currentTime = SDL_GetTicks();
    for(Bomb_t *p = b; p != NULL; p = p->next)
    {
        if(p->exploded == 0)
        {
             if((currentTime - p->timer) > 1000 && (currentTime - p->timer) < 1200) // 200 ms delay seems to work alright
                drawTile(t, p->imgIndex + 1, p->x, p->y, TILE_SCALE);
            else if((currentTime - p->timer) > 2000 && (currentTime - p->timer) < 2200)
                drawTile(t, p->imgIndex + 1, p->x, p->y, TILE_SCALE);
            else if((currentTime - p->timer) > 3000 && (currentTime - p->timer) < 3200)
                drawTile(t, p->imgIndex + 1, p->x, p->y, TILE_SCALE);
            else
                drawTile(t, p->imgIndex, p->x, p->y, TILE_SCALE);
        }
        else if(p->exploded == 1)
        {
            if((currentTime - p->timer) < 3500)
                drawTile(t, 8, p->x, p->y, TILE_SCALE);
            else if((currentTime - p->timer) < 4500)
            {
                drawTile(t, 9, p->x, p->y, TILE_SCALE);
                // probably a better way to do this...
                if(arena[p->x][p->y - 1] != TILE_WALL) // above
                {
                    drawTile(t, 11, p->x, p->y - 1, TILE_SCALE);
                    checkDestructible(p->x, p->y - 1);
                }
                if(arena[p->x][p->y + 1] != TILE_WALL) // below
                {
                    drawTile(t, 11, p->x, p->y + 1, TILE_SCALE);
                    checkDestructible(p->x, p->y + 1);
                }
                if(arena[p->x - 1][p->y] != TILE_WALL) // left
                {
                    drawTile(t, 10, p->x - 1, p->y, TILE_SCALE);
                    checkDestructible(p->x - 1, p->y);
                }
                if(arena[p->x + 1][p->y] != TILE_WALL) // right
                {
                    drawTile(t, 10, p->x + 1, p->y, TILE_SCALE);
                    checkDestructible(p->x + 1, p->y);
                }
            }
            else if((currentTime - p->timer) < 5000)
                drawTile(t, 8, p->x, p->y, TILE_SCALE);
        }
    }
}

void drawScore(int s)
{
    char c;
    int x;

    drawString("Score", 0, 10, 4);
    
    for (int i = 0; i < 8; i++)
    {
        c = (s % 10) + 48;
        s = s / 10;
        x = 450 - (i * 8 * 4);
        drawLetter(c, x, 10, 4);
    }
}