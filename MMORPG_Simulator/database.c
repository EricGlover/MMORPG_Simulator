//
//  database.c
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

//TODO: BUGLIST
    /*Known bugs
     1. handling negative stat values, note that the database while show them because they're (the dash '-') interpreted as chars
     2. Memory leaks, probably
     */
#include "database.h"

#define TRUE    1
#define FALSE   0

/*Format heroFormat = {
    12, 2, 9,           //number of lines per entry, stringArgs, intArgs
    {                   //order[], 1 is type int, 0 is type string, -1 is ignore it
        1,0,0,           //loaded, name, class
        1,1,1,1,1,       //stats
        1,1,1,           //desires
        -1               //blank line
    }
};*/
Format heroFormat = {
    17, 2, 14,           //number of lines per entry, stringArgs, intArgs
    {                   //order[], 1 is type int, 0 is type string, -1 is ignore it
        1,0,0,           //loaded, name, class
        1,1,1,1,          //stats : 4
        1,1,1,1,           //brainEffects : 4 (intelligence / desires)
        1,1,1,1,1,       //reproduction stats
        -1               //blank line
    }
};


//these format values are incorrect on both item and monster, just using for testing
Format itemFormat = {
    18, 3, 14,           //number of lines per entry, stringArgs, intArgs
    {                   //order[], 1 is type int, 0 is type string, -1 is ignore it
        1,0,0,           //loaded, name, kind
        1,1,1,1,       //statEffects  : 5
        1,1,1,1,           //brainEffects : 4 (intelligence / desires)
        0,               //rarity
        1,1,1,1,1,        //itemStats
        -1               //blank line
    }
};
Format monsterFormat = {
    19, 4, 14,           //number of lines per entry, stringArgs, intArgs
    {                   //order[], 1 is type int, 0 is type string, -1 is ignore it
        1,0,0,           //loaded, name, class
        1,1,1,1,       //stats        4
        1,1,1,1,           //desires/intelligence      4
        1,1,1,1,1,       //drop rates : 5
        0,               //inventory not implemented but has to be string type currently to not get fucked up
        0,               //unique drop kind : string
        -1               //blank line
    }
};

Format *entityFormat[3] = {&heroFormat, &itemFormat, &monsterFormat};

//TO-DO :
    //oddly enough I believe I was having problems using readkey()
    //do the stuff it says to do in database.h
    //set it up so that you can declare heroes from the database??? is this a good idea?

//STATUS : OPERATIONAL

//internal functions ( getters, setters, creaters, and deleters ) for each datatype
void *heroCreate(void);
void *itemCreate(void);
void *monsterCreate(void);
void *(*create[3])(void) = {heroCreate, itemCreate, monsterCreate };

void heroSet(int index, void *value, void *entity);     //consider doing this for multiple values, you could do variable args
void itemSet(int index, void *value, void *entity);
void monsterSet(int index, void *value, void *entity);
void (*set[3])(int, void *, void *) = { heroSet, itemSet, monsterSet };

void *heroRead(int index, void *entity);
void *itemRead(int index, void *entity);
void *monsterRead(int index, void *entity);
void *(*readEnt[3])(int, void *) = { heroRead, itemRead, monsterRead };

//note that there are no delete functions for each datatype, this is handled by the node function delete()

//internal functions
int findNum(char *);        //used for parsing files - extracts a number from a string
char *extractAndReplace(char *string, int number, int replacement);     //used for parsing files
void* getEntI(int i, Node **topEntity);

//node functions
void insert(Node **topEntity, void *entity);
int delete(Node **topEntity, void **entity);
int isEmpty(int listType);      //pass it HERO ITEM OR MONSTER
void printList(Node **top, int type);
/////////

///internal database functions and things

int readKey(char *mode, FILE *, ... );
int findDigits(int number);
    //calls readKey("read only", FILE *);
    //readKey("read and write", FILE *, FILE *);

//entity databases
FILE *heroDb;
FILE *monsterDb;
FILE *itemDb;
//entity database names
char *hDbName = "heroDatabase.txt";
char *iDbName = "itemDatabase.txt";
char *mDbName = "monsterDatabase.txt";
char *dBNames[3] = {"heroDatabase.txt", "itemDatabase.txt", "monsterDatabase.txt" };        //consider using the paste macro for this ##

//entity Lists
Node *topH = NULL;
Node *topI = NULL;
Node *topM = NULL;
Node **lists[3] = {&topH, &topI, &topM};


//necessary database format for this to work : one line per attribute, the only numbers in the database are the values of the hero, there must be a \t or whitespace separating the numbers from the words, a blank line between entries, for attributes that are strings have only the attribute value in that line, for the moment no junk text inbetween entries is allowed (do to updateHeroDb() and probably createHero() ), also no junk text at the top of the database...
//createHero reads the values of the first unloaded hero from the database into a newly made hero struc and returns a pointer to it


///**************       TESTING FUNCTION   *************************///////////////////


