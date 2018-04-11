//
//  ui.c
//  MMORPG_Simulator
//
//  Created by Eric Glover on 1/19/17.
//  Copyright © 2017 Glover Corp. All rights reserved.
//


//----------------------------------------------------------------------
//#define SCREEN_w 640
//#define SCREEN_H 480
#define SCREEN_NAME "Prototype"
#define SCREEN_SCALE 1
#define SDL_MAIN_HANDLED
#define PASS    1
#define FAIL    0

//----------------------------------------------------------------------
#include "ui.h"
#include <SDL2_ttf/SDL_ttf.h>

// Stuff for text rendering
TTF_Font* font;      //TTF_Fonts are a struc accessing the structures themselves is discouraged by the makers
SDL_Color textColor = { 255, 255, 255, 255 }; // white
SDL_Color backgroundColor = { 0, 0, 0, 255 }; // black
char *fontName = "Pokemon GB.ttf";
int ptSize = 10;

SDL_Texture* text;

SDL_Rect solidRect = {50 , 50 , 50 , 50 };
SDL_Rect rect = {0,0,0,0};      //using this to pass h parameters from createTextTexture to RenderViewport
//currently the viewports split the screen into thirds 
SDL_Rect firstView = {0,0, SCREEN_w / 3, SCREEN_H};
SDL_Rect secondView = {SCREEN_w / 3, 0, SCREEN_w / 3, SCREEN_H};
SDL_Rect thirdView = {((SCREEN_w / 3 ) * 2), 0, SCREEN_w / 3, SCREEN_H};
int linesPerView = 10;

SDL_Rect windowRect = { 900, 300, 400, 400 };

SDL_Texture *textures[10];

//GameScreen gSList[5];       //definitely do this better later
//you could write this like this
GameScreen* gSlist[5];  //then malloc space for each screen later...

//it's probably a good idea to keep a list of assets in ui.c somewheres
    //ie : surfaces, textures, windows, renderers, game/gameScreen structs



//----------------------------------------------------------------------
int ui_init(void) {
    //init everything the UI could need
    int success = PASS;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        puts("Error in SDL_init()");
        success = FAIL;
    }
    if(setupTTF(fontName) == -1){
        puts("Error in TTF_Init()");
        success = FAIL;
    }
    
    return success;
}

void ui_close(void) {
    //close all ui assets
    
}


//----------------------------------------------------------------------
void gameScreenInit(GameScreen** gameScreenPtr){
    
    //find space
    GameScreen *gameScreen;
    gameScreen = malloc(sizeof(GameScreen));
    //set the ptr you were given to this address
    *gameScreenPtr = gameScreen;
    //create window
    //gameScreen->screen.window = SDL_CreateWindow("Prototype", SCREEN_w / 2, SCREEN_H / 2, SCREEN_w / 2, SCREEN_H, SDL_WINDOW_SHOWN );
    gameScreen->screen.window = SDL_CreateWindow( "Prototype", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_w, SCREEN_H, SDL_WINDOW_SHOWN );
    //SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    //create renderer
    gameScreen->screen.renderer = SDL_CreateRenderer(gameScreen->screen.window, -1, SDL_RENDERER_ACCELERATED);
    
    //don't quite know why we have these in screen
    gameScreen->screen.w = SCREEN_w / 2;
    gameScreen->screen.h = SCREEN_H / 2;
    gameScreen->running = SDL_TRUE;
    
    //add this to an array?
    gSlist[0] = gameScreen;
}
 
 //
 int setupTTF( const char* chosenFont){
    //this was declared earlier in the global namespace
    //TTF_Font* font;      //TTF_Fonts are a struc accessing the structures themselves is discouraged by the makers
    //
 
    // SDL2_TTF needs to be initialized just like SDL2
    if ( TTF_Init() == -1 ){                //before you use TTF you need to init()
        puts("error in TTF_INIT");
        return FAIL;
    }
 
    // Load our fonts, with a huge size
    font = TTF_OpenFont(chosenFont, ptSize);          //takes file name, and pt size : returns a TTF_Font *
 
    // Error check
    if ( font == NULL  ){
        //std::cout << " Failed to load font : " << SDL_GetError() << std::endl;
        puts("failed to load font");
        return FAIL;
    }
 
    return PASS;
 }
 
 SDL_Texture* createTextTextures(char *diplayText, SDL_Renderer* renderer){
    SDL_Surface* solid = TTF_RenderText_Solid( font, diplayText, textColor );  //ttf dows all the hard work making our text into a surface
    SDL_Texture* text;
 
    text = SDL_CreateTextureFromSurface( renderer, solid ); //create a texture
    rect.w = solid->w;
    rect.h = solid->h;
 
    SDL_FreeSurface( solid );   //cause ?
 
 
    SDL_QueryTexture( text, NULL, NULL, &solidRect.w, &solidRect.h );   //no idea
    solidRect.x = 0;
    solidRect.y = 0;
 
 
    return text;
 }

