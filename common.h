#ifndef COMMON_H
#define COMMON_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
//#include <SDL3_mixer/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TILE_SCALE      2
#define TILE_SIZE       16       // in px
#define ARENA_WIDTH     15
#define ARENA_HEIGHT    13
#define TIMERBAR_HEIGHT 8
#define SCREEN_WIDTH    ARENA_WIDTH * TILE_SIZE * TILE_SCALE
#define SCREEN_HEIGHT   (ARENA_HEIGHT + 2) * TILE_SIZE * TILE_SCALE + TIMERBAR_HEIGHT
#define MAX_FPS         60
#define TILE_NUM        8 * 8
#define MENU_START_Y	245
#define MENU_EXIT_Y     295

// palette
#define PAL_0_R     5       // 0 -- dark blue
#define PAL_0_G     26
#define PAL_0_B     48
#define PAL_1_R     59      // 1 -- light blue
#define PAL_1_G     179
#define PAL_1_B     231
#define PAL_2_R     95      // 2 -- green
#define PAL_2_G     177
#define PAL_2_B     78
#define PAL_3_R     243     // 3 -- eggshell white
#define PAL_3_G     233
#define PAL_3_B     197

// font
#define FONT_NUM        256
#define FONT_HEIGHT     8
#define FONT_WIDTH      8
#define FONT_COLOR_R    PAL_3_R
#define FONT_COLOR_G    PAL_3_G
#define FONT_COLOR_B    PAL_3_B

// tiles
// map
#define TILE_EMPTY              0
#define TILE_WALL               1
#define TILE_BLOCK              2
#define TILE_WRECK              32
#define TILE_EXIT               3
// entities
#define TILE_PLAYER             5
#define TILE_BOMB               6
#define TILE_MOB                16
// explosions
#define TILE_EXPLOSION_SMALL    8
#define TILE_EXPLOSION_LARGE    9
#define TILE_EXPLOSION_HORIZ    10
#define TILE_EXPLOSION_VERT     11
// powerups
#define TILE_POWERUP_BOMB       24
#define TILE_POWERUP_RANGE      25

#define INVULNERABLE_TIME   1000 // ms of invulnerability (at start and when hit)

typedef enum {MENU, GAME, WIN, GAMEOVER, TITLECARD} Gamestate;
typedef enum {TICKING, EXPLODED, DEAD} Bombstate;
typedef enum {ALIVE, KILLED} Mobstate;
typedef enum {NOTYPE, BOMB, RANGE} Poweruptype;

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
    int moveDir; // clockwise compass: 1 (N), 2 (E), 3 (S), 4 (W)
    int lives;
    int invulnerable;
    int maxBombs;
    int numBombs;
    int range;
} Player_t;

typedef struct Bomb_t
{
    int imgIndex;
    int x;
    int y;
    int timer;
    Bombstate state;
    struct Bomb_t *next;
} Bomb_t;

typedef struct Mob_t {
    int imgIndex;
    int x;
    int y;
    int dir;
    int lastMove;
    Mobstate state;
    struct Mob_t *next;
} Mob_t;

typedef struct Powerup_t {
    int x;
    int y;
    Poweruptype type;
    struct Powerup_t *next;
} Powerup_t;

typedef struct Wreck_t {
    int x;
    int y;
    int timer;
    struct Wreck_t *next;
} Wreck_t;

typedef struct Game_t {
    Gamestate state;
    int level;
    int titleCardTimer;
} Game_t;

extern Game_t game;
extern int arena[ARENA_WIDTH][ARENA_HEIGHT];
extern Player_t player;
extern int score;
extern Bomb_t *bombList;
extern Mob_t *mobList;
extern Powerup_t *powerupList;
extern Wreck_t *wreckingList;

#endif