void testDatabase(void){
    /*printf("isEmpty() == %d\n", isEmpty());
    printHeroList();
    Hero *knight = malloc(sizeof(Hero));
    insert(knight);
    printf("isEmpty() == %d\n", isEmpty());
    printHeroList();
    delete(knight);
    printHeroList();
     */
    /*
    Hero *wizard = malloc(sizeof(Hero));
    strcpy(wizard->name, "Wizard");
    Hero *rogue = malloc(sizeof(Hero));
    strcpy(rogue->name, "Rogue");
    Hero *sona = malloc(sizeof(Hero));
    strcpy(sona->name, "Sona");
    insert(wizard);
    insert(rogue);
    printHeroList();
    insert(sona);
    printHeroList();
    */
    //testing item helper functions: I believe they all pass
    /*
    //1
    Item *wand = malloc(sizeof(Item));
    strcpy(wand->name, "Wand");
    isEmpty(ITEM);
    printItemList();
    insertItem(wand);       //seems to pass
    printItemList();
    deleteItem(&wand);
    
    //2
    Item *ruby = malloc(sizeof(Item));
    strcpy(ruby->name, "Ruby");
    wand = malloc(sizeof(Item));      //errors on insert item ruby when you have this setup for some reason
        //investigating
    strcpy(wand->name, "Wand");
    insertItem(wand);
    insertItem(ruby);
    printItemList();
    
    
    //3
    Item *dancePotion = malloc(sizeof(Item));
    strcpy(dancePotion->name, "Dance Potion");
    insertItem(dancePotion);
    printf("%s is the ith item\n", getItemI(1)->name);
    printf("%s is the ith item\n", getItemI(0)->name);
    printf("%s is the ith item\n", getItemI(3)->name);  //out of index
    printItemList();
    deleteItem(&ruby);
    printItemList();
    insertItem(ruby);
    printItemList();
    deleteItem(&wand);
    */
    
    
    /*
    int num = -9;
    num = findNum("This dank ass string 100\n");
    num = findNum("1");
    num = findNum("Health :22\n");
    */
    /*
    char test[100];
    char *string = "health:100";
    strcpy(test, extractAndReplace(string, findNum(string), 7) );
    string = malloc(sizeof(char) * 100);
    strcpy(string, "dank:   8\n");
    strcpy(test, extractAndReplace(string, findNum(string), 7) );
    string = malloc(sizeof(char) * 100);
    strcpy(string, "0");
    strcpy(test, extractAndReplace(string, findNum(string), 7) );
    string = malloc(sizeof(char) * 100);
    strcpy(string, "\\\\\\\\\\");
    strcpy(test, extractAndReplace(string, findNum(string), 7) );
    string = malloc(sizeof(char) * 100);
     */
    /*TESTING*/
    /*char testLine[100];
    char *string = testLine;
    
    //strcpy(string, testLine);
    printf("size of testLine %lu, size of string %lu\n", sizeof(testLine), sizeof(string));
    printf("size of *string = %lu\n", sizeof((*string)) );
    printf(" strlen(string) + 1 * char = %lu\n", (sizeof(char) * (strlen(string) +1)) );
    memset(testLine, 'z', 100);
    printf("size of testLine %lu, size of string %lu\n", sizeof(testLine), sizeof(string));
    printf("size of *string = %lu\n", sizeof((*string)) );
    printf(" strlen(string) + 1 * char = %lu\n", (sizeof(char) * (strlen(string) +1)) );
    string = malloc((sizeof(char)) * 100);
    memset(string, 'z', 100);
    printf("size of testLine %lu, size of string %lu\n", sizeof(testLine), sizeof(string));
    printf("size of *string = %lu\n", sizeof((*string)) );
    printf(" strlen(string) + 1 * char = %lu\n", (sizeof(char) * (strlen(string) +1)) );
    strcpy(string, testLine);
    printf("size of testLine %lu, size of string %lu\n", sizeof(testLine), sizeof(string));
    printf("size of *string = %lu\n", sizeof((*string)) );
    printf(" strlen(string) + 1 * char = %lu\n", (sizeof(char) * (strlen(string) +1)) );
    */
    
    
    
    
    /*int num = 0;
    num = findNum("DankMemes>9000\n");
    extractAndReplace("DankMemes>9000\n", num, 1);*/
    /*
    Hero *rogue = createHero();       //TESTING , REMEMBER TO CHANGE DATABASE ENTRY MANAULLY TO LOADED = 1 ON ROGUE AND KNIGHT
    Hero *knight = createHero();
    Hero *gnome = createHero();
    */
    //TESTING NEW FUNCTIONS USING VOID POINTERS
    
    
            //testing new node functions
    /*void printListZZZ(Node **top, int type);
     int deleteZZZ(void **topEntity, void *entity);
     void insertZZZ(Node **topEntity, void *entity);
     */     //topH topI topM
    /*
     /*
    Item *wand = (Item*) createItem(ITEM);
    for (int i = 1; i < 20; i++) {
        printf("wand line %d should be %d if int\n", i, (*(int *)itemRead(i, (void *) wand)) );
        printf("wand line %d should be %s if string\n", i, (char *)itemRead(i, (void *) wand) );
    }
     */
    /*
    int num = 0;
    char string[100];
    Hero *wand = (Hero *) createItem(HERO);
     for (int i = 1; i < 20; i++) {
         heroRead(i, (void *) wand, &num);
         readEnt[HERO](i, (void *) wand, &num);
         readEnt[HERO](i, (void *) wand, string);
         printf("wand line %d should be %d if int\n", i, num);
         printf("wand line %d should be %s if string\n", i, string);
         //printf("wand line %d should be %d if int\n", i, (*(int *)readEnt[ITEM](i, (void *) wand)) );
         //printf("wand line %d should be %s if string\n", i, (char *)readEnt[ITEM](i, (void *) wand) );
     }
     */
     /*
     Monster *wand = (Monster*) createItem(MONSTER);       //monster read function tests
     for (int i = 1; i < 20; i++) {
     printf("wand line %d should be %d if int\n", i, (*(int *)monsterRead(i, (void *) wand)) );
     printf("wand line %d should be %s if string\n", i, (char *)monsterRead(i, (void *) wand) );
     }
    ///
     */
    
     
    //newNum = (*(int *) readEnt[type](lineNum, entity) );
    //newNum = (int *) readEnt[type](lineNum, entity);
    //newNum = readEnt[type](lineNum, entity);
    updateDatabase(MONSTER);
    Monster *slug = (Monster *) createEntity(MONSTER);
    Monster *slug2 = (Monster *) createEntity(MONSTER);
    Monster *hydra = (Monster *) createEntity(MONSTER);
    
    
    updateDatabase(ITEM);
    Item *wand = (Item*) createEntity(ITEM);
    wand->loaded = 1;   //manually set to 1 in db while testing
    //TODO : fix updateDatabase for items
    Item *dagger = (Item *) createEntity(ITEM);
    dagger->loaded = 1;
    Item *stick = (Item *) createEntity(ITEM);
    stick->loaded = 1;
    
    //testing updateEntities
    updateEntities(ITEM);   //first see if it fucks the database
    updateEntities(ITEM);   //second fuck around with wand
    updateEntities(ITEM);   //switch wand with something in the database
    updateEntities(ITEM);   //switch it back
    updateEntities(ITEM);
    
    insert(&topI, wand);
    printList(&topI, ITEM);
    insert(&topI, dagger);
    printList(&topI, ITEM);
    insert(&topI, stick );
    printList(&topI, ITEM);
    
    delete(&topI, (void *) &wand);
    printList(&topI, ITEM);
    wand = (Item*) createEntity(ITEM);
    insert(&topI, wand);
    printList(&topI, ITEM);
    delete(&topI, (void *) &stick);
    printList(&topI, ITEM);
    delete(&topI, (void *) &wand);
    printList(&topI, ITEM);
    delete(&topI, (void *) &dagger);
    printList(&topI, ITEM);
    
    /*Item *one = (Item *)getEntI(0, &topI);
    Item *two = (Item *)getEntI(1, &topI);
    Item *three = (Item *)getEntI(2, &topI);
     */

    
    
    puts("BLAH");
    printf("%s is 0's name\n", ((Item *)(getEntI(3, &topI)))->name);
    puts("BLAH");
    printf("%s is 0's name\n", ((Item *)(getEntI(-1, &topI)))->name);
    puts("BLAH");
    
    
    Monster *ferdinand = (Monster *) createEntity(MONSTER);
    printf("%s is monster name\n", ferdinand->name);
    Hero *rogue = (Hero*) createEntity(HERO);
    //Item *wand = (Item*) createEntity(ITEM);
    //printf("%s is wand name\n", wand->name);
    
    
    /*
    printHero(getHeroI(0));
    printHero(getHeroI(1));
    printHero(knight);
    printHero(getHeroI(2));
    
    
    
    delete(rogue);
    delete(knight);
    delete(gnome);
    //printHeroList();
    
    //delete(wizard);
    printHeroList();
    updateHeroDb();
    
    //delete(sona);
    printHeroList();
    
    //delete(sona);
    printHeroList();
    */
    
    
    //need to set up a refresh system for terminal
    //testing updateHeroes()
    /*
    int i = 0;
    Hero *hero = NULL;
    while (1) {
        i = 0;
        updateHeroes();
        while ((hero = getHeroI(i)) != NULL) {
            i++;
            printHero(hero);
        }
    }
    
    */
    
}



