//
//  database.h
//  MMORPG_Simulator
//
//  Created by Eric Glover on 1/23/17.
//  Copyright © 2017 Glover Corp. All rights reserved.
//

/* INSTRUCTIONS
 STEPS TO MODIFY EXISTING DATATYPE ("CLASS") STRUCTURES AND HAVE DATABASE.C DEAL WITH YOUR INSANITY:
 1. Change the struct in main.h ; example : change struct hero { blah } ;
 2. Change MAX_LINES_OF_ANY_ENTRY if  (new Datatype Database Entry Total Line # > MAX_LINES_OF_ANY_ENTRY), if so set max to your new line count
 3. Change the appropriate format struct in database.c (it's at the top of the file) ; example change heroFormat
    -read the notes on the format structs that are there
 4. Change the read() and set() functions in database.c that are appropriate for your struct
    -obviously DON'T CHANGE THEIR RETURN TYPE OR ARGS TYPE OR ARGS NUMBER
    example : to change hero, change heroRead() and heroSet()
 
        ////////////////////////////
    
STEPS TO ADD AN ADDITIONAL DATABASE
 ??????
 1. create the structure in main.h
 2. change max_lines_of_any_entry if need be
 3. create a format structure and add it to the format list
 4. add the new database name to the list
 5. add the new node** topX to the appropriate nodeList
 6. write xSet(), xRead(), and xCreate(), add these functions into the lists of set read create (or whatever they're exact names are)
 7. add the macro for the type : example #define CLOUD  3, make sure this is the same as the index for it's set read create etc.. position in the lists
 ????anything else?????
 
 */
 /*                 */

#ifndef database_h
#define database_h

#include <stdio.h>
#include "main.h"

#define DATA_DUMP   FALSE
#define DATABASE_KEY    FALSE

struct node {
    void *data;
    struct node *next;
};
typedef struct node Node;

//externally defined node functions
extern void* getI(int i, int type);
extern int getListLength(int type);

void testDatabase(void);

//do I need other parts of my problem to have access to the lists??
    //probs not, use getEntI for shit
//entity Lists
/*extern Node *topH;
extern Node *topI;
extern Node *topM;
extern Node **lists[3];*/


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

void initDatabases(void);

void *createEntity(int type);
void updateDatabase(int type);
void updateEntities(int type);

//what was this supposed to be?
void createAllItems(void);  //just init all the items in one place, use getItemI to find them later?

#endif /* database_h */
