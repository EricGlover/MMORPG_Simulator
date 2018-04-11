//
//  ai.c
//  MMORPG_Simulator
//
//  Created by Eric Glover on 2/5/17.
//  Copyright © 2017 Glover Corp. All rights reserved.
//

#include <stdlib.h>
#include <time.h>
#include "ai.h"
#include "main.h"

//functino prototypes
int choseDesire(Brain *brain);

//passed an entity and an environment (room struct probably)

/*      for reference
struct brain {
    int intelligence;
    //desires : greed, survival, party members survival
    int desires[DESIRES_NUM];
};
*/


//how should I reference the other entities in the game? should main through them to me or should there be some kind of static room I could refer to
    //should room be able to be different from the database linked lists???

//should I receive the entire entity or just a pointer to their brain???
int decide(void *entity, int type, Room *room) {
    //for the moment decisions are just coded as numbers, because i don't even know
    int desChoice = -1;
    
    //how do I access their brain?
    //Hero *hero = (Hero *) entity;
    Hero *hero = NULL;
    Monster *monster = NULL;
    Brain *brain = NULL;
        //a rough and ready approach but an effective one
    switch (type) {
        case HERO:
            hero = (Hero *) entity;
            brain = &(hero->brain);
            break;
        case MONSTER:
            monster = (Monster *) entity;
            brain = &(monster->brain);
            break;
        default:
            puts("Error, can't find the brain in decision() AI");
            break;
    }
    
    //pick a desire
    desChoice = choseDesire(brain);
    
    
    
    //make a plan, should their be separate functions for the different desires?
    //maybe plan[desChoice]( ); //TODO: set up plan function array and code
    
    //find an implementation
    
    return 0;
}

//should return the index of the chosen desire in the desires array
int choseDesire(Brain *brain){
    int choice = -1;
    int highest = -1;
    int rando = 0;
    int range = 5;
    
    //TODO: look into using microsoft random number generator and linux's random() etc..
    //using srand to seed and rand() later for now
    srand(time(NULL));
    
    
    //loop through des_num and decide on a desire
    for (int i = 0; i < DESIRES_NUM; i++) {
        //rando should be +- range, also srand was already seeded
        //TODO: see using random() instead of rand() effects portability???
            //bug, the -1 part yields values -2, and -1; instead of 1 and -1
        rando = ( ( rand() % range) + 1 ) * ( -1 * ((rand() % 2) + 1));       //last rand sets rando to - half the time, pos the other half,
                    //rando's range is +- range
        printf("rando is %d\n", rando);
        if (brain->desires[i] + rando > highest) {
            choice = i;
        }
    }
    
    
    return choice;
}