///**************       DATABASE FUNCTIONS   *************************///////////////////

//used to make sure all the loaded values are set to 0
void initDatabases(void){
    int listLength = (sizeof(lists) / sizeof(Node **) );
    for (int i = 0; i < listLength; i++) {
        updateDatabase(i);
    }
}



void* createEntity(int type){
    int loaded = 0, lineNum = 1, data = 0, createdHero = 0;  //
    char line[100];
    char *foundEOF;
    int stat = 0, desire = 0;   //iterators
    //memcpy(line, 'z', 100);
    memset(line, 'z', 100);     //init line cause why not
    void *blank = NULL;
    
    
    //use type as the index for all the function arrays
    
    FILE *database = NULL;
    if ( (database = fopen(dBNames[type], "r") ) == NULL ) {            //if file open fails
        puts("Error in loading database...exiting and returning a blank");
        return blank;
    }
    
    //malloc space for generic entity
    void *entity = create[type]( );
    
    //find correct format pointer
    Format *eForm = entityFormat[type];
    int sNum = 0, iNum = 0, spareInt = 0;
    
doEntry:
    do {
        if ( (foundEOF = fgets(line, 100, database)) == NULL) {       //sets foundEOF to null for later if at the end of the file
            if (lineNum - 1 == eForm->lines){       //testing, it seems if the last line of a database is the end of an entry it spazzes out , maybe this'll fix it
                createdHero = 1;
                iNum = 0;
                sNum = 0;
                break;
                
            }
            break;      //break loop on EOF
        }
        if (eForm->order[ (lineNum-1) ] == 0) {
            //is string
            line[strlen(line) - 1] = '\0';  //clip the null terminator off
            void *stringz = line;
            set[type](lineNum, stringz, entity);
            sNum++;
        }else if (eForm->order[ (lineNum-1) ] == 1){
            //is int
            if (iNum == 0) {
                loaded = findNum(line);
                void *stringz = &loaded;
                set[type](lineNum, loaded, entity);
            }else{
                set[type](lineNum, (spareInt = findNum(line)), entity);
            }
            iNum++;
        }else if (lineNum == eForm->lines){
            createdHero = 1;
            iNum = 0;
            sNum = 0;
            break;
            
        }
        lineNum++;
        
        
    } while (loaded == 0);
    
    //broke out of the loop either because found EOF or loaded == 1
    if ( foundEOF == NULL) {      //could probably just use a char * to catch the return of this earlier but this works too
        if(!createdHero){
            //print error?
            puts("Couldn't find an unloaded hero in db, createHero() failure");
            fclose(database);
            return blank;
        }
    }else if (loaded == 1){
        //skip entry and goto doEntry       //skip line amount - 1
        for (int j = 0; j < eForm->lines - 1; j++) {
            if ( (fgets(line, 100, database)) == NULL) {
                puts("Couldn't find an unloaded hero in db, createHero() failure");
                fclose(database);
                return blank;    //if we run out of file without finding a hero print error and break
            }
        }
        desire = stat = 0;
        iNum = 0;
        sNum = 0;
        lineNum = 1;
        goto doEntry;
    }
    
    insert(lists[type], entity);
    set[type](1, 1, entity);
    
    //close all file pointers before calling this
    fclose(database);
    updateDatabase(type);
    
    return entity;
}


