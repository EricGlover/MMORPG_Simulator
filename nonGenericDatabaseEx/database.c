//
//  database.c
//  MMORPG_Simulator
//
//  Created by Eric Glover on 1/23/17.
//  Copyright © 2017 Glover Corp. All rights reserved.
//

#include "database.h"

#define TRUE    1
#define FALSE   0

#define HERO    0
#define ITEM    1
#define MONSTER 2


Hero *createHero(void);


//node functions
void insert(Hero *hero);
int delete(Hero **heroPtr);
int isEmpty(int listType);      //pass it HERO ITEM OR MONSTER
void printHeroList(void);

void insertItem(Item *item);
int deleteItem(Item **item);

/////////

///internal database functions and things
void printItemList(void);

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
char *mDbName = "monsterDatabase.txt";
char *iDbName = "itemDatabase.txt";
//entity Lists
HeroNode *topHero = NULL;
ItemNode *topItem = NULL;
MonsterNode *topMonster = NULL;


//necessary database format for this to work : one line per attribute, the only numbers in the database are the values of the hero, there must be a \t or whitespace separating the numbers from the words, a blank line between entries, for attributes that are strings have only the attribute value in that line, for the moment no junk text inbetween entries is allowed (do to updateHeroDb() and probably createHero() ), also no junk text at the top of the database...
//createHero reads the values of the first unloaded hero from the database into a newly made hero struc and returns a pointer to it


//not actually in the database, just using this for demonstration purposes.
int main(void) {
  /*
    Hero *rogue = createHero();       //TESTING , REMEMBER TO CHANGE DATABASE ENTRY MANAULLY TO LOADED = 1 ON ROGUE AND KNIGHT
    Hero *knight = createHero();
    Hero *gnome = createHero();
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

    return 0;
}



Hero * createHero(){
    int loaded = 0, lineNum = 1, data = 0, createdHero = 0;  //
    char line[100];
    char *foundEOF;
    int stat = 0, desire = 0;   //iterators
    //memcpy(line, 'z', 100);
    memset(line, 'z', 100);     //init line cause why not

    //open hero db
    if ( (heroDb = fopen(hDbName, "r") ) == NULL ) {            //if file open fails
        puts("Error in loading heroDb...exiting and returning a blank hero");
        Hero *hero = malloc(sizeof(Hero));
        return hero;
    }
    /*if(DATABASE_KEY){
        readKey("read only", heroDb);
     }*/    //TEST THIS LATER

    Hero *hero = malloc(sizeof(Hero));
    /*      TESTING */
    doEntry:
    do {
        if ( (foundEOF = fgets(line, 100, heroDb)) == NULL) {       //sets foundEOF to null for later if at the end of the file
            break;      //break loop on EOF
        }
        //syntax struct of if else's
        if (lineNum == 1) {
            //loaded
            loaded = findNum(line);
        }else if(lineNum == 2){
            //name
            line[strlen(line) - 1] = '\0';  //clip the null terminator off
            strcpy(hero->name, line);
        }else if (lineNum == 3) {
            //class
            line[strlen(line) - 1] = '\0';  //clip the null terminator off
            strcpy(hero->h_class, line);
        }else if (lineNum <= STAT_NUM + 3){
            //this should be the int array stats part of the file
            hero->stats[stat] = findNum(line);
            stat++;
        }else if (lineNum <= STAT_NUM + 3 + DESIRES_NUM){
            hero->desires[desire] = findNum(line);
            desire++;
        }else if (lineNum == LINES_PER_HERO_ENTRY){
            //last line is a junk text separator, just skip it
            createdHero = 1;
            desire = stat = 0;  //not necessary but feels good
            break;
        }
        lineNum++;


    } while (loaded == 0);
/*    */
    //broke out of the loop either because found EOF or loaded == 1
    if ( foundEOF == NULL) {      //could probably just use a char * to catch the return of this earlier but this works too
        if(!createdHero){
            //print error?
            puts("Couldn't find an unloaded hero in db, createHero() failure");
            fclose(heroDb);
            return hero;
        }
    }else if (loaded == 1){
        //skip entry and goto doEntry       //skip line amount - 1
        for (int j = 0; j < LINES_PER_HERO_ENTRY - 1; j++) {
            if ( (fgets(line, 100, heroDb)) == NULL) {
                puts("Couldn't find an unloaded hero in db, createHero() failure");
                fclose(heroDb);
                return hero;    //if we run out of file without finding a hero print error and break
            }
        }
        desire = stat = 0;
        lineNum = 1;
        goto doEntry;     //testing
    }

    insert(hero);
    hero->loaded = 1;     testing

    //close all file pointers before calling this
    fclose(heroDb);
    updateHeroDb();

    return hero;
}

