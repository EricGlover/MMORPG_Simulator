//
//  main.c
//  MMORPG_Simulator
//
//  Created by Eric Glover on 1/17/17.
//  Copyright © 2017 Glover Corp. All rights reserved.
//

//Eric Glover [and nominally, Paul Stumpe , Chris Stumpe ]
//Adventure Management Simulator PROTOTYPE
// 1/12/17

//TM T-Swift

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "ui.h"
#include "ai.h"
#include "database.h"
#include <SDL2_ttf/SDL_ttf.h>

//Status : database code done, fairly genric and modular; UI is not hard coded and inflexible but currently will update the stats reflect the ones in the database.
//GAME AI logic :  undone
    //game ai system, archetecture
    //main passes some sort of reference to the environment and the entity that will make a decision
        //AI makes a decision, constructs a decision thingy and passes to main
    //Main takes that decision and adds it to the action que
    //Main then sends AI the same stuff for a new entity, until all the entities have make their choices for that combat cycle (turn thing)
    //????????Main sorts the actions or something in the que to reflect priority??????
    //Main then sends a reference to the que to RunTurn() (the simulator) and it attempts to have all the decisions acted on simultaneously
        //depending on the combat system and UI there will be some decisions that can be make by heroes and monsters during actions


//UI :  **** need to code ui_close() **** also, need to rewrite quite a lot of the code to allow it to be modular

//Currently: implementing entity creation / updates from txt files
//TODO: TODO LIST
        //Redesign the UI-----
    // see ui, also write functions to close out all the space I've been mallocing
    //change updateViewport to use database
    // LATER change init viewports
        //other
    //conisder re-implementing the nameDictionary
    //look into the memory leaks
    //do the AI
    //implement reproduction stats
        //UI
    //program the main game screen
    //need to code ui_close()

        //Combat System
    //needs to be designed possibly before the AI


//note: I moved practically all of the macros and stuff into main.h

//use enums for game state tracking
enum state {MAIN, SIMULATOR};
enum state gameState = MAIN;
#define TRUE    1
#define FALSE   0
#define PASS    1
#define FAIL    0


char ***dictionary;

//viewports //static for the moment
//char viewports[3][10][19];

//an array of viewports, viewports are the string arrays of the hero struct - UI stuff
char ***viewports;

int main(int argc, const char * argv[]){
    //SDL shit
    if(ui_init() == FAIL) {
        puts("Erro in ui.init()");
        return 0;
    }
    SDL_Event event;
    //make a GameScreen
    GameScreen* statScreen;
    gameScreenInit(&statScreen);
    initDatabases();
    
    //renderText(char *text, GameScreen);
    //renderText("Hello World", statScreen);
    
    
    //int x = Game.statScreen.w/2, y = Game.statScreen.h/2;
    //SDL_Rect rect = {0, 0, 8*2, 8*2};
 
    
    //rect.x = 0+x, rect.y = 0+y;
    
    //move all this shit between here and the game loop to a function
    srand(time(NULL));
    FILE *output;
    char string[100];
    //char ***dictionary;
    char **german;
    char **russian;
    char **french;
    
    //testDatabase();
    
    
    
    
    //heroes
    Hero *rogue = createEntity(HERO);
    Hero *knight = createEntity(HERO);
    Hero *gnome = createEntity(HERO);
    Hero *dancer = createEntity(HERO);
    //Hero *karlTheLamma = createHero();      //loading a hero from database when there are none left results in a hero struct with junk data
    
    //view port shit    --UI stuff
    initViewports(&viewports);
    updateViewport(rogue, viewports[0]);
    updateViewport(knight, viewports[1]);
    updateViewport(gnome, viewports[2]);
    
    //create monsters
    //populateNameDictionary(german, russian, french);
    dictionary = populateNameDictionary();
    german = dictionary[0];
    russian = dictionary[1];
    french = dictionary[2];
    
    Monster *slug1 = createEntity(MONSTER);
    Monster *slug2 = createEntity(MONSTER);
    Monster *tinyHydra = createEntity(MONSTER);
    
    //create items
    Item *wand = createEntity(ITEM);
    Item *dagger = createEntity(ITEM);
    Item *stick = createEntity(ITEM);
    
    //create rooms
    Room *room1 = malloc(sizeof(Room)); //TODO: free this later
    
    
    //here's probably where we'll have our party selection stuff later..
    room1->monsters[0] = slug1;
    room1->monsters[1] = slug2;
    room1->monsters[2] = tinyHydra;
    
    room1->heroes[0] = knight;
    room1->heroes[1] = rogue;
    room1->heroes[2] = gnome;
    //room1->heroes[3] = dancer;
    
    //create a few heroes (JSON, txt file or hard code?)
    //later add generation
    
    
    //create dungeon asset
    //game loop
    //select heroes
    //run dungeon with heroes
    //print out results every turn
    
    printf("<----------WELCOME TO THE FABULOUS PROTOTYPE---------->\n");
    //do setup shit
    //game loop : while ( getInput && runGame() )
    int length = 0;
    while(statScreen->running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: {
                    statScreen->running = SDL_FALSE;
                } break;
            }
        }
        
        //probably export this entity decision stuff to a separate function
        //entity decision()
        //get reference to all the entities to make decisions, pass references to ai.c
            //use getI() and getListLength() for this, just loop through each list probably
        length = 0;
        //rinse and repeat for monsters
        length = getListLength(HERO);
        for (int i = 0; i < length; i++) {
            //pass getI(i) to AI and pass the environment
            decide(getI(i, HERO), HERO, room1);
        }
        
        //updateDatabase(HERO); //mode change
        updateEntities(HERO);
        //view port shit
        updateViewport(rogue, viewports[0]);
        updateViewport(knight, viewports[1]);
        updateViewport(gnome, viewports[2]);
        
        //renderViewport(viewports[0], "firstView", statScreen);
        //renderViewport(viewports[1], "secondView", statScreen);
        renderAllViewPorts(viewports, statScreen);
        SDL_Delay(1000);
        
        
        /*if (runGame(getInput())) {
            
        }else {
            //throw to some quit game functions
        }*/
    //    SDL_RenderClear(Game.statScreen.renderer);
        
    //    rect.x = 0+x, rect.y = 0+y;
        //updateViewport(room1->heroes[0], viewports[0]
        //renderText("Hello World! Are you ready for my dank memes?", statScreen);
        //renderText("Dank Memes", statScreen);
        //renderText("Fred : Knight\nStats : health 100", statScreen);
    }
    printf("GAME OVER\n");
    
    ui_close();
    
    
    /*----------*/
    //Game AI
    //design, character
    //should chars and monsters have the same abstract class?
    //character desires
    //greed, survival, party members survival, urge to kill (?)
    //stats
    //stats : health armor attack crit% intelligence
    
    //desire check then planning algorithms then actions
    
    
    
    return 0;
}

