//
//  main.h
//  MMORPG_Simulator
//
//  Created by Eric Glover on 1/17/17.
//  Copyright © 2017 Glover Corp. All rights reserved.
//

#ifndef main_h
#define main_h

//1/12/17
//game #1

//TM T-Swift


#define HERO    0
#define ITEM    1
#define MONSTER 2


//maybe use enums for this?
//#define STAT_NUM 5    changed for the brain structs
#define STAT_NUM 4
#define DESIRES_NUM 3
#define REPRODUCTION_STAT_NUM 5
#define REPRODUCTION    1
//#define LINES_PER_HERO_ENTRY 12     //not including the blank separating line between entries
//#define LINES_PER_HERO_ENTRY 17     //not including the blank separating line between entries

#define ITEM_STATS_NUM          5
#define LINE_PER_ITEM_ENTRY     18
#define ITEM_KINDS              5

#define BAG_SIZE            10      //inventory capacity

#define MAX_LINES_OF_ANY_ENTRY  19

#include <SDL2_ttf/SDL_ttf.h>


//TODO: consider changing char[]'s in structs to pointers
//TODO: MAKE A CONDITIONALLY DEFINED STRUCT HERO FOR EASE OF TURNING ON AND OFF REPRODUCTION MODE

//new design choice, new systems like AI, and Actions, etc will have unique structs that all entities using them will have
    //example : anything using the AI system will have a brain struct, all the AI code is solely dependent upon the brain structure
        //the actions system will probably be dependent on the stats struct (which is basically the same as the stats from before)

//
struct brain {
    int intelligence;
     //desires : greed, survival, party members survival
    int desires[DESIRES_NUM];
};
struct hero {
    //to use ptrs or array declar here ? oh the questions..
    int loaded;     //0 for no, 1 for yes       //used to see if you've loaded this from the database already
    char name[100];
    char h_class[100];
    
    //stats : health armor attack crit% ///intelligence
    int stats[STAT_NUM];
    struct brain brain;
    
    int reproductionStats[REPRODUCTION_STAT_NUM];
    
    //TODO: GIVE THEM MONEY
    //TODO: ADD AN INVENTORY
};

struct item {
    int loaded;
    char name[100];
    char kind[100];
    
    //attributes that are synced to the hero class
    int statEffects[STAT_NUM];
    //int desireEffects[DESIRES_NUM];   //should this have a brain struct cause that feels weird
    struct brain brainEffects;
    
    //item specific attributes
    char rarity[100];
    //value, shinyness, durability, dropChance, visible (0 or 1?)
    int itemStats[ITEM_STATS_NUM];
    //should position stuff be in here?
};

struct monster {
    int loaded;
    char name[100];
    char h_class[100];
    
    //stats : health armor attack crit% intelligence
    int stats[STAT_NUM];
    struct brain brain;
    
    int dropRates[ITEM_KINDS];
    
    struct item* inventory[BAG_SIZE];
    //TODO: FIX THE WEIRD #INCLUDE .H BUGS
    //Node *inventoryList; #include "database.h" makes database.h freak out on compilation so no Nodes for the moment
    
    //should unique drop actually have pointers to the kind of items? or instances of that particular item?
    //struct
    char uniqueDropItemKind[100];

};

struct room {       //TODO: consider giving database control over room structs, and making CRUD functions for them
    struct monster *monsters[10];   //TODO: consider changing this to some kind of a linked list structure later??
    struct hero *heroes[10];        //TODO: consider changing this to some kind of a linked list structure later??
    struct item *items[10];         //TODO: consider changing this to some kind of a linked list structure later??
};

struct format {
    int lines;
    int strings;
    int ints;
    int order[MAX_LINES_OF_ANY_ENTRY];
    
};

typedef struct brain Brain;
typedef struct hero Hero;
typedef struct monster Monster;
typedef struct item Item;
typedef struct room Room;
typedef struct format Format;





//function prototypes
char *** populateNameDictionary(void);
void initViewports(char ****viewports);
void updateViewport(Hero *hero, char **viewport);

int main(int argc, const char * argv[]);
int mainMenu(char *);
int simulator(char *);

void runSim(void);

char * getInput(void);
int runGame(char input[]);



#endif /* main_h */