//run through the heroList and use it to rewrite the database
//later try and rewrite the code to allow for code reuse when you do the updateMonsterDb() and updateItemDb()
//potential error but I'm ignoring it - let it be known that the program may overwrite the database
//for the moment no junk text inbetween entries is allowed
void updateHeroDb(void) {
    FILE *heroDb = NULL;
    FILE *temp = NULL;
    int loaded = 0, lineNum = 0, data = 0, i = 0, heroesInDatabase = 0;
    char line[100];
    //char newLine[100];
    char *newLine;
    memset(line, '\0', 100);
    char *token;
    int stat = 0, desire = 0;   //iterators
    Hero *hero = NULL;

    //open the database
    if ( (heroDb = fopen(hDbName, "r") ) == NULL ) {            //if file open fails
        puts("Error in loading heroDb...exiting, sorry couldn't update it");
    }
    //open tmp file to write into
    if ( (temp = fopen("temp.txt", "w") ) == NULL) {
        puts("Error in opening the temp file...not handling the error");
    }

    if(DATABASE_KEY){
        heroesInDatabase = readKey("read and write", heroDb, temp);
    }

    //how's this working? for each hero read through the database and write the updated entry into tmp.txt (with \n 's)
    //then write the rest of the database into tmp.txt as it is
    //then close out the database file pointer, open it again with write privledges (that should create a fresh page)
    //read tmp.txt into the newly made blank database, then close tmp.txt and database

    //while loop for reading file
        //if ! null hero
        //else
    /*      TESTING    */
    lineNum = 1;
    stat = 0;
    desire = 0;
    while ( (fgets(line, 100, heroDb)) != NULL) {
        if ( (hero = getHeroI(i)) != NULL) {
            //readEntry()
            if (lineNum == 1) {
                //loaded
                data = findNum(line);
                newLine = extractAndReplace(line, data, hero->loaded);
                fprintf(temp, "%s", newLine);
                free(newLine);
            }else if (lineNum == 2){
                //name
                fprintf(temp, "%s", hero->name);
                fprintf(temp, "\n");
            }else if (lineNum == 3){
                //class
                fprintf(temp, "%s", hero->h_class);
                fprintf(temp, "\n");
            }else if (lineNum <= 3 + STAT_NUM){
                //stats[]
                data = findNum(line);
                newLine = extractAndReplace(line, data, hero->stats[stat]);
                fprintf(temp, "%s", newLine);
                free(newLine);
                stat++;
            }else if (lineNum <= LINES_PER_HERO_ENTRY - 1){
                //desires[]
                data = findNum(line);
                newLine = extractAndReplace(line, data, hero->desires[desire]);
                fprintf(temp, "%s", newLine);
                free(newLine);
                desire++;
            }else{
                //blank line
                fprintf(temp, "%s", line);
                lineNum = 0;    //will ++ soon
                stat = 0;
                desire = 0;
                i++;
            }
        }else{
            //change loads to 0
            if (lineNum == 1) {
                //loaded
                data = findNum(line);
                newLine = extractAndReplace(line, data, 0);
                fprintf(temp, "%s", newLine);
                free(newLine);
            }else{
                //copy and paste line
                fprintf(temp, "%s", line);
            }
            if(lineNum == LINES_PER_HERO_ENTRY){
                //reset vars
                lineNum = 0;    //will ++ soon
                stat = 0;
                desire = 0;
            }
        }
        lineNum++;
    }

/*    */

    //now the database file should be written into temp, so close both files and copy temp.txt into the new database file
    fclose(heroDb);
    fclose(temp);
    heroDb = fopen(hDbName, "w");
    temp = fopen("temp.txt", "r");
    printf("PRINTING WHAT UPDATE_HERO_DB IS WRITING TO THE DATABASE...\n");
    while ( (fgets(line, 100, temp)) != NULL ){
        fprintf(heroDb, "%s", line);
        printf("%s", line);
    }
    //now close temp and delete it
    fclose(temp);
    remove("temp.txt");

    //close the database file
    fclose(heroDb);
}