void renderText(char *text, GameScreen *gameScreen){         //consider making you specify the rect stuff in function call
    SDL_Renderer *renderer = gameScreen->screen.renderer;
    //take text and convert it into a texture
    SDL_Texture *texture = createTextTextures(text, renderer);
    
    SDL_RenderClear(renderer);

    /*
    //use the textures array to find your texture and render it
    //this very doesn't work
    for (size_t i = 0; i < 10; i++) {
        if (texture == textures[i]){
            //do nothing
            printf("Found an equivalent texture\n");
            SDL_Rect rect = {i*100, 0, 100, 18};
            SDL_RenderCopy(renderer, textures[i], NULL, &rect);
            break;
        }else if (textures[i] == NULL) {
            printf("Found blank texture\n");
            textures[i] = texture;
            SDL_Rect rect = {i*100, 0, 100, 18};
            SDL_RenderCopy(renderer, textures[i], NULL, &rect);
            break;
        }
    }
    */
    
    //check the rect array and the text count? or array
    //make a rectangle for display stuffz?
    //SDL_Rect rect = {0, 0, 192, 17};
    
    //clear renderer and render to screen
    //SDL_SetRenderDrawColor(renderer, backgroundColor, <#Uint8 g#>, <#Uint8 b#>, <#Uint8 a#>)
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    //SDL_RenderCopy(renderer, texture, NULL, NULL);//attempting with the rectangle of death
    SDL_RenderPresent(renderer);
    
    //test access for render draw color
   // printf("Render draw color is %d %d %d %d\n", backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
}

//void renderViewport( char **viewport){
void renderAllViewPorts( char ***viewports, GameScreen *gameScreen){
    int i = 0;
    char *viewArgs[3] = { "firstView", "secondView", "thirdView"};
    SDL_RenderClear( gameScreen->screen.renderer );
    for (i = 0; i < 3; i++) {
        renderViewport(viewports[i], viewArgs[i], gameScreen);
    }
    SDL_RenderPresent(gameScreen->screen.renderer);
    
}
void renderViewport( char **viewport, char *name, GameScreen *gameScreen){
    //you're sent a string array of the text to render
    SDL_Renderer *renderer = gameScreen->screen.renderer;
    SDL_Rect line = {0, 0, 0, 0};
    SDL_Texture *textT;
    //which viewport ui part do you render to ?
    if ( !strcmp(name, "firstView") ){
        //use firstView Rect when you set viewport
        SDL_RenderSetViewport(renderer, &firstView);
    }else if ( !strcmp(name, "secondView")){
        //use secondView Rect
        SDL_RenderSetViewport(renderer, &secondView);
    }else if ( !strcmp(name, "thirdView")){
        //use thirdView Rect
        SDL_RenderSetViewport(renderer, &thirdView);
    }else {
        puts("Error in finding viewport");
    }
    //get the text, line by line
        //later probably program it to check a struct for the string to be rendered and if we already have a texture just reuse it
    int texture_h = 0, i = 0;
    //SDL_RenderClear(renderer);
    for (i = 0; i < linesPerView; i ++){               //currently there are only 10 lines
        textT = createTextTextures(viewport[i], renderer);
        //setup your line rectangle
        texture_h = rect.h;      //risk it for the biscuit
        line.w = rect.w;    //consider changing this
        line.y = i * texture_h;
        line.h = texture_h;
        //render to screen
        //should I use renderText()? should I clear the Renderer?
        SDL_RenderCopy(renderer, textT, NULL, &line);
    }
    //SDL_RenderPresent(renderer);
    
}