char * getInput(){
    char *input;
    char c;
    int i;
    i = 0;
    input = malloc(sizeof(char) * 100);
    //printf("What you do? : \n");
    gets(input);
    //printf("your input is : %s\n", input);
    return input;
}

int runGame(char *input){
    printf("Your input : %s\n", input);
    
    //switch statement for game modes
    switch (gameState) {
        case MAIN:
            printf("run Game says gameState is MAIN\n");
            //mainMenu(input);
            return mainMenu(input);
            break;
        case SIMULATOR :
            puts("running simulator in runGame");
            return simulator(input);
            break;
        default :
            //nothing
            puts("Default error");
    }
    //later program in a command array and a switch system like in simple.c
    //for now just use some if elses
    /*
     if (!strcmp(input, "quit")){
     return 0;
     }else if (!strcmp(input, "print all")){
     printf("Printing out all Objs\n");
     //code here
     }else if (!strcmp(input, "run dungeon1")){
     printf("Running first dungeon\n");
     //select heroes
     //run first room
     //run second room
     }else{
     return 1;
     }
     */
    return 0;
}

//LEGACY CODE   , used to be used for the menu system in the simulation (?)
int simulator(char *input){
    
    if (!strcmp(input, "quit")){
        return 0;
    }else if (!strcmp(input,"print heroes")){
        //print heroes
    }else if (!strcmp(input,"print monsters")){
        //print monsters
    }else if (!strcmp(input,"")){
        //run sim
        runSim();
    }else if (!strcmp(input, "run")){
        //run sim
        runSim();
    }
    return 1;
}

//uncoded, was planned to be a turn of the simulation I believe
void runSim(){
    //run game ai logics
    //execute a turn
    //update any necessary files
}