//run through the datatypes list and update the database to reflect those values
//for the moment there's one line of junk between entries
void updateDatabase(int type){
    //programmed generically
        //basic idea is you'll  read database correct it and write it to tmp, then reload and write tmp straight into a fresh database
            //open the database and a tmp file (database is needed to be opened to preserve formatting)
            //then use the format struct to decide whether to look for a number or just copy the attribute into tmp
    int lineNum = 1, createdHero = 0, i = 0;  //lineNum = 1 is pretty key
    char line[100];
    char string[100];
    char *foundEOF = NULL;
    char *newLine = NULL;
    char *str = NULL;
    //memset(line, 'z', 100);     //init line cause why not
    memset(line, '\0', 100);        //shouldn't be an issue to change from the above line to this one but check this later
    memset(string, '\0', 100);
    void *entity = NULL;
    int oldNum = 0, newNum = 0;
    //use type as the index for all the function arrays
    
    FILE *database = NULL;
    FILE *temp = NULL;
    if ( (database = fopen(dBNames[type], "r") ) == NULL ) {            //if file open fails
        puts("Error in loading database...exiting");
        return;
    }
    //open tmp file to write into
    if ( (temp = fopen("temp.txt", "w") ) == NULL) {
        puts("Error in opening the temp file...not handling the error");
        //close the other file
        fclose(database);
        return;
    }
    
    //TODO: CONSIDER IMPLEMENTING THE DATABASE_KEY FEATURE
        //at some point I stopped using this because it was creating bugs, don't remember what though
    /*
    if(DATABASE_KEY){
        heroesInDatabase = readKey("read and write", heroDb, temp);
    }
    */
    
    //find correct format pointer
    Format *eForm = entityFormat[type];
    int sNum = 0, iNum = 0;       //number of strings so far, number of ints so far, a spareInt value
    while ( ( (entity = getEntI(i , lists[type]) ) != NULL) && (foundEOF = fgets(line, 100, database) != NULL) ) {
        //don't check loaded, cause fuck it
        //do an entry
        //so you'll start on line 1
    doEntry:
        //
        do {
            if (eForm->order[ (lineNum-1) ] == 0) {
                //is string
                //copy hero string into database
                
                /* old way, wand of forgetfu\x34\x71 problem */
                //strcpy(string, (char *) readEnt[type](lineNum, entity) ); //here we have the problems
                //fprintf(temp, "%s", string);
                
                /*new way, seems riskier but works better, idk why*/
                str = (char *) readEnt[type](lineNum, entity);
                fprintf(temp, "%s", str);
                
                //add \n
                fprintf(temp, "\n");
                sNum++;
            }else if (eForm->order[ (lineNum-1) ] == 1){
                //is int
                oldNum = findNum(line);
                newNum = (*(int *) readEnt[type](lineNum, entity) );
                //treating all numbers the same including loaded
                newLine = extractAndReplace(line, oldNum, newNum);
                fprintf(temp, "%s", newLine);
                free(newLine);
                newLine = NULL;
                iNum++;
            }else if (lineNum == eForm->lines){
                //should I check that iNum and sNum are what they should be ? 1 over eForm->ints // eForm->strings ?
                createdHero = 1;
                oldNum = iNum = sNum = 0, newNum = 0;
                i++;
                lineNum = 1;
                fprintf(temp, "%s", line);
                break;
            }
            lineNum++;
            
            
        } while ( (foundEOF = fgets(line, 100, database)) != NULL );

    }
    //check which condition failed
        //you could be at the end of the file with no more heroes loaded or with heroes loaded
            //or out of heroes with leftover database to deal with
    //grab next line, testing
    foundEOF = fgets(line, 100, database);
    if ( (foundEOF == NULL) && (entity == NULL) ){
        //you're done
        //do nothing
    }else if (entity == NULL) {
        //change loaded to 0 for the rest of the entries, finish this section when you run out of database to read
        do {
            if (lineNum == 1) {
                //change load to 0
                fprintf(temp, "%d\n", 0);
                lineNum++;
            }else if (lineNum == eForm->lines){
                fprintf(temp, "%s", line);
                createdHero = 1;
                oldNum = iNum = sNum = 0, newNum = 0;
                lineNum = 1;
                i++;
            }else{
                fprintf(temp, "%s", line);
                lineNum++;
            }
        } while ((foundEOF = fgets(line, 100, database) != NULL));
    }else if (foundEOF == NULL){
        //write the leftover heroes into the database
        //pretty much the same could as above just ignoring the former code dealing with the old text
        //untested
        while ( (entity = getEntI(i , lists[type]) ) != NULL) {
            if (eForm->order[ (lineNum-1) ] == 0) {
                //is string
                strcpy(string, (char *) readEnt[type](lineNum, entity) );
                fprintf(temp, "%s", string);
                //add \n
                fprintf(temp, "\n");
                sNum++;
            }else if (eForm->order[ (lineNum-1) ] == 1){
                //is int
                newNum = (*(int *) readEnt[type](lineNum, entity) );
                newLine = malloc(sizeof(char) * 100);
                sprintf(newLine, "%d\n", newNum);
                fprintf(temp, "%s", newLine);
                //do I need to add a \n? i don't think so...
                free(newLine);
                newLine = NULL;
                iNum++;
            }else if (lineNum == eForm->lines){
                //should I check that iNum and sNum are what they should be ? 1 over eForm->ints // eForm->strings ?
                createdHero = 1;
                oldNum = iNum = sNum = 0, newNum = 0;
                i++;
                lineNum = 0;    //will be incremented in a second
            }
            lineNum++;
            fgets(line, 100, database);
        }
    }

    //now the database file should be written into temp, so close both files and copy temp.txt into the new database file
    fclose(database);
    fclose(temp);
    if ( (database = fopen(dBNames[type], "w") ) == NULL ) {            //if file open fails
        puts("Error in loading database...exiting");
        return;
    }
    temp = fopen("temp.txt", "r");
    
    while ( (fgets(line, 100, temp)) != NULL ){
        fprintf(database, "%s", line);
    }
    
    
    //now close temp and delete it
    fclose(temp);
    remove("temp.txt");
    
    //close the database file
    fclose(database);
}

