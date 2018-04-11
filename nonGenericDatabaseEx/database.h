//
//  database.h
//  MMORPG_Simulator
//
//  Created by Eric Glover on 1/23/17.
//  Copyright © 2017 Glover Corp. All rights reserved.
//

#ifndef database_h
#define database_h

#include <stdio.h>

//#include "main.h"     //I just ripped main.h out and pasted it
//
//  main.h
//  MMORPG_Simulator
//
//  Created by Eric Glover on 1/17/17.
//  Copyright © 2017 Glover Corp. All rights reserved.
//
//1/12/17
//game #1

//TM T-Swift

//test1.h
//maybe use enums for this?
#define STAT_NUM 5
#define DESIRES_NUM 3
#define LINES_PER_HERO_ENTRY 12     //not including the blank separating line between entries

#define ITEM_STATS_NUM          5
#define LINE_PER_ITEM_ENTRY     19
#include <SDL2_ttf/SDL_ttf.h>


//  the normal working structure
struct hero {
    //to use ptrs or array declar here ? oh the questions..
    int loaded;     //0 for no, 1 for yes       //used to see if you've loaded this from the database already
    char name[100];
    char h_class[100];

    //stats : health armor attack crit% intelligence
    int stats[STAT_NUM];
    //desires : greed, survival, party members survival
    int desires[DESIRES_NUM];
};
struct item {
    int loaded;
    char name[100];
    char kind[100];

    //attributes that are synced to the hero class
    int statEffects[STAT_NUM];
    int desireEffects[DESIRES_NUM];

    //item specific attributes
    char rarity[100];
    //value, shinyness, durability, dropChance, visible (0 or 1?)
    int itemStats[ITEM_STATS_NUM];
    //should position stuff be in here?
};

struct monster {
    char *name;
    char *h_class;

    //stats : health armor attack crit% intelligence
    int stats[STAT_NUM];
};

struct room {
    struct monster *monsters[10];
    struct hero *heroes[10];

};

struct format {
    int lines;  //12
    int strings;
    int ints;
    int order[LINES_PER_HERO_ENTRY];

};

typedef struct hero Hero;
typedef struct monster Monster;
typedef struct item Item;
typedef struct room Room;
typedef struct format Format;





//function prototypes

//Hero * createHero(char *name, char *class, int health, int armor, int attack, int crit, int intel, int greed, int survival, int partyLove);
//Hero * createHero();
//void updateHeroDb(void);
void printHero(Hero*);

Monster * createMonster(char ***dictionary, char *class, int health, int armor, int attack, int crit, int intel);
void printMonster(Monster*);

//void populateNameDictionary(char[20][100], char[20][100], char[20][100]);
char *** populateNameDictionary(void);
void initViewports(char ****viewports);
void updateViewport(Hero *hero, char **viewport);

int main(int argc, const char * argv[]);
int mainMenu(char *);
int simulator(char *);

void runSim(void);

char * getInput(void);
int runGame(char input[]);

/* main.h*/


/*Database.h*/

#define DATA_DUMP   FALSE
#define DATABASE_KEY    FALSE

struct heroNode {
    Hero *node;
    struct heroNode *next;
};
struct itemNode {
    Item *node;
    struct itemNode *next;
};
struct monsterNode {
    Monster *node;
    struct MonsterNode *next;
};


typedef struct heroNode HeroNode;
typedef struct itemNode ItemNode;
typedef struct monsterNode MonsterNode;

//externally defined node functions
Hero *getHeroI(int);
Item *getItemI(int); //is this necessary?
Monster *getMonsterI(int);

void testDatabase(void);


//internal functions
int findNum(char *);        //used for parsing files - extracts a number from a string
char *extractAndReplace(char *string, int number, int replacement);     //used for parsing files



//later try and change these to
//void update(char *entity);
    /*where the call from main would look like this
        update("heroes");
     */
//void create(char *entity);
    /*where the call from main would look like this
        Hero *knight = (Hero *) create("hero");
     */
//same thing for the other functions
//the functions above would probably end up calling the specific function below after doing some strcmp's
//the functions prototypes would be moved into database.c to protect the namespace

void updateHeroDb(void);    //update the database to reflect the hero values
void updateHeroes(void);    //update the heroes to reflect the values in the database
Hero * createHero(void);    //read the first unloaded hero into a new Hero struct

//the analogous functions
void updateItemDb(void);
void updateItems(void);
void updateItems(void);
void createAllItems(void);  //just init all the items in one place, use getItemI to find them later?
//Item* createItem(void);
void* createItem(int type);

void updateMonsterDb(void);
void updateMonsters(void);
//Monster * createMonster(void);    //replace the current one in main later
