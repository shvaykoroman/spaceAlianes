#ifndef HANDMADE_H
#define HANDMADE_H
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float  f32;
typedef double f64;

typedef size_t sizet;

#define internal static
#define global static
#define local_persist static

#define kilobytes(x) (x*1024LL)
#define megabytes(x) (kilobytes(x) * 1024LL)
#define gigabytes(x) (megabytes(x) * 1024LL)
#define terabytes(x) (gigabytes(x)*1024LL)

#define assert(Expression); if(!(Expression)) {*(int*)0 = 0;}
#define arrayCount(array) (sizeof(array) / sizeof(array[0]))
#define invalidPath() assert("Invalid code path!\n")

struct GameBackbuffer
{
    void *memory;
    s32 width;
    s32 height;
    s32 stride;
};

struct game_button_state
{
    s32 halfTransitionCount;
    bool endedDown;
};

struct game_controller_input
{
    union
    {
        game_button_state buttons[5];
        struct
        {
            game_button_state buttonLeft;
            game_button_state buttonRight;
            game_button_state buttonUp;
            game_button_state buttonDown;
            game_button_state buttonEnter;
        };
    };
};

struct game_input
{
    f32 dtForFrame;
    game_controller_input controllers[2];
};



struct game_memory
{
    bool memIsInit;
    
    u64 permanentStorageSize;
    void *permanentStorage;
    
    u64 transientStorageSize;
    void *transientStorage;
};

typedef struct v3 
{
    union
    {
        struct
        {
            f32 r;
            f32 g;
            f32 b;
        };
        struct
        {
            f32 x;
            f32 y;
            f32 z;
        };
    };
}v3;

#define v3(x,y,z) v3_create(x,y,z)

v3 v3_create(f32 x, f32 y, f32 z)
{
    v3 result;
    
    result = {x, y, z};
    
    return result;
}

typedef struct v2 
{
    union
    {
        f32 xy[2];
        struct
        {
            f32 x;
            f32 y;
        };
    };
}v2;

#define v2(x,y) v2_create(x,y)

v2 v2_create(f32 x, f32 y)
{
    v2 result;
    result = {x,y};
    return result;
}


struct Invader
{
    v2 p;
    f32 ddPlayer;
    bool isAlive;
};


struct Sprite
{
    bool loop;
    u32 numFrames;
    u32 frameDuration;
    u32 time;
    
    s8 *frame[2];
};

struct Bullet
{
    v2 p;
    v2 size;
    bool fireAvailable;
};

struct Player
{
    v2 p;
    u8 lives[3];
    Bullet bullet;
};

enum enemiesTypes
{
    enemyType_invader,
    enemyType_ghost,
    enemyType_mydak,
    enemyType_boss,
    
    enemyType_count
};

struct Enemies
{
    v2 p;
    v3 color;
    f32 ddPlayer;
    v2 minBoundary;
    v2 maxBoundary;
    
    bool isAlive;
    enemiesTypes type;
};


struct memory_arena
{
    sizet size;
    u8 *base;
    sizet used;
};

#define MAX_BULLETS 128



struct game_state
{
    memory_arena gameArena;
    Player player;
    Enemies enemies[34];
    Sprite sprite[enemyType_count];
    sizet numBullets;
    Bullet bullets[MAX_BULLETS];
};


#define GAME_UPDATE_AND_RENDER(name) void name(game_memory *memory,GameBackbuffer *backbuffer,game_input *input)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
// TODO(shvayko): can I delete this?
GAME_UPDATE_AND_RENDER(game_update_and_render_stub)
{
}

#endif //HANDMADE_H