//generically programmed
//update all the entities in a list to reflect the values found in their database
    //note : it is assumed that databse entries with loaded = 1 will all be at the top of the database file
    //note : it is assumed that enough entries will be in the database to cover what's loaded into the game
//further note : will assume that the first entity in the list is also the first entity in the database
void updateEntities(int type){
    //read database, while their are more entities in the list read an entry and update values
    
    int i = 0, iNum = 0, sNum = 0;
    int lineNum = 1;
    char line[100];
    memset(line, '\0', (sizeof(char) * 100) );
    void *entity = NULL;
    
    //get correct database
    FILE *database = NULL;
    if ( (database = fopen(dBNames[type], "r") ) == NULL ) {            //if file open fails
        puts("Error in loading database...exiting");
    }
    //get correct list
    Node **listTop = lists[type];
    
    //find correct format pointer
    Format *eForm = entityFormat[type];
    
    while ( (entity = getEntI(i, listTop)) != NULL )  {
        //do an entry
        while ( fgets(line , 100, database) ) {
            if (eForm->order[ (lineNum-1) ] == 0) {
                //is string
                line[strlen(line) - 1] = '\0';  //clip the new line char off ('\n' becomes '\0')
                void *stringz = line;
                //set[type](lineNum, stringz, entity);
                set[type](lineNum, line, entity);   //testing
                sNum++;
            }else if (eForm->order[ (lineNum-1) ] == 1){
                //is int
                set[type](lineNum, findNum(line), entity);  //check this
                iNum++;
            }else if (lineNum == eForm->lines){
                iNum = 0;
                sNum = 0;
                lineNum = 1;
                i++;    //get a new entity
                break;
            }
            lineNum++;
        }
    }
    
    //close file
    fclose(database);
}

///**************       GETTERS AND SETTERS FOR EACH DATATYPE   *************************///////////////////

//TODO: make the magic numbers in the getters and setters go away plix

//note this is not a creation function, should it be?
//use the format struct that's relevant combined with the attribute
//need to pass it which hero, so a hero pointer or index
//is index 1-11 or 0-10 ?????, setting it up to be 1-11 based
void heroSet(int index, void *value, void *entity){
    //probably need to change the declaration of the function on the void *data thing
    //so yeah set the data to a different pointer of the type that you actually want.
    Hero *hero = (Hero *) entity;
    int number = (int ) value;
    char *string = (char *) value;
    if (index == 1) {
        //loaded
        hero->loaded = number;
    }else if(index == 2){
        //name
        strcpy(hero->name, string);
    }else if (index == 3) {
        //class
        strcpy(hero->h_class, string);
    }else if (index <= STAT_NUM + 3){
        //this should be the int array stats part of the file
        hero->stats[index-4] = number;
    }else if (index <= STAT_NUM + 4 + DESIRES_NUM){
        //brain : intelligence, desires
        if (index == STAT_NUM + 4) {
            hero->brain.intelligence = number;
        }else{
            hero->brain.desires[index-(STAT_NUM+5)] = number;
        }
    }else if (index <= STAT_NUM + 4 + DESIRES_NUM + REPRODUCTION_STAT_NUM){
        hero->reproductionStats[index-(STAT_NUM+5+DESIRES_NUM)] = number;
    }else{
        puts("error in heroSet");
    }
}

//index corresponds to database line # 1-blah, not 0-blah
void itemSet(int index, void *value, void *entity){
    Item *item = (Item *) entity;
    int number = (int ) value;
    char *string = (char *) value;
    if (index == 1) {
        //loaded
        item->loaded = number;
    }else if(index == 2){
        //name
        strcpy(item->name, string);
    }else if (index == 3) {
        //class
        strcpy(item->kind, string);
    }else if (index <= STAT_NUM + 3){
        //this should be the int array stats part of the file
        item->statEffects[index-4] = number;
    }else if (index <= STAT_NUM + 4 + DESIRES_NUM){
        //brainEffects : intelligence, desires
        if (index == STAT_NUM + 4) {
            item->brainEffects.intelligence = number;
        }else{
            item->brainEffects.desires[index-(STAT_NUM+5)] = number;
        }
    }else if (index <= STAT_NUM + 5 + DESIRES_NUM){
        strcpy(item->rarity, string);
    }else if (index <= STAT_NUM + 6 + DESIRES_NUM + ITEM_STATS_NUM){
        item->itemStats[index - (STAT_NUM + 6 + DESIRES_NUM)] = number;
    }else{
        puts("error in itemSet");
    }
}

