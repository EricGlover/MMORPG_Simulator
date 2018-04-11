//
//  ui.h
//  MMORPG_Simulator
//
//  Created by Eric Glover on 1/19/17.
//  Copyright © 2017 Glover Corp. All rights reserved.
//

#ifndef ui_h
#define ui_h

#include <stdio.h>
#include "SDL2/SDL.h"
#include <SDL2_ttf/SDL_ttf.h>
//#include "main.h"

//----------------------------------------------------------------------
#define SCREEN_w 820
#define SCREEN_H 480
#define SCREEN_NAME "Prototype"
#define SCREEN_SCALE 1
#define SDL_MAIN_HANDLED

//function prototypes
int ui_init(void);
void ui_close(void);


struct gameScreen {
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
        //SDL_Surface** spritesheet;
        //SDL_Texture* (*buildSpr)(int w, int h, int** grid);
        //void (*destroySpr)(SDL_Texture* spr);
    } gfx;
    
    // define "methods"
    //void (*init)(void);
    //void (*quit)(void);
};

typedef struct gameScreen GameScreen;

//internal functions, these really shouldn't be in my header file 
int setupTTF( const char*);
SDL_Texture* createTextTextures(char *, SDL_Renderer*);
void gameScreenInit(GameScreen** gameScreen);
void renderViewport( char **viewport, char *, GameScreen *);
void renderAllViewPorts( char ***viewports, GameScreen *gameScreen);

//external functions
void renderText(char *text, GameScreen* );

#endif /* ui_h */
