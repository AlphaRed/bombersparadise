#include "common.h"
#include "events.h"
#include "gfx.h"
#include "system.h"
#include "map.h"

// some things were meant to be global
SDL_Window *window;
SDL_Renderer *renderer;
Gamestate gs = MENU;
int arena[ARENA_WIDTH][ARENA_HEIGHT];
Player_t player;
Bomb_t *bombList = NULL;
int score;
int win;

SDL_Rect fontTiles[FONT_NUM];
SDL_Rect tileIndex[TILE_NUM];

// resources
SDL_Texture *bg;
SDL_Texture *tiles;
SDL_Texture *font;
//Mix_Music   *menuMusic;

int main(int argc, char *args[])
{    
    // SDL setup
    if(initSDL())
    {
        printf("initSDL failed.\n");
        return 1;
    }

    // Load in images and tiles
    loadResources();
    setupFontTiles(fontTiles, FONT_NUM); // move this?
    setupTiles(tileIndex, TILE_NUM);
    loadMap("arena.txt");
    srand(time(0));

    //menuMusic = Mix_LoadMUS("sound/menu.wav");
    //if(menuMusic == NULL)
    //    printf("Could not load menu music: %s", Mix_GetError());
    //Mix_PlayMusic(menuMusic, -1);

    Cursor_t menuCursor;
    menuCursor.imgIndex = 4;
    menuCursor.x = 5;
    menuCursor.y = 255;

    player.imgIndex = 5;
    resetplayer(&player);

    int quit = 1;
    SDL_Event e;
    int current_ticks;
    int fps_counter = 0;
    int blockTicks = 0;
    score = 0;
    win = 0;

    // Game loop
    while(quit)
    {
        current_ticks = SDL_GetTicks();

        // Input
        //SDL_WaitEvent(&e); // seems to work better than PollEvent
        SDL_PollEvent(&e);
        if(gs == GAME)
        {
            quit = checkGameEvents(e, &player);
            if(quit == 2)   // Won screen, moving to menu state
            {
                gs = MENU;
                win = 0;
                //Mix_PlayMusic(menuMusic, -1);
                resetplayer(&player);
            }
        }
        else if(gs == MENU)
        {
            quit = checkMenuEvents(e, &menuCursor);
            if(quit == 2) // menu, moving to game
            {
                //Mix_HaltMusic();
                blockTicks = SDL_GetTicks();
                gs = GAME;
                addBlocks(40, &player);
            }     
        }

        // Logic
        if(gs == GAME)
        {
            checkCollision(&player);
            bombTimers(bombList);
            if((SDL_GetTicks() - blockTicks) > 10000)
            {
                addBlocks(5, &player);
                score += 5;
                blockTicks = SDL_GetTicks();
            }
        }
        else if(gs == MENU)
        {
            checkMCursorBounds(&menuCursor);
        }

        // Render
        SDL_RenderClear(renderer);
        if(gs == GAME)
        {
            drawArena(tiles);
            drawPlayer(player, tiles);
            drawBombs(bombList, tiles);
            drawScore(score);
            drawWin(win);
        }
        else if(gs == MENU)
        {
            drawMenu(tiles);
            drawCursor(menuCursor, tiles);
        }

        drawFPS(fps_counter);
        SDL_RenderPresent(renderer);
        fps_counter = calculateFPS(current_ticks);
    }

    cleanup();
    return 0;
}