void monsterSet(int index, void *value, void *entity){
    Monster *monster = (Monster *) entity;
    int number = (int ) value;
    char *string = (char *) value;
    if (index == 1) {
        //loaded
        monster->loaded = number;
    }else if(index == 2){
        //name
        strcpy(monster->name, string);
    }else if (index == 3) {
        //class
        strcpy(monster->h_class, string);
    }else if (index <= STAT_NUM + 3){
        //this should be the int array stats part of the file
        monster->stats[index-4] = number;
    }else if (index <= STAT_NUM + 4 + DESIRES_NUM){
        //brain : intelligence, desires
        if (index == STAT_NUM + 4) {
            monster->brain.intelligence = number;
        }else{
            monster->brain.desires[index-(STAT_NUM+5)] = number;
        }
    }else if (index <= STAT_NUM + 4 + DESIRES_NUM + ITEM_KINDS){
        //drop rate
        //monster->itemStats[index - (STAT_NUM + 5 + DESIRES_NUM)] = number;
        monster->dropRates[index - (STAT_NUM + 5 + DESIRES_NUM)] = number;
    }else if (index <= STAT_NUM + 5 + DESIRES_NUM + ITEM_KINDS){
        //TODO: implement inventory system
        //Inventory skip for now
    }else if (index <= STAT_NUM + 6 + DESIRES_NUM + ITEM_KINDS){
        //unique drops
        strcpy(monster->uniqueDropItemKind, string);
    }else if (index <= STAT_NUM + 7 + DESIRES_NUM + ITEM_KINDS){
        //blank line skip it
    }else{
        puts("error in monsterSet");
    }

}

void *heroRead(int index, void *entity){
    Hero *hero = (Hero *) entity;
    int number = 0;
    //char *string = NULL;
    char string[100];
    memset(string, '\0', (sizeof(char) * 100));
    void *value = NULL;
    if (index == 1) {
        //loaded
        number = hero->loaded;
        value = &number;
    }else if(index == 2){
        //name
        strcpy(string, hero->name);
        value = string; //this seems all kind of weird
    }else if (index == 3) {
        //class
        strcpy(string, hero->h_class);
        value = string;
    }else if (index <= STAT_NUM + 3){
        //this should be the int array stats part of the file
        number = hero->stats[index-4];
        value = &number;
    }else if (index <= STAT_NUM + 4 + DESIRES_NUM){
        //brain : intelligence, desires
        if (index == STAT_NUM + 4) {
            number = hero->brain.intelligence;
        }else{
            number = hero->brain.desires[index-(STAT_NUM+5)];
        }
        value = &number;
    }else if (index <= STAT_NUM + 4 + DESIRES_NUM + REPRODUCTION_STAT_NUM){
        number = hero->reproductionStats[index-(STAT_NUM+5+DESIRES_NUM)];
        value = &number;
    }else{
        printf("Error in HeroRead\n");
    }
    
    return value;

}
void *itemRead(int index, void *entity){
    Item *item = (Item *) entity;
    int number = 0;
    char string[100];
    memset(string, '\0', (sizeof(char) * 100));
    void *value = NULL;
    
    if (index == 1) {
        //loaded
        number = item->loaded;
        value = &number;
    }else if(index == 2){
        //name
        strcpy(string, item->name);
        value = string;
    }else if (index == 3) {
        //class
        strcpy(string, item->kind);
        value = string;
    }else if (index <= STAT_NUM + 3){
        //this should be the int array stats part of the file
        number = item->statEffects[index-4];
        value = &number;
    }else if (index <= STAT_NUM + 4 + DESIRES_NUM){
        //brainEffects : intelligence, desires
        if (index == STAT_NUM + 4) {
            number = item->brainEffects.intelligence;
        }else{
            number = item->brainEffects.desires[index-(STAT_NUM+5)];
        }
        value = &number;
    }else if (index <= STAT_NUM + 5 + DESIRES_NUM){
        strcpy(string, item->rarity);
        value = string;
    }else if (index <= STAT_NUM + 6 + DESIRES_NUM + ITEM_STATS_NUM){
        number = item->itemStats[index - (STAT_NUM + 6 + DESIRES_NUM)];
        value = &number;
    }else{
        printf("Error in ItemRead\n");
    }
    
    return value;

}
void * monsterRead(int index, void *entity){
    Monster *monster = (Monster *) entity;
    int number = 0;
    char string[100];
    memset(string, '\0', (sizeof(char) * 100));
    void *value = NULL;
    
    if (index == 1) {
        //loaded
        number = monster->loaded;
        value = &number;
    }else if(index == 2){
        //name
        strcpy(string, monster->name);
        value = string;
    }else if (index == 3) {
        //class
        strcpy(string, monster->h_class);
        value = string;
    }else if (index <= STAT_NUM + 3){
        //this should be the int array stats part of the file
        number = monster->stats[index-4];
        value = &number;
    }else if (index <= STAT_NUM + 4 + DESIRES_NUM){
        //brain : intelligence, desires
        if (index == STAT_NUM + 4) {
            number = monster->brain.intelligence;
        }else{
            number = monster->brain.desires[index-(STAT_NUM+5)];
        }
        value = &number;
    }else if (index <= STAT_NUM + 4 + DESIRES_NUM + ITEM_KINDS){
        //drop rate
        number = monster->dropRates[index - (STAT_NUM + 5 + DESIRES_NUM)];
        value = &number;
    }else if (index <= STAT_NUM + 5 + DESIRES_NUM + ITEM_KINDS){
        //TODO: implement inventory system
        //Inventory skip for now
        strcpy(string, "NOT IMPLEMENTED YET");
        value = string;
    }else if (index <= STAT_NUM + 6 + DESIRES_NUM + ITEM_KINDS){
        //unique drops
        strcpy(string, monster->uniqueDropItemKind);
        value = string;
    }else if (index <= STAT_NUM + 7 + DESIRES_NUM + ITEM_KINDS){
        //blank line skip it
    }else{
        printf("Error in MonsterRead\n");
    }
    
    return value;

}


