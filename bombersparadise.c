#include "common.h"
#include "events.h"
#include "game.h"
#include "gfx.h"
#include "map.h"
#include "system.h"

// some things were meant to be global
SDL_Window *window;
SDL_Renderer *renderer;
Game_t game;
int arena[ARENA_WIDTH][ARENA_HEIGHT];
Player_t player;
Bomb_t *bombList = NULL;
Mob_t *mobList = NULL;
int score;

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


    // Honestly need to clean up everything here, set up some initialization functions for game and player...
    game.state = MENU;
    game.level = 1;

    // Load in images and tiles
    loadResources();
    setupFontTiles(fontTiles, FONT_NUM); // move this?
    setupTiles(tileIndex, TILE_NUM);
    loadMap(game.level);
    srand(time(0));

    //menuMusic = Mix_LoadMUS("sound/menu.wav");
    //if(menuMusic == NULL)
    //    printf("Could not load menu music: %s", Mix_GetError());
    //Mix_PlayMusic(menuMusic, -1);

    Cursor_t menuCursor;
    menuCursor.imgIndex = 15;
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
    player.lives = 3;
    loadMobs(1);

    // Game loop
    while(quit)
    {
        current_ticks = SDL_GetTicks();

        // Input
        SDL_PollEvent(&e);
        if(game.state == GAME)
        {
            quit = checkGameEvents(e, &player);
        }
        else if(game.state == MENU)
        {
            quit = checkMenuEvents(e, &menuCursor);
            if(quit == 2) // menu, moving to game
            {
                //Mix_HaltMusic();
                blockTicks = SDL_GetTicks();
                game.state = GAME;
                addBlocks(40, &player);
                // give player some room at spawn to use a bomb
                arena[1][1] = TILE_EMPTY;
                arena[2][1] = TILE_EMPTY;
                arena[1][2] = TILE_EMPTY;
                player.lives = 3;
                player.invulnerable = INVULNERABLE_TIME;
            }     
        }
        else if (game.state == WIN)
        {
            quit = checkWinEvents(e);
            if (quit == 2) // win, moving to next round
            {
                game.state = GAME;
                game.level++;
                blockTicks = SDL_GetTicks();
                resetplayer(&player);
                loadMap(game.level);
                addBlocks(40, &player);
                // give player some room at spawn to use a bomb
                arena[1][1] = TILE_EMPTY;
                arena[2][1] = TILE_EMPTY;
                arena[1][2] = TILE_EMPTY;
                player.invulnerable = INVULNERABLE_TIME;
            }
        }
        else if (game.state == GAMEOVER)
        {
            quit = checkGameOverEvents(e);
            if (quit == 2) // gameover, moving to start screen
            {
                game.state = MENU;
                game.level = 1;
                resetplayer(&player);
                loadMap(game.level);
            }
        }

        // Logic
        if(game.state == GAME)
        {
            movePlayer(&player);
            moveMobs(mobList);
            bombTimers(bombList);
            checkExplosions(bombList);
            bombList = clearBombs(bombList);
            mobList = clearMobs(mobList);
            //printBombs(bombList);
            if((SDL_GetTicks() - blockTicks) > 10000)
            {
                addBlocks(5, &player);
                score += 5;
                blockTicks = SDL_GetTicks();
            }
            if (player.invulnerable > 0)
                player.invulnerable = player.invulnerable - 1;
        }
        else if(game.state == MENU)
        {
            checkMCursorBounds(&menuCursor);
        }

        // Render
        SDL_RenderClear(renderer);
        if(game.state == GAME)
        {
            drawArena(tiles);
            drawPlayer(player, tiles);
            drawMobs(mobList, tiles);
            drawBombs(bombList, tiles);
            drawScore(score);
            drawLives(player.lives);
        }
        else if(game.state == MENU)
        {
            drawMenu(tiles);
            drawCursor(menuCursor, tiles);
        }
        else if(game.state == WIN)
        {
            drawWin();
        }
        else if(game.state == GAMEOVER)
        {
            drawGameOver();
        }

        drawFPS(fps_counter);
        SDL_RenderPresent(renderer);
        fps_counter = calculateFPS(current_ticks);
    }

    cleanup();
    return 0;
}