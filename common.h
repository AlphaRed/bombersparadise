#ifndef COMMON_H
#define COMMON_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
//#include <SDL3_mixer/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH    256 * 2
#define SCREEN_HEIGHT   240 * 2
#define TILE_SIZE       8
#define TILE_SCALE      4
#define FONT_NUM        200
#define MAX_FPS         60
#define MAX_CHARS       30
#define TILE_NUM        4 * 4
#define ARENA_WIDTH     15
#define ARENA_HEIGHT    13

// map tiles
#define TILE_EMPTY      0
#define TILE_WALL       1
#define TILE_BLOCK      2
#define TILE_EXIT       3

typedef enum {MENU, GAME} Gamestate;

typedef struct Cursor_t // could maybe make this a generic sprite/obj struct?
{
    int imgIndex;
    int x;
    int y;
} Cursor_t;

typedef struct Player_t
{
    int imgIndex;
    int x;
    int y;
    int lastDir; // clockwise compass: 1 (N), 2 (E), 3 (S), 4 (W)
} Player_t;

typedef struct Bomb_t
{
    int imgIndex;
    int x;
    int y;
    int timer;
    int exploded;
    struct Bomb_t *next;
} Bomb_t;

extern int arena[ARENA_WIDTH][ARENA_HEIGHT];
extern Bomb_t *bombList;

#endif