///**************       LINKED LIST FUNCTIONS   *************************///////////////////


//this is the API sort of function that is called from code external to database.c
void* getI(int i, int type) {
    return getEntI(i, lists[type]);
}

//returns list length starting from 1
int getListLength(int type) {
    //call getEntI until it returns null
    int i = 0;
    if (lists[type] == NULL) {
        return 0;
    }
    while ( (getEntI(i, lists[type]) ) != NULL) {
        i++;
    }
    return i;
    //return 1 + 1; should be correct but returns 1 extra, it's weird
}



void* getEntI(int i, Node **topNode) {
    Node *node = NULL;
    int q  = 0;
    Node *current = *topNode;
    
    if( *topNode == NULL){
        //puts("error couldnt get ith Ent. because the linked list is empty");  //commonly used to find the empty index of the lists so stop printing errors
        return NULL;
    }
    
    //walk through the items
    while (i != q) {
        q++;
        current = current->next;
        //this function is used for indexes outside of the bounds it returns a item pointing to NULL
        if (current == NULL) {  //otherwise we could start trying to access null pointers in the line above
            return NULL;
        }
    }
    node = current; //why?
    return node->data;
}
//if you prefer it this way later
/*
Node* getNodeI(int i, Node **topNode) {
    Node *node = NULL;
    int q  = 0;
    Node *current = *topNode;
    
    if( *topNode == NULL){
        puts("error couldnt get ith Ent. because the linked list is empty");
    }
    
    //walk through the items
    while (i != q) {
        current = current->next;
        //this function is used for indexes outside of the bounds it returns a item pointing to NULL
        if (current == NULL) {  //otherwise we could start trying to access null pointers in the line above
            return node;
        }
    }
    node = current;
    return node;
}
*/

//the generic version of the list functions
//don't pass it nothing because I can't check it.....well....
void insert(Node **topEntity, void *entity){
    if (entity == '\0') {       //equivalent to null    //tots works
        puts("can't insert a NULL into the list");
        return;
    }
    Node *new = malloc(sizeof(Node));
        
    if(new != NULL){
        new->data = entity;
        new->next = NULL;
            
        if (*topEntity == NULL) {        //change later isEmpty(ITEM)
            *topEntity = new;
        }else{
            Node *current = *topEntity;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new;
        }
    }else{
        puts("Wasn't enough space for the new item");
    }
}

void *heroCreate(void){
    Hero *hero = malloc(sizeof(Hero));
    //return (void *) hero;
    return hero;
}
void *itemCreate(void){
    Item *item = malloc(sizeof(Item));
    return item;
}
void *monsterCreate(void){
    Monster *monster = malloc(sizeof(Monster));
    return monster;
}

//return 1 if success, 0 if fail
//so this is a bit tricky but you need the entity pointer's address so you can set it to null otherwise you can end up
    //using that pointer after the memory has been freed
        //remember that void *entity ... entity = NULL, results in deleteZZZ setting it's own ptr in it's namespace called entity
int delete(Node **topEntity, void **entity){
    if (*topEntity == NULL) {
        puts("Error, the linked list is already empty");
        return 0;
    }else if (*entity == NULL){
        puts("Error, the entity can't be deleted because it's NULL");
        return 0;
    }
    Node *top = *topEntity;
    Node *current = top;
    Node *previous = NULL;
    while (current->data != *entity ) {
        previous = current;
        current = current->next;
        if(current == NULL) {
            puts("Couldn't delete entity from the linked list because it's not in the list");
            return 0;
        }
    }
    //in case the first thing in the list is being deleted, don't attempt to access a NULL ptr
    if (previous == NULL) {
        *topEntity = current->next;    //either null or actual heroNode
        top = NULL;     //testing
    }else{
        previous->next = current->next;
    }
    //free up the space and make sure nothing is pointing to there any more...
    //otherwise you can corrupt your heap...which is the sad times.
    free(current->data);
    free(current);
    *entity = NULL; //this should set calling ptr to null, testing
    current = NULL; //testing
    
    return 1;
}

void printList(Node **top, int type){
    if (*top == NULL) {
        puts("List is currently empty");
        return;
    }
    Node *current = *top;
    Hero *hero = NULL;
    Item *item = NULL;
    Monster *monster = NULL;
    while (current != NULL) {
        switch (type) {
            case HERO:
                hero = (Hero *) current->data;
                printf("%s --> ", hero->name);
                break;
            case ITEM:
                item = (Item *) current->data;
                printf("%s --> ", item->name);
                break;
            case MONSTER:
                monster = (Monster *) current->data;
                printf("%s --> ", monster->name);
                break;
            default:
                printf("Print list doesn't recognize %d (seen as int) data type\n", type);
                return;
        }
        current = current->next;
    }
    puts("NULL");
}

