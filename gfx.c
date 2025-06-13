#include "common.h"
#include "events.h"
#include "gfx.h"
#include "game.h"

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
    destRect.w = FONT_WIDTH * s;
    destRect.h = FONT_HEIGHT * s;
    
    int fontIndex = c;
    blitTile(font, fontTiles[fontIndex].x, fontTiles[fontIndex].y, fontTiles[fontIndex].w, fontTiles[fontIndex].h, destRect);
}

// TO DO: move with other text related functions to new file
void setupFontTiles(SDL_Rect f[], int num)
{
    for(int i = 0; i < num; i++)
    {
        f[i].x = (i % 16) * FONT_WIDTH;
        f[i].y = (i / 16) * FONT_HEIGHT;
        f[i].w = FONT_WIDTH;
        f[i].h = FONT_HEIGHT;
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
        x += (FONT_WIDTH * scale); // Minus two for distancing...kerning(?)
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
    destRect.y = (y+2) * TILE_SCALE * TILE_SIZE + 8;
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
    drawString("Bomber's Paradise", 50, 20, 3);
    drawString("Start", 50, 250, 3);
    drawString("Exit", 50, 300, 3);
    drawString("By Isaac and Sam", 50, 420, 3);
    drawSprite(t, 12, 192, 160, TILE_SCALE * 2);
    drawSprite(t, 13, 256, 160, TILE_SCALE * 2);
    drawSprite(t, 14, 192, 96, TILE_SCALE * 2);
}

void drawCursor(Cursor_t c, SDL_Texture *t)
{
    drawSprite(t, c.imgIndex, c.x, c.y, TILE_SCALE);
}

void drawWin()
{
    drawString("Good job!", 50, 40, 3);
    drawString("Press enter to", 50, 150, 3);
    drawString("continue.", 50, 200, 3);
}

void drawGameOver()
{
    drawString("Game Over!", 50, 40, 3);
    drawString("Press enter.", 50, 150, 3);
}

void drawPlayer(Player_t p, SDL_Texture *t)
{
    int tile = p.imgIndex;

    // change tile for invulnerability
    if ((p.invulnerable > 1) && (p.invulnerable/100 & 1))
        tile = tile - 1;

    drawTile(t, tile, p.x, p.y, TILE_SCALE);
}

void drawBombs(Bomb_t *b, SDL_Texture *t)
{
    int currentTime = SDL_GetTicks();
    for(Bomb_t *p = b; p != NULL; p = p->next)
    {
        if(p->state == TICKING)
        {
             if((currentTime - p->timer) > 1000 && (currentTime - p->timer) < 1200) // 200 ms delay seems to work alright
                drawTile(t, p->imgIndex + 1, p->x, p->y, TILE_SCALE);
            else if((currentTime - p->timer) > 2000 && (currentTime - p->timer) < 2200)
                drawTile(t, p->imgIndex + 1, p->x, p->y, TILE_SCALE);
            else if((currentTime - p->timer) > 3000 && (currentTime - p->timer) < 3500)
                drawTile(t, TILE_EXPLOSION_SMALL, p->x, p->y, TILE_SCALE);
            else
                drawTile(t, p->imgIndex, p->x, p->y, TILE_SCALE); // fail-safe (shouldn't happen)
        }
        else if(p->state == EXPLODED)
        {
            if((currentTime - p->timer) < 3500)
                drawTile(t, TILE_EXPLOSION_SMALL, p->x, p->y, TILE_SCALE);
            else if((currentTime - p->timer) < 4500)
            {
                drawTile(t, TILE_EXPLOSION_LARGE, p->x, p->y, TILE_SCALE);
                drawShockwave(t, TILE_EXPLOSION_VERT, p->x, p->y, 0, -1);   // above
                drawShockwave(t, TILE_EXPLOSION_VERT, p->x, p->y, 0, 1);    // below
                drawShockwave(t, TILE_EXPLOSION_HORIZ, p->x, p->y, -1, 0);  // left
                drawShockwave(t, TILE_EXPLOSION_HORIZ, p->x, p->y, 1, 0);   // right
            }
            else if((currentTime - p->timer) < 5000)
                drawTile(t, 8, p->x, p->y, TILE_SCALE);
        }
    }
}

void drawShockwave(SDL_Texture *tiles, int tileid, int startx, int starty, int dx, int dy) {
    int x = startx + dx;
    int y = starty + dy;

    for (int length = player.range; length > 0; length--) {
        // is this a wall?
        if (arena[x][y] == TILE_WALL)
            return; // finished drawing the explosion

        // is this a block?
        else if ((arena[x][y] == TILE_BLOCK) || (arena[x][y] == TILE_WRECK)) {
            return; // this is the last tile to draw in the shockwave
        }

        for (Bomb_t *thisBomb = bombList; thisBomb != NULL; thisBomb = thisBomb->next) {
            if ((thisBomb->x == x) && (thisBomb->y == y) && (thisBomb->state == EXPLODED)) {
                drawTile(tiles, TILE_EXPLOSION_LARGE, x, y, TILE_SCALE); // draw a center tile if it's another bomb
                return;
            }
        }

        // draw an explosion
        drawTile(tiles, tileid, x, y, TILE_SCALE);

        x += dx;
        y += dy;
    }
}

void drawTimerBar(int lastSpawn) {
    SDL_FRect bar;

    float width = ((float)SDL_GetTicks() - (float)lastSpawn) / 10000.0 * (SCREEN_WIDTH);

    SDL_SetRenderDrawColor(renderer, 95, 177, 78, 255);

    bar.x = 0;
    bar.y = 0;
    bar.h = TIMERBAR_HEIGHT;
    bar.w = width;

    SDL_RenderFillRect(renderer, &bar);
}

void drawScore(int score)
{
    char c;
    int x;

    drawString("Score", 0, 10, 3);
    
    // draw digits from ones column first (right to left)
    for (int i = 0; i < 8; i++) {
        c = (score % 10) + 48;  // 48 = '0'
        score = score / 10;
        x = ((13 - i) * 8 * 3);
        drawLetter(c, x, 10, 3);
    }
}

void drawLives(int lives, SDL_Texture *texture)
{
    char c;

    drawSprite(texture, TILE_PLAYER, TILE_SIZE * 11 * TILE_SCALE, 38, TILE_SCALE);
    drawLetter('x', 393, 44, 3);
    // lives should really only be one digit
    c = (lives % 10) + 48;
    drawLetter(c, 423, 44, 3);
}

void drawMobs(Mob_t *list, SDL_Texture* tex)
{
    for (Mob_t* thisMob = list; thisMob != NULL; thisMob = thisMob->next) {
        drawTile(tex, thisMob->imgIndex, thisMob->x, thisMob->y, TILE_SCALE);
    }
}

void drawPowerups(Powerup_t *list, SDL_Texture* tex)
{
    int tile;

    for (Powerup_t* thisPowerup = list; thisPowerup != NULL; thisPowerup = thisPowerup->next)
    {
        switch (thisPowerup->type) {
            case BOMB:
                tile = TILE_POWERUP_BOMB;
                break;
            case RANGE:
                tile = TILE_POWERUP_RANGE;
                break;
            case LIFE:
                tile = TILE_POWERUP_LIFE;
                break;
            default:
                break;
        }

        drawTile(tex, tile, thisPowerup->x, thisPowerup->y, TILE_SCALE);
    }
}

void drawLevelTitleCard(int levelNum)
{
    switch (levelNum)
    {
    case 1:
        SDL_RenderClear(renderer);
        drawString("Level One", 120, 200, 3);
        break;
    case 2:
        SDL_RenderClear(renderer);
        drawString("Level Two", 120, 200, 3);
        break;
    case 3:
        SDL_RenderClear(renderer);
        drawString("Level Three", 110, 200, 3);
        break;
    case 4:
        SDL_RenderClear(renderer);
        drawString("Level Four", 120, 200, 3);
        break;
    case 5:
        SDL_RenderClear(renderer);
        drawString("Level Five", 120, 200, 3);
        break;
    case 6:
        SDL_RenderClear(renderer);
        drawString("Level Six", 120, 200, 3);
        break;
    case 7:
        SDL_RenderClear(renderer);
        drawString("Level Seven", 110, 200, 3);
        break;
    case 8:
        SDL_RenderClear(renderer);
        drawString("Level Eight", 110, 200, 3);
        break;
    case 9:
        SDL_RenderClear(renderer);
        drawString("Level Nine", 120, 200, 3);
        break;
    case 10:
        SDL_RenderClear(renderer);
        drawString("Level Ten", 120, 200, 3);
        break;
    default:
        break;
    }
}

void drawNumBombs(SDL_Texture *tex) {
    drawSprite(tex, TILE_BOMB, 0, 38, TILE_SCALE);
    drawLetter('x', 40, 44, 3);
    char c = (player.maxBombs % 10) + 48;
    drawLetter(c, 70, 44, 3);
}

void drawRange(SDL_Texture *tex) {
    char c;

    drawSprite(tex, TILE_EXPLOSION_SMALL, 180, 38, TILE_SCALE);
    drawLetter('x', 180+40, 44, 3);
    c = (player.range % 10) + 48;
    drawLetter(c, 180+70, 44, 3);
}