//probably legacy code at this point, self explanatory though
int mainMenu(char *input){
    //later program in a command array and a switch system like in simple.c
    //for now just use some if elses
    if (!strcmp(input, "quit")){
        return 0;
    }else if (!strcmp(input, "print all")){
        printf("Printing out all Objs\n");
        //code here
        return 1;
    }else if (!strcmp(input, "run dungeon1")){
        printf("Running first dungeon\n");
        //select heroes
        gameState = SIMULATOR;
        return 1;
    }else{
        return 1;
    }
    return 0;
}
/*
Monster * createMonster(char ***dictionary, char *class, int health, int armor, int attack, int crit, int intel){
    Monster *monster = malloc(sizeof(Monster));
    char *string = NULL;
    
    string = dictionary[ (rand() % 3)][ (rand() % 20)];
    strcpy(monster->name, string);
    strcpy(monster->h_class, class);
    
    
    monster->stats[0] = health;
    monster->stats[1] = armor;
    monster->stats[2] = attack;
    monster->stats[3] = crit;
    monster->stats[4] = intel;
    
    return monster;
}
*/
//frees space for the monster name dictionary and sets dictionary to the values found in "NameDictionary.txt"
char *** populateNameDictionary(void){
    FILE *dictionaryText;
    dictionaryText = fopen("NameDictionary.txt", "r");
    char **german;
    char **russian;
    char **french;
    char ***dictionary;
    char c;
    char empty[100];
    int z, i;
    z = 0;
    i = 0;
    
    dictionary = malloc(sizeof(char**) * 3);
    german = malloc(sizeof(char*) * 20);
    dictionary[0] = german;
    russian = malloc(sizeof(char*) * 20);
    dictionary[1] = russian;
    french = malloc(sizeof(char*) * 20);
    dictionary[2] = french;
    for (i = 0; i < 3; i++){
        while( (c = fgetc(dictionaryText) ) != '\n' ){
            //skip first line
        }
        for (size_t j = 0; j < 20; j++){
            dictionary[i][j] = malloc( sizeof(char) * 40);
            while( (c = fgetc(dictionaryText) ) != '\n' ){
                //printf("%c", c);
                dictionary[i][j][z] = c;
                //printf("%c", dictionary[i][j][z]);
                z++;
            }
            z = 0;
            //puts("");
            //printf("german[j] = %s\n", dictionary[i][j]);
        }
    }
    //printf("size of dictionary is %lu\n", sizeof(dictionary));
    
    return dictionary;
}

//frees up the appropriate space for the viewport string arrays, UI stuff
//probably want to migrate this over to ui.c
void initViewports(char ****viewportsPtr){
    int i = 0, j = 0;
    char ***viewports;
    viewports = malloc(sizeof(char **) * 3);        //only three heroes so far
    viewports[0] = malloc(sizeof(char *) * 10);     //only 10 lines of stats so far
    viewports[1] = malloc(sizeof(char *) * 10);
    viewports[2] = malloc(sizeof(char *) * 10);
    for (i = 0; i < 3; i++){
        for(j = 0; j < 10; j++){
            viewports[i][j] = malloc(sizeof(char) * 20);        //each line has 19 chars + \0
            memset(viewports[i][j], ' ', 19);       //init all strings to null byte, just cause
            viewports[i][j][19] = '\0';
        }
    }
    printf("viewports[0][0] is %s\n", viewports[0][0]);
    *viewportsPtr = viewports;
}

//updates the viewport string arrays to reflect a hero's current state
//probably want to migrate this over to ui.c
void updateViewport(Hero *hero, char **viewport){
    
    char string[20];        //19 chars + \0
    strcpy(string, hero->name);
    //later if you want to set the class far to the right you can for loop add " " until hero->name.len() + i = 19
    strcat(string, " ");
    strcat(string, hero->h_class);
    strcpy(viewport[0], string);
    
    //stats
    sprintf(viewport[1], "health          %3d", hero->stats[0]);
    sprintf(viewport[2], "armor           %3d", hero->stats[1]);
    sprintf(viewport[3], "attack          %3d", hero->stats[2]);
    sprintf(viewport[4], "crit            %3d", hero->stats[3]);
    sprintf(viewport[5], "intelligence    %3d", hero->brain.intelligence);
    
    //desires
    sprintf(viewport[6], "survival        %3d", hero->brain.desires[0]);
    sprintf(viewport[7], "greed           %3d", hero->brain.desires[1]);
    sprintf(viewport[8], "party love      %3d", hero->brain.desires[2]);
    
}
/*  //TODO: move this to ui.c
//alternate code attempt
void updateViewport(Hero *hero, char **viewport){
    
    char string[20];        //19 chars + \0
    strcpy(string, hero->name);
    //later if you want to set the class far to the right you can for loop add " " until hero->name.len() + i = 19
    strcat(string, " ");
    strcat(string, hero->h_class);
    strcpy(viewport[0], string);
    
    //stats
    sprintf(viewport[1], "health          %3d", hero->stats[0]);
    sprintf(viewport[2], "armor           %3d", hero->stats[1]);
    sprintf(viewport[3], "attack          %3d", hero->stats[2]);
    sprintf(viewport[4], "crit            %3d", hero->stats[3]);
    sprintf(viewport[5], "intelligence    %3d", hero->stats[4]);
    
    //desires
    sprintf(viewport[6], "survival        %3d", hero->desires[0]);
    sprintf(viewport[7], "greed           %3d", hero->desires[1]);
    sprintf(viewport[8], "party love      %3d", hero->desires[2]);
    
}
*/


