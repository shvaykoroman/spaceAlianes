#include "handmade.h"
#include "windows.h"
#include "pacman_math.h"

void clearBackbuffer(GameBackbuffer *backbuffer)
{
    memset(backbuffer->memory, 0, backbuffer->width * backbuffer->height * 4);
}

inline s32
roundFromFloatToInt(f32 x)
{
    s32 result = s32(x+0.5);
    return result;
}

void drawRectangle(GameBackbuffer *backbuffer,  v2 playerPos, v2 playerSize, v3 color)
{
    
    s32 minX = roundFromFloatToInt(playerPos.x);
    s32 maxX = minX + roundFromFloatToInt(playerSize.x);
    s32 minY = roundFromFloatToInt(playerPos.y);
    s32 maxY = minY + roundFromFloatToInt(playerSize.y);
    
    if(minX < 0)
    {
        minX = 0;
    }
    if(maxX > backbuffer->width)
    {
        maxX = backbuffer->width;
    }
    if(minY < 0)
    {
        minY = 0;
    }
    if(maxY > backbuffer->height)
    {
        maxY = backbuffer->height;
    }
    s8 *row = ((s8*)backbuffer->memory + minX*4)  + (minY*backbuffer->stride);
    
    u32 colorsrc = (roundFromFloatToInt(color.r * 255.f) << 16) | (roundFromFloatToInt(color.g * 255.f) << 8) | (roundFromFloatToInt(color.b * 255.f) << 0);
    
    for(s32 y = minY; y < maxY; y++)
    {
        u32 *pixel = (u32*)row;
        for(s32 x = minX; x < maxX; x++)
        {
            *pixel++ = colorsrc;
        }
        row += backbuffer->stride;
    }
    
}

void
drawEntity(GameBackbuffer *backbuffer, Entity *entity)
{
  
  for(s32 y = 0; y < entity->height; y++)
    {
      for(s32 x = 0; x < entity->width; x++)
	{
	  if(entity->invader[y*entity->width+x])
            {
	      v2 pos = v2(entity->pos.x + entity->size.x * x,entity->pos.y + entity->size.y * y);
	      drawRectangle(backbuffer,  pos, entity->size, entity->color);
            }
	}
    }
  
}

internal bool  
checkCollision(v2 firstObjMin, v2 firstObjMax, v2 secondObjMin, v2 secondObjMax)
{
    bool result = false;
    
    if(firstObjMin.x < secondObjMin.x + secondObjMax.x &&
       firstObjMin.x + firstObjMax.x > secondObjMin.x &&
       firstObjMin.y < secondObjMin.y + secondObjMax.y &&
       firstObjMin.y + firstObjMax.y > secondObjMin.y)
    {
        result = true;
    }
    return result;
}

internal s8*
updateAnimation(game_state *gameState,u32 animation,u32 currentFrame)
{
    s8 *sprite = gameState->sprite[animation].frame[currentFrame];
    return sprite;
}

void
initAliens(game_state *gameState)
{   
    for(s32 entityIndex = 0; entityIndex < MAX_ENTITIES - 1; entityIndex++)
      {
	s32 id = entityIndex / 11;

	switch(id)
	  {
	  case 0:
	    {
	      gameState->entities[entityIndex].pos      = v2(200+50.f*entityIndex,150.f);
	      gameState->entities[entityIndex].width    = 11;
	      gameState->entities[entityIndex].height   = 8;	      
	      gameState->entities[entityIndex].isAlive  = true;
	      gameState->entities[entityIndex].type     = entityType_invader;
	      gameState->entities[entityIndex].color    = v3(1.f,1.f,1.f);
	      gameState->entities[entityIndex].invader  = gameState->invader;
	      gameState->entities[entityIndex].size     = v2(3.f,3.f);
	    }break;
	  case 1:
	    {
	      gameState->entities[entityIndex].pos      = v2(200+50.f*entityIndex,150.f);
	      gameState->entities[entityIndex].width    = 11;
	      gameState->entities[entityIndex].height   = 8;	      
	      gameState->entities[entityIndex].isAlive  = true;
	      gameState->entities[entityIndex].type     = entityType_ghost;
	      gameState->entities[entityIndex].color    = v3(0.f,1.f,1.f);
	      gameState->entities[entityIndex].invader  = gameState->ghost;
	    }break;
	  case 2:
	    {
	      gameState->entities[entityIndex].pos      = v2(200+50.f*entityIndex,150.f);
	      gameState->entities[entityIndex].width    = 11;
	      gameState->entities[entityIndex].height   = 8;	      
	      gameState->entities[entityIndex].isAlive  = true;
	      gameState->entities[entityIndex].type     = entityType_mydak;
	      gameState->entities[entityIndex].color    = v3(1.f,1.f,0.f);
	      gameState->entities[entityIndex].invader  = gameState->mydak;
	
	    }break;
	  default:
	    {
	      invalidCodePath();
	    }break;
	  }	
      }
}

internal void
initArena(memory_arena *arena, sizet size, u8 *base)
{
  arena->size = size;
  arena->base = base;
  arena->used = 0;
}

