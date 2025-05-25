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
Cursor_t menuCursor;
int arena[ARENA_WIDTH][ARENA_HEIGHT];
Player_t player;
Bomb_t *bombList = NULL;
Mob_t *mobList = NULL;
Powerup_t *powerupList = NULL;
Wreck_t *wreckingList = NULL;
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

    initGame(&game);
    initPlayer(&player);
    initCursor(&menuCursor);

    // Load in images and tiles
    loadResources();
    setupFontTiles(fontTiles, FONT_NUM); // move this?
    setupTiles(tileIndex, TILE_NUM);
    loadMap(game.level);
    loadMobs(game.level);
    srand(time(0));

    //menuMusic = Mix_LoadMUS("sound/menu.wav");
    //if(menuMusic == NULL)
    //    printf("Could not load menu music: %s", Mix_GetError());
    //Mix_PlayMusic(menuMusic, -1);

    int quit = 1;
    SDL_Event e;
    int current_ticks;
    int fps_counter = 0;
    int blockTicks = 0;
    score = 0;


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
                game.state = TITLECARD;
                game.titleCardTimer = SDL_GetTicks();
                //Mix_HaltMusic();
                resetplayer(&player);
                addBlocks(30, &player);
                blockTicks = SDL_GetTicks();
                clearSpawn(); // give player some room at spawn to use a bomb
            }     
        }
        else if (game.state == WIN)
        {
            quit = checkWinEvents(e);
            if (quit == 2) // win, moving to next round
            {
                game.level++;
                game.state = TITLECARD;
                game.titleCardTimer = SDL_GetTicks();
                loadMap(game.level);
                loadMobs(game.level);
                resetplayer(&player);
                bombList = deleteBombs(bombList);
                powerupList = deletePowerups(powerupList);
                addBlocks(30, &player);
                blockTicks = SDL_GetTicks();
                clearSpawn();
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
                bombList = deleteBombs(bombList);
                powerupList = deletePowerups(powerupList);
                wreckingList = deleteWrecks(wreckingList);
                loadMap(game.level);
                loadMobs(game.level);
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
            wreckingList = checkWrecks(wreckingList);
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
        else if (game.state == TITLECARD)
        {
            if (SDL_GetTicks() - game.titleCardTimer > 1000)
                game.state = GAME;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 5, 26, 48, 255);
        SDL_RenderClear(renderer);
        if(game.state == GAME)
        {
            drawArena(tiles);
            drawPowerups(powerupList, tiles);
            drawMobs(mobList, tiles);
            drawBombs(bombList, tiles);
            drawPlayer(player, tiles);
            drawTimerBar(blockTicks);
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
            killMobs(mobList);
        }
        else if(game.state == GAMEOVER)
        {
            drawGameOver();
        }
        else if (game.state == TITLECARD)
        {
            drawLevelTitleCard(game.level);
        }

        drawFPS(fps_counter);
        SDL_RenderPresent(renderer);
        fps_counter = calculateFPS(current_ticks);
    }

    cleanup();
    return 0;
}