#include "common.h"
#include "events.h"
#include "game.h"
#include "gfx.h"
#include "map.h"
#include "system.h"

// some things were meant to be global
SDL_Renderer *renderer;
Game_t game;
Highscore_t highscores[HIGHSCORE_LIMIT];
HighscoreEntry_t highscoreEntry;
Sprite_t menuCursor;
int arena[ARENA_WIDTH][ARENA_HEIGHT];
Player_t player;
Bomb_t *bombList = NULL;
Mob_t *mobList = NULL;
Powerup_t *powerupList = NULL;
Wreck_t *wreckingList = NULL;

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
    initHighscores(highscores);
    initPlayer(&player);
    initCursor(&menuCursor);

    // Load in images and tiles
    loadResources();
    SDL_SetTextureColorMod(font, FONT_COLOR_R, FONT_COLOR_G, FONT_COLOR_B);
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
    Uint64 current_ticks;
    int fps_counter = 0;
    Uint64 blockTicks = 0;
    int menuIndex = 0;  // 0 first item, 1 is second... etc
    player.score = 0;


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
            quit = checkMenuEvents(e, &menuIndex);
            if(quit == 2) // menu, moving to game
            {
                game.state = TITLECARD;
                game.titleCardTimer = SDL_GetTicks();
                //Mix_HaltMusic();
                game.level = 1;
                initPlayer(&player);
                resetplayer(&player);
                addBlocks(30, &player);
                blockTicks = SDL_GetTicks();
                clearSpawn(); // give player some room at spawn to use a bomb
            }
            else if (quit == 3) { // menu, moving to highscores
                game.state = HIGHSCOREVIEW;
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
                switch (game.level) {
                case 1:
                case 2:
                case 3:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                    addBlocks(30, &player);
                    break;
                case 4:
                    addBlocks(15, &player);
                    break;
                }
                blockTicks = SDL_GetTicks();
                clearSpawn();
            }
        }
        else if (game.state == GAMEOVER)
        {
            quit = checkGameOverEvents(e);
            if (quit == 2) // moving out of game over
            {
                bombList = deleteBombs(bombList);
                powerupList = deletePowerups(powerupList);
                wreckingList = deleteWrecks(wreckingList);
                killMobs(mobList);
                loadMap(game.level);
                loadMobs(game.level);

                if (isHighscore(highscores, player.score) > -1) {
                    game.state = HIGHSCORE;
                    highscoreEntry.cursor = 0;
                    strcpy(highscoreEntry.initials, "AAA");
                    highscoreEntry.confirmed = 0;
                }
                else
                    game.state = MENU;
            }
        }
        else if (game.state == HIGHSCOREVIEW) {
            quit = checkWinEvents(e); // I'm a bad man for reusing functions
            if (quit == 2) {    // highscores, moving to menu
                game.state = MENU;
            }
        }
        else if (game.state == HIGHSCORE) {
            quit = checkHighscoreEntryEvents(e, &highscoreEntry);
            if (quit == 2)      // finished entering highscore, moving to menu
                game.state = MENU;
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
                player.score += 5;
                blockTicks = SDL_GetTicks();
            }
            if (player.invulnerable > 0)
                player.invulnerable = player.invulnerable - 1;
        }
        else if(game.state == MENU)
        {
        }
        else if (game.state == TITLECARD)
        {
            if (SDL_GetTicks() - game.titleCardTimer > 1000)
                game.state = GAME;
        }
        else if (game.state == HIGHSCORE) {
            // clamp cursor to 0-2
            if (highscoreEntry.cursor < 0) highscoreEntry.cursor = 2;
            if (highscoreEntry.cursor > 2) highscoreEntry.cursor = 0;
            // clamp current character to 20h-5Fh
            if (highscoreEntry.initials[highscoreEntry.cursor] < 32)
                highscoreEntry.initials[highscoreEntry.cursor] = 95;
            if (highscoreEntry.initials[highscoreEntry.cursor] > 95)
                highscoreEntry.initials[highscoreEntry.cursor] = 32;

            if (highscoreEntry.confirmed == 1) {
                Highscore_t score;
                strcpy(score.initials, highscoreEntry.initials);
                score.score = player.score;
                score.level = 3;
                if (submitHighscore(highscores, score))
                    game.state = HIGHSCOREVIEW;
            }
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
            drawScore(player.score);
            drawLives(player.lives, tiles);
            drawNumBombs(tiles);
            drawRange(tiles);
        }
        else if(game.state == MENU)
        {
            drawMenu(tiles);
            drawCursor(menuCursor, menuIndex, tiles);
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
        else if (game.state == HIGHSCORE) {
            drawHighscoreEntry(highscoreEntry, player.score);
        }
        else if (game.state == HIGHSCOREVIEW) {
            drawHighscores(highscores);
        }

        drawFPS(fps_counter);
        SDL_RenderPresent(renderer);
        fps_counter = calculateFPS(current_ticks);
    }

    cleanup();
    return 0;
}