//returns 1 if list is empty : 0 if not empty
/*  LEGACY CODE
int isEmpty(int listType){
    switch (listType) {
        case HERO:
            if (topHero == NULL){
                puts("List is empty");
                return 1;
            }
            break;
        case ITEM:
            if (topItem == NULL){
                puts("List is empty");
                return 1;
            }
            break;
        case MONSTER:
            if (topMonster == NULL){
                puts("List is empty");
                return 1;
            }
            break;
        default:
            puts("Error in isEmpty");
            return -1;  //error
            break;
    }
    return 0;
}
 */

//read the key (aka legend) of the database, return it's info (currently just # of entities in database)
//depending on the mode handle the read / write file operations so the calling functions can entirely ignore the database keys
    //remember that if the variable args doesn't work just write it to assume that the output file is "temp.txt"
int readKey(char *mode, FILE *input, ... ){
    int entities = 0, testNum = 0;
    char line[100];
    char *token;
    
    fgets(line, 100, input);
  
    
    if (!(strcmp(mode, "read only")) ) {
        //read mode
        entities = findNum(line);
    }else if (! (strcmp(mode, "read and write")) ){
        //read and write mode
        
        //set up variable args
        va_list ap;
        va_start(ap, input);
        
        FILE *output = va_arg(ap, FILE*);
        fprintf(output, "%s", line); //worked seemingly
        fclose(output);
        
        entities = findNum(line);
        
        va_end(ap);
    }


    return entities;
}

int findNum(char *string){
    char *token;
    //char *dank = malloc(sizeof(char) * 100);
    //char *dank = malloc(sizeof(string)); //this bugs out sometimes, but not always..... signal SIGABRT
    unsigned long stringLength = sizeof(char) * (strlen(string)+1);
    
    
    
    int length = 0, start = 0, result = 0;
    char c;
    
    //get a copy of string, why?
    //strcpy(dank, string);
    //bad access on occasion, why? no null terminator maybe? nope, when dank is malloc'd to sizeof String there are
            //problems but malloc it to sizeof(char) * 100 and no problems
            //sizeof(char[100]) and
            //*string ptr = char[100]
            //sizeof(string) == sizeof(char[100]) no?
    
    //checking numerics manually by finding digits in the string
    for (int i = 0; i < strlen(string); i++) {
        if(isdigit((c = string[i])) ){
            start = i;
            length++;
            //find all the contiguous digits
            while (isdigit((c = string[++i])) ) {
                length++;
            }
            break;      // only one number per line so break
        }
    }
    
    //set a char * to point to the digits
    //char *number = malloc(sizeof(char) * length); //don't need this and can't free this block later anyway
    char *number = &( (string[start]) );
    token = malloc(stringLength);
    
    //copy full string into token
    //memcpy(token, string, length);
    
    //copy the chars that make up the number into token
    memcpy(token, number, length);
    token[length] = '\0';       //set null terminator
    result = atoi(token);       //atoi converts the char string into an int
    
    free(token);   //ATTEMPT AT MEM HEAP UNCORRUPTION

    return result;
}

//semingly works
//take string, parse the numerical part out and replace it with another number
    //TODO: check to see if you're freeing the malloc'd space later
char *extractAndReplace(char *string, int number, int replacement){
    //"ABC" B=int to replace, A=shit before it, C=shit after it, B*=replacement
    char *a, *b, *c, *result, *numberAsString, *replacementStr;
    char *bStart;
    char *blank;
    int digits = 0, aLen = 0, bLen = 0, cLen = 0, difference = 0;
    
    //step1 : find lengths of a b and c
    //step2 : malloc at least enough space for a b and c; if lengths are 0, malloc 10
    //step3 : set a b and c to all \0
    //step3a : set a b and c to proper values?
    //step4 : replace b
    //step5 : glue the strings together
    //step6 : free all the stuff
    
    
    //calculate lengths of a, b, and c      //step1
    unsigned long totalLength = sizeof(char) * (strlen(string));
    
    //find string length of B, first set number to string
    digits = findDigits(number);
    bLen = digits;
    //convert number to string and use to find aLen
    numberAsString = malloc(sizeof(char) * (digits + 1) ); //digits + \0
    sprintf(numberAsString, "%d", number);
    aLen = (int) strcspn(string, numberAsString);
    cLen = (int)totalLength - (bLen + aLen);
    
    
    a = malloc(sizeof(char) * (aLen + 10));
    memset(a, '\0', aLen + 10);
    b = malloc(sizeof(char) * (bLen + 10));
    memset(b, '\0', bLen + 10);
    c = malloc(sizeof(char) * (cLen + 10));
    memset(c, '\0', cLen + 10);
    
    //set the strings
    memcpy(a, string, (sizeof(char) * aLen) );
    memcpy(b, &(string[aLen]), (sizeof(char) * bLen ) );
    memcpy(c, &(string[(aLen + bLen)]), (sizeof(char) * cLen) );
    
    //realloc space for new b
    free(b);
    difference = (findDigits(replacement)) - digits;
    digits = findDigits(replacement);
    b = malloc(sizeof(char) * (digits + 1));
    sprintf(b, "%d", replacement);
    
    //find new total length
    totalLength += difference;
    
    //alloc space for result = to new total length
    result = malloc(sizeof(char) * (totalLength + 1) );     //was just total length
    memset(result, '\0', (sizeof(char) * (totalLength) ));
    
    //glue the strings together in result;
    
    strcpy(result, a);
    strcat(result, b);
    strcat(result, c);
    
    free(a);
    free(b);
    free(c);
    free(numberAsString);
    
    return result;
}

int findDigits(int number){
    int digits = 1;
    while (number /= 10) {
        digits++;
    }
    return digits;
}