#define pushStruct(arena,type)    (type *)pushSize_(arena,sizeof(type))
#define pushArray(arena,type,num) (type *)pushSize_(arena,(num)*sizeof(type))

internal void*
pushSize_(memory_arena *arena, sizet size)
{
    
    void *result = arena->base + arena->used;
    arena->used += size;
    return result;
}



extern "C" GAME_UPDATE_AND_RENDER(gameUpdateAndRender)
{
    // TODO(shvayko):memory arena
    game_state *gameState = (game_state*)memory->permanentStorage;
    
    s8 invader[8][11] = 
    {
        {0,0,1,0,0,0,0,0,1,0,0},
        {0,0,0,1,0,0,0,1,0,0,0},
        {0,0,1,1,1,1,1,1,1,0,0},
        {0,1,1,0,1,1,1,0,1,1,0},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,0,1,1,1,1,1,1,1,0,1},
        {1,0,1,0,0,0,0,0,1,0,1},
        {0,0,0,1,1,0,1,1,0,0,0},
    };
    
    
    s8 invader1[8][11] = 
    {
        {0,0,1,0,0,0,0,0,1,0,0},
        {1,0,0,1,0,0,0,1,0,0,1},
        {1,0,1,1,1,1,1,1,1,0,1},
        {1,1,1,0,1,1,1,0,1,1,1},
        {0,1,1,1,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,1,1,1,0,0},
        {0,0,1,0,0,0,0,0,1,0,0},
        {0,0,0,1,1,0,1,1,0,0,0},
    };
    
    
    s8 player[9][13] = 
    {
        {0,0,0,0,0,0,1,0,0,0,0,0,0},
        {0,0,0,0,0,1,1,1,0,0,0,0,0},
        {0,0,0,0,0,1,1,1,0,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,0},
        {1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1},
    };
    
    s8 ghost[8][13] = 
    {
        {0,0,0,1,0,0,0,0,0,0,0,0,0},
        {0,0,1,1,1,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,0,0,0,0,0,0,0},
        {1,1,0,1,0,1,1,0,0,0,0,0,0},
        {1,1,1,1,1,1,1,0,0,0,0,0,0},
        {0,0,1,0,1,0,0,0,0,0,0,0,0},
        {0,1,0,1,0,1,0,0,0,0,0,0,0},
        {1,0,1,0,1,0,1,0,0,0,0,0,0},
    };
    
    
    s8 ghost1[8][13] = 
    {
        {0,0,0,1,0,0,0,0,0,0,0,0,0},
        {0,0,1,1,1,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,0,0,0,0,0,0,0},
        {1,1,0,1,0,1,1,0,0,0,0,0,0},
        {1,1,1,1,1,1,1,0,0,0,0,0,0},
        {0,1,0,1,1,0,1,0,0,0,0,0,0},
        {1,0,0,0,0,0,0,1,0,0,0,0,0},
        {0,1,0,0,0,0,1,0,0,0,0,0,0},
    };
    
    
    s8 mudak[8][12] = 
    {
        {0,0,0,0,1,1,1,1,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,1,1,0},
        {1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,0,0,1,1,0,0,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1},
        {0,0,1,1,1,0,0,1,1,1,0,0},
        {0,1,1,0,0,1,1,0,0,1,1,0},
        {0,0,1,1,0,0,0,0,1,1,0,0} 
    };
    
    
    s8 mudak1[8][12] = 
    {
        {0,0,0,0,1,1,1,1,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,1,1,0},
        {1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,0,0,1,1,0,0,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1},
        {0,0,0,1,1,0,0,1,1,0,0,0},
        {0,0,1,1,0,1,1,0,1,1,0,0},
        {1,1,0,0,0,0,0,0,0,0,1,1}
    };
    
    s8 boss[7][15]
    {
        {0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
        {0,0,0,1,1,1,1,1,1,1,1,1,0,0,0},
        {0,0,1,1,1,1,1,1,1,1,1,1,1,0,0},
        {0,1,1,0,1,1,0,1,0,1,1,0,1,1,0},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {0,0,1,1,1,0,0,1,0,0,1,1,1,0,0},
        {0,0,0,1,0,0,0,0,0,0,0,1,0,0,0}
    };
    
    
    
    
    if(!memory->memIsInit)
    {
        initArena(&gameState->gameArena, memory->permanentStorageSize - sizeof(gameState),(u8*)memory->permanentStorage+sizeof(game_state));
        
        
        
        gameState->player.p = v2(backbuffer->width*0.5f, backbuffer->height-35.f);
        gameState->player.bullet.p = gameState->player.p;
        gameState->player.bullet.size  = v2(2,20);
        gameState->player.bullet.fireAvailable = true;

	gameState->invader = (s8*)invader;
	gameState->ghost   = (s8*)ghost;
	gameState->mydak   = (s8*)mudak;
	
	
	
        gameState->sprite[0].loop = true;
        gameState->sprite[0].numFrames = 2;
        gameState->sprite[0].frameDuration = 10;
        gameState->sprite[0].time = 10;
        gameState->sprite[0].frame[0] = (s8*)invader;
        gameState->sprite[0].frame[1] = (s8*)invader1;
        
        gameState->sprite[1] = gameState->sprite[0];
        gameState->sprite[1].frame[0] = (s8*)ghost;
        gameState->sprite[1].frame[1] = (s8*)ghost1;
        
        gameState->sprite[2] = gameState->sprite[0];
        gameState->sprite[2].frame[0] = (s8*)mudak;
        gameState->sprite[2].frame[1] = (s8*)mudak1;
        
        initAliens(gameState);
        memory->memIsInit = true;
    }
    
    
    
    game_controller_input *controller = &input->controllers[0]; 
    
    
    
    
    f32 ddPlayer = 0;
    if(controller->buttonRight.endedDown)
    {
        ddPlayer = 1.f;
    }
    if(controller->buttonLeft.endedDown)
    {
        ddPlayer = -1.f;
    }
    if(controller->buttonEnter.endedDown)
    {
        //NOTE(shvayko): spawn bullet
        
        if(gameState->player.bullet.fireAvailable != false)
        {
            gameState->player.bullet.p = v2(gameState->player.p.x+17.f,gameState->player.p.y-12.f);
            gameState->player.bullet.fireAvailable = false;
            
        }
    }
    ddPlayer *= 128;
    gameState->player.p.x = gameState->player.p.x + ddPlayer*input->dtForFrame;
    
    clearBackbuffer(backbuffer);    
    
    drawEntity(backbuffer, &gameState->entities[0]);
    
    for(u32 entityIndex = 0; entityIndex < arrayCount(gameState->entities); entityIndex++)
    {
        if(gameState->entities[entityIndex].isAlive)
        {
            // NOTE(shvayko): updating the animation 
            // TODO(shvayko): do animation for all types of alienes!
	  u32 currentFrame = gameState->sprite[0].time / gameState->sprite[0].frameDuration;
            
            
            if((s32)gameState->entities[entityIndex].pos.x == 940)
            { 
                gameState->movement = toLeft;
                gameState->entities[entityIndex].pos.y += 30;
            }
            else if((s32)gameState->entities[entityIndex].pos.x == 0)
            {
	      gameState->movement = toRight;
                gameState->entities[entityIndex].pos.y += 30; 
            }	    
            gameState->entities[entityIndex].pos.x +=  gameState->movement*input->dtForFrame;
	    
            switch(gameState->entities[entityIndex].type)
	      {
	      case entityType_invader:
                {
		  s8* sprite = updateAnimation(gameState,entityType_invader,currentFrame);
		  //drawInvader(backbuffer,sprite, gameState->entity[entityIndex].pos, gameState->entity[entityIndex].color);
                }break;
	      case entityType_ghost:
                {
		  s8* sprite = updateAnimation(gameState,entityType_ghost,currentFrame);
		  //drawGhost(backbuffer, sprite, gameState->entity[entityIndex].pos,
		  // gameState->entities[entityIndex].color);
		    
                }break;
	      case entityType_mydak:
                {
		  s8* sprite = updateAnimation(gameState,entityType_mydak,currentFrame);
		  //drawMudak(backbuffer, sprite, gameState->entity[entityIndex].pos, gameState->entity[entityIndex].color);
                    
                }break;
	      case entityType_boss:
                {
		  //drawBoss(backbuffer, (s8*)boss, gameState->entity[entityIndex].pos, gameState->entity[entityIndex].color);
                    
                }break;
	      default:
                {
		  invalidCodePath();
                }
	      }
        }
    }
    
    // TODO(shvayko): collision detection with enemies and with arena
    if(!(gameState->player.bullet.p.y <= 0)
       && (!gameState->player.bullet.fireAvailable))  
    {
        gameState->player.bullet.p.y -= 10; // NOTE(shvayko): speed of the bullet
        drawRectangle(backbuffer, gameState->player.bullet.p, gameState->player.bullet.size,v3(1.f,0.0f, 0.0f));
    }
    else
    {
        gameState->player.bullet.fireAvailable = true;
    }
    
    for(u32 entityIndex = 0; entityIndex < arrayCount(gameState->entities); entityIndex++)
    {
        if(checkCollision(gameState->entities[entityIndex].pos,
                          v2(7*5, 12*3),
                          gameState->player.bullet.p,
                          gameState->player.bullet.size)
           && (gameState->entities[entityIndex].isAlive))
        {
            // TODO(shvayko): explosion
            gameState->entities[entityIndex].isAlive = false;
            gameState->player.bullet.fireAvailable = true;
            gameState->player.bullet.p = gameState->player.p;
            break;
        }
    }
    
    ++gameState->sprite[0].time;
    if(gameState->sprite[0].time == (gameState->sprite[0].numFrames * gameState->sprite[0].frameDuration))
    {
        if(gameState->sprite[0].loop) gameState->sprite[0].time = 0;
    }
    
    //drawPlayer(backbuffer,  (s8*)player, gameState->player.p, v3(0.f,1.f,0.f));
}

