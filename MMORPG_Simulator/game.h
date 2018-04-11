//
//  MMORPG_Simulator
//
//  game.h
//  SDL_tutorial_c
//
//  Created by Eric Glover on 1/17/17.
//  Copyright © 2017 Glover Corp. All rights reserved.
//

//#ifndef game_h
//#define game_h
// Stephen's Tutorials (http://stephenmeier.net/)
//----------------------------------------------------------------------
#ifndef _GAME_H_
#define _GAME_H_

//----------------------------------------------------------------------
#define SCREEN_w 640
#define SCREEN_H 480
#define SCREEN_NAME "Prototype"
#define SCREEN_SCALE 1
#define SDL_MAIN_HANDLED
#define PASS    1
#define FAIL    0


//----------------------------------------------------------------------
#include "SDL2/SDL.h"
#include <SDL2_ttf/SDL_ttf.h>

//----------------------------------------------------------------------

//function prototypes
void game_init(void);
void game_quit(void);

//SDL_Texture* gfx_buildSpr(int w, int h, int** grid);
//void gfx_destroySpr(SDL_Texture* spr);

//void screen_drawSpr(SDL_Texture* spr, SDL_Rect* rect);



struct window {
    // define "attributes"
    SDL_bool running;
    
    struct {
        unsigned int w;
        unsigned int h;
        const char* name;
        SDL_Window* window;
        SDL_Renderer* renderer;
        //void (*drawSpr)(SDL_Texture* spr, SDL_Rect* rect);
    } screen;
    struct {
        unsigned int n;
        SDL_Surface** spritesheet;
        SDL_Texture* (*buildSpr)(int w, int h, int** grid);
        void (*destroySpr)(SDL_Texture* spr);
    } gfx;
    
    // define "methods"
    void (*init)(void);
    void (*quit)(void);
}
/*Game = {
    SDL_FALSE,
    {
        SCREEN_SCALE*SCREEN_w,
        SCREEN_SCALE*SCREEN_H,
        SCREEN_NAME,
        NULL, NULL,
        screen_drawSpr
    },
    {
        SCREEN_SCALE*SCREEN_w,
        SCREEN_SCALE*SCREEN_H,
        "Stats",
        NULL, NULL,
        screen_drawSpr
    },
    {
        0,
        NULL,
        gfx_buildSpr,
        gfx_destroySpr
    },
    game_init,
    game_quit
};
 */


//functions
//----------------------------------------------------------------------
void game_init(void) {
    if(SDL_Init(SDL_INIT_EVERYTHING)!=0) {
        printf("SDL error -> %sn", SDL_GetError());
        exit(1);
    }
    
    unsigned int w = Game.screen.w;
    unsigned int h = Game.screen.h;
    const char* name = Game.screen.name;
    

    
    Game.screen.window = SDL_CreateWindow(
                                          name,
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          w, h, 0
                                          );
    
    Game.screen.renderer = SDL_CreateRenderer(
                                              Game.screen.window, -1,
                                              SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC
                                              );

    
    Game.statScreen.window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                              w, h, 0);
    Game.statScreen.renderer = SDL_CreateRenderer(Game.statScreen.window, -1, SDL_RENDERER_ACCELERATED);
    // Set size of renderer to the same as window
    SDL_RenderSetLogicalSize( Game.statScreen.renderer, Game.statScreen.w, Game.statScreen.h );
    
    // Set color of renderer to red
    //SDL_SetRenderDrawColor( Game.statScreen.renderer, 255, 0, 0, 255 );

    
    SDL_Surface* surface = SDL_LoadBMP("spritesheet.bmp");
    int n = ((surface->w/8)*(surface->h/8)+1);
    
    Game.gfx.n = n;
    Game.gfx.spritesheet = (SDL_Surface**) malloc(sizeof(SDL_Surface*)*n);
    
    int i, x, y;
    SDL_Rect rect = {0, 0, 8, 8};
    for(i=0; i<n; i++) {
        Game.gfx.spritesheet[i] = SDL_CreateRGBSurface(0, 8, 8, 24, 0x00, 0x00, 0x00, 0x00);
        SDL_SetColorKey(Game.gfx.spritesheet[i], 1, 0xFF00FF);
        SDL_FillRect(Game.gfx.spritesheet[i], 0, 0xFF00FF);
        if(i!=0) {
            x = (i-1)%(surface->w/8);
            y = (i-x)/(surface->w/8);
            rect.x = x*8;
            rect.y = y*8;
            SDL_BlitSurface(surface, &rect, Game.gfx.spritesheet[i], NULL);
        }
    }
     
    
    SDL_FreeSurface(surface);

    
    Game.running = SDL_TRUE;
}

//----------------------------------------------------------------------
void game_quit(void) {
    int i;
    for(i=0; i<Game.gfx.n; i++)
        SDL_FreeSurface(Game.gfx.spritesheet[i]);
    free(Game.gfx.spritesheet);
    Game.gfx.spritesheet = NULL;
    
    SDL_DestroyRenderer(Game.screen.renderer);
    Game.screen.renderer = NULL;
    
    SDL_DestroyWindow(Game.screen.window);
    Game.screen.window = NULL;
    
    SDL_Quit();
}

//----------------------------------------------------------------------
/*
SDL_Texture* gfx_buildSpr(int w, int h, int** grid) {
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 8*w, 8*h, 24, 0x00, 0x00, 0x00, 0x00);
    SDL_SetColorKey(surface, 1, 0xFF00FF);
    SDL_FillRect(surface, 0, 0xFF00FF);
    
    int i, j;
    SDL_Rect rect = {0, 0, 8, 8};
    for(j=0; j<h; j++) {
        for(i=0; i<w; i++) {
            rect.x = i*8;
            rect.y = j*8;
            SDL_BlitSurface(Game.gfx.spritesheet[grid[j][i]], NULL, surface, &rect);
        }
    }
    
    SDL_Texture* tex = SDL_CreateTextureFromSurface(Game.screen.renderer, surface);
    SDL_FreeSurface(surface);
    
    return tex;
}

//----------------------------------------------------------------------
void gfx_destroySpr(SDL_Texture* spr) {
    SDL_DestroyTexture(spr);
}

//----------------------------------------------------------------------
void screen_drawSpr(SDL_Texture* spr, SDL_Rect* rect) {
    SDL_RenderCopy(Game.screen.renderer, spr, NULL, rect);
}

 */

#endif /* game_h */