void updateHeroes(void){
    //how am I managing the different entityLists??
        //makes more sense to be in main, I think...
            //entityLists currently reside in database.c
        //maybe if I use some getter and setter functions defined in main to access the lists here?
        //still, even at length ten the heroList is 80 bytes so if every time I call one of these functions it has to declare space for it and then populate it using getters....ughhhhhhh....just seems so sloppy
            //could use a pointer linked list , indeed this will do, use a getter defined in main.h to retrieve the starting ptr
                //probably declare some basic functions that'll be used a lot for using the linked list in database.h
                //like getI() or something, so getters, setters, add, drop, search

    char line[100];
    char *token = NULL;
    int lineNum = 0, loaded = 0, loadedHeroes = 0, heroesInDatabase = 0, heroI = 0, desire = 0, stat = 0;
    Hero *hero = NULL;

    if( (heroDb = fopen(hDbName, "r")) == NULL){
        puts("error, problem loading the heroDB");
    }
    if( DATABASE_KEY ){
        heroesInDatabase = readKey("read only", heroDb);
    }

        //loop 1 : loop through the file
    while ( (fgets(line, 100, heroDb)) != NULL) {
        //load next hero from list that has loaded == 1
        //hero = getHeroI(heroI);
        if ( (hero = getHeroI(heroI++)) == NULL) {
            //puts("error)
            //allowing this to then create a new Hero from here is a way to allow us to dump the database heroes into the game all at once
                //undecided if this is really useful, for now I'm setting it to a flag
            if (!DATA_DUMP) {
                //we're out of heroes that we're updating break out of loop 1
                break;
            }
        }else{
            loadedHeroes++;     //i think...
        }

        readEntry:
        //reset things
        loaded = desire = stat = lineNum = 0;   //added lineNum = 0;
        line[strlen(line) - 1] = '\0'; //write over the newline character with a null terminator, and write it in later so it doesn't fuck up
        if(lineNum == 0) {
            //LOADED
            //if entry has loaded /= 1 then skip the entry and start at readEntry where we look at loaded again
            loaded = findNum(line);
            if (loaded == 0){
                //execute skip code, if dataDump flag is false
                if (DATA_DUMP) {
                    //create a new hero....
                }else{
                    for (size_t i = 0; i < LINES_PER_HERO_ENTRY + 1; i++) {
                        fgets(line, 100, heroDb);
                    }
                    goto readEntry;     //check this
                }

            }else if(loaded == 1){
                //happy times, do nothing ?
            }
            lineNum++;
        }

        //
        /*      TESTING */
        //loop 2: loop through a single entry sans first line (loaded)
        while ( (fgets(line, 100, heroDb)) ) {
            line[strlen(line) - 1] = '\0'; //write over the newline character with a null terminator, and write it in later so it doesn't fuck up

            //syntax of entries
            if (lineNum == 1) {
                //NAME
                //hero->name
                strcpy(hero->name, line);
            }else if (lineNum == 2) {
                //H_CLASS
                strcpy(hero->h_class, line);
            }else if (lineNum <= STAT_NUM + 2){
                //STATS
                hero->stats[stat] = findNum(line);
                stat++;
            }else if (lineNum <= STAT_NUM + 2 + DESIRES_NUM){
                //DESIRES
                hero->desires[desire] = findNum(line);
                desire++;
                //find last line of entry
                if (lineNum == (LINES_PER_HERO_ENTRY - 1)){ //is never getting called maybe b/c -1
                    //skip line needed?
                    break;
                }
            }else if (lineNum == (LINES_PER_HERO_ENTRY - 1) ){
                break;
            }
            lineNum++;
        }
        /* */
    }


    //close file pointers, reopen with appropraite privledges
    fclose(heroDb);
}

//add a heroNode to the heroLinked List
void insert(Hero *hero){
    //create a new node
    HeroNode* new = malloc(sizeof(HeroNode));

    if(new != NULL) {
        new->node = hero;
        new->next = NULL;

        if (isEmpty(HERO)) {
            topHero = new;
        }else{
            HeroNode *current = topHero;
            //HeroNode *next = NULL;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new;
        }
    }else{
        puts("Wasn't enough space to create a new Hero");
    }
}
//return 1 if success, 0 if fail
//free's the node, the hero in the node, and resets the linked list
int delete(Hero **heroPtr){
    if (isEmpty(HERO) ) {
        puts("Error, the list is already empty");
        return 0;
    }else if (*heroPtr == NULL){
        puts("Can't delete a null hero from the hero list");
        return 0;
    }
    Hero *hero = *heroPtr;
    HeroNode *current = topHero;
    HeroNode *previous = NULL;
    while (current->node != hero) {
        previous = current;
        current = current->next;
        if (current == NULL) {
            puts("Could not delete hero from list because hero isn't in the list");
            return 0;
        }
    }
    if(previous == NULL){
        topHero = current->next;    //either null or actual heroNode
    }else{
        previous->next = current->next;
    }
    //free up the space and make sure nothing is pointing to there any more...
    //otherwise you can corrupt your heap...which is the sad times.
    free(current->node);
    free(current);        //ATTEMPT AT MEM HEAP UNCORRUPTION
    hero = NULL;


    return 1;
}
//uses the printHero on every hero in the list and prints a nice little arrow diagram with there names
void printHeroList(void){
    if (isEmpty(HERO)){
        puts("Hero list is currently empty");
    }else{
        HeroNode *current = topHero;
        while (current != NULL) {
            printHero(current->node);
            current = current->next;
        }
        current = topHero;
        while (current != NULL) {
            //printf("%s --> ", current->node->name);   //TESTING
            current = current->next;
        }
        puts("NULL");
    }
}
//returns 1 if list is empty : 0 if not empty
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

    //free(dank);      //ATTEMPT AT MEM HEAP UNCORRUPTION
    free(token);   //ATTEMPT AT MEM HEAP UNCORRUPTION
    //free(number);

    return result;
}
//semingly works
//take string, parse the numerical part out and replace it with another number
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
