#include "main_commands.h"

/* ==================================== Static Functions Declarations ====================================*/

static void collectKeysSearch(char* line,Find_keys *keys);
static int collectDaysRange(char* line);
static void findAndPrint(List* price_lst, Find_keys* keys);
static BOOL checkAptMatch(Apt* apt,Find_keys* keys);
static BOOL isEarlier(short int year,short int month ,short int day,Find_keys* keys);
static BOOL InTimeRange(Apt* apt,int days_ago);
static void collectDate(const char* date_str,Find_keys* keys);

/* ==================================== Functions Implementation ====================================*/

/* comparing the first letter of the line to decide which command to active */
void checkCommand(char* line, List* lstByCode, List* lstByPrice, char** shortHistory, CList* history, int* numOfApts)
{
    if(line[INITIAL] == 'a') {
        addApt(line + 9, lstByCode, lstByPrice, numOfApts);
        recordHistory(line, shortHistory, history);
    }
    else if(line[INITIAL] == 'f') {
        find(lstByPrice,line); // need to check which list to send
        recordHistory(line, shortHistory, history);
    }
    else if(line[INITIAL] == 'b') {
        buyApt(line, lstByCode, lstByPrice, numOfApts);
        recordHistory(line, shortHistory, history);
    }
    else if(line[INITIAL] == 'd') {
        deleteApt(line, lstByPrice, lstByCode, numOfApts);
        recordHistory(line, shortHistory, history);
    }
    else if(line[INITIAL] == 'e')
        exitProg(history,shortHistory,lstByCode,lstByPrice, numOfApts);
    else if(line[INITIAL] == 'h')
    {
        PrintCList(history);
        PrintShortHArray(shortHistory, history);
    }
    else if(line[INITIAL] == 's')
        PrintShortHArray(shortHistory, history);
    else
        checkReconstraction(line+1, lstByCode, lstByPrice,shortHistory, history, numOfApts); // the first char was !
}

/* Main command - Find - finds apt according to searching values*/
void find(List* price_lst, char* line)
{
    Find_keys keys;    /* struct to represent the required info to find the required apartments */
    collectKeysSearch(line, &keys);  /* collect search details from command */
    findAndPrint(price_lst,&keys);
}

/* Main command - Delete - delete apt according to given values*/
void deleteApt(char *line ,List *lstByPrice, List *lstByCode, int* numOfApts)
{
    int flag =FALSE;
    int days_ago = collectDaysRange(line);
    LNode *curr = lstByCode->tail;

    if(!curr)
    {
        puts("Database is empty ,no values for deletion\n");
        return;
    }
    while(curr != NULL)
    {
        if(InTimeRange(curr->apartment,days_ago))
        {
            DeleteFromList(lstByCode, lstByPrice, curr->apartment->code);
            curr = lstByCode->tail;
            flag = TRUE;
            (*numOfApts)--; // updates the current amount of apartments
        }
        else
            curr = curr->prev;
    }
    if(flag == FALSE)
        puts("not found\n");
}

/* Main command - Add - creates new apartment and adding it to both lists */
void addApt(char* line, List* lstByCode, List* lstByPrice, int* numOfApts)
{
    int currCode = INITIAL;
    if(lstByCode->tail != NULL)
        currCode = lstByCode->tail->apartment->code; // gets the latest apartment code from the list
    Apt *apt1 = getAptParameters(line, currCode); // allocating new apartment
    if(lstByCode->head == NULL) // decides where to add the new apartment
        AddToEmptyList(lstByCode, NULL, AllocateLNode(apt1), NULL);
    else
        AddToEndOfList(lstByCode, AllocateLNode(apt1), NULL, NULL);
    AddToListByPrice(lstByPrice, apt1); // adds also to the list sorted by price
    (*numOfApts)++; // updates the current amount of apartments
}

/* Main command - Buy - buying apt according to given values*/
void buyApt(char* line, List* lstByCode, List* lstByPrice, int* numOfApts)
{
    int code = atoi(line+8); // gets the apartment code from the line
    DeleteFromList(lstByCode, lstByPrice, code);
    (*numOfApts)--; // updates the current amount of apartments
}

/* Main command - Exit - update both database files,then exit the program */
void exitProg(CList *history,char** shortHistory,List* lstByCode,List* lstByPrice, int* numOfApts)
{
    WriteHistoryToFile(history,shortHistory); /* writing history to txt file */
    WriteDatabaseToBinFile(lstByCode, numOfApts); /* writing apartments DB to Bin file */
    ClearDB(history, shortHistory,lstByCode,lstByPrice);
    printf("Good Bye!\n");
}

/* ==================================== Static Functions Implementation ====================================*/

/* returns TRUE if Enter to apartment is earlier than received date */
static BOOL isEarlier(Sint apt_year,Sint apt_month ,Sint apt_day, Find_keys* keys)
{
    if(apt_year < keys->year)                /* in case of earlier year */
        return TRUE;
    else if( apt_year > keys->year)          /*  in case of later year */
        return FALSE;
    else                                     /* in case of equal year */
    {
        if(apt_month < keys->month )         /* in case of equal year and earlier month  */
            return TRUE;
        else if (apt_month > keys->month )   /* in case of equal year and bigger month  */
            return FALSE;
        else                                 /* in case of equal year and month */
        {
            if(apt_day <= keys->day)         /* in case of earlier day and equal year and month */
                return TRUE;
            else
                return FALSE;
        }
    }
}

/* returns TRUE if apartment is match to "FIND" keys search */
static BOOL checkAptMatch(Apt* apt, Find_keys* keys)
{
    if( (apt->price >= keys->minP) && (apt->price <= keys->maxP) )
    {                    /* check price */
        if( (apt->num_of_rooms >= keys->minR) && (apt->num_of_rooms <= keys->maxR) ) /* check room number */
        {
            if(keys->year != INITIAL)
            {
                if (isEarlier(apt->year,apt->month,apt->day,keys) == TRUE ) /* check Enter to Apt date */
                    if(InTimeRange(apt,keys->lastXdays_enter) == TRUE) /* check Database Entry date */
                        return TRUE;
            }
            else
            {
                if ( InTimeRange(apt,keys->lastXdays_enter) == TRUE) /* check Database Entry date*/
                    return TRUE;
            }
        }
    }
    return FALSE ;
}

/* returns TRUE if apartment's EntryToDB date is in the given range */
static BOOL InTimeRange(Apt* apt,int days_ago)
{
    if(days_ago == INITIAL )
        return TRUE;
    time_t current;
    int seconds_in_day,Total_days;
    double seconds_diff;

    time(&current);
    seconds_in_day = 3600*24;
    Total_days= days_ago*seconds_in_day;

    seconds_diff = difftime(current,apt->Database_entry_date);
    if (seconds_diff <= Total_days)
        return TRUE;
    else
        return FALSE;
}

/* functions collect numeric date from string of date */
static void collectDate(const char* date_str,Find_keys* keys)
{
    char temp[N];
    int i = INITIAL ;
    temp[INITIAL] = date_str[i++];
    temp[1] = date_str[i++];
    keys->day = atoi(temp) ;
    temp[INITIAL] = date_str[i++];
    temp[1] = date_str[i++];
    keys->month = atoi(temp);
    i += 2;
    temp[INITIAL] = date_str[i++];
    temp[1] = date_str[i];
    keys->year = atoi(temp);
}

/* gets string and collect from it searching values (keys)*/
static void collectKeysSearch(char* line,Find_keys *keys)
{
    line = line + 9;
    char* temp = strdup(line);
    char delim[NUM_OF_DELIM] = {' ','-'};
    char* findByCommands[NUM_OF_FIND_COMMANDS] = {"MinNumRooms","MaxNumRooms","MaxPrice","MinPrice","Date","Enter","s","sr"};
    char* token = NULL;

    keys->lastXdays_enter = keys -> minP = keys->minR  = keys->order = INITIAL;
    keys->year = keys->month = keys->day = INITIAL;
    keys->maxR = MAXROOMS ;
    keys->maxP = MAX_INIT ;

    token = strtok(temp,delim);
    while(token)
    {
        if(strcmp(token,findByCommands[INITIAL]) == INITIAL){            /* find by MinNumRooms */
            token = strtok(NULL,delim);
            keys->minR  = atoi(token);
        }
        else if(strcmp(token,findByCommands[1]) == INITIAL){        /* find by MaxNumRooms */
            token = strtok(NULL,delim);
            keys->maxR = atoi(token);
        }
        else if(strcmp(token,findByCommands[2]) == INITIAL){        /* find by MaxPrice */
            token = strtok(NULL,delim);
            keys->maxP = atoi(token);
        }
        else if(strcmp(token,findByCommands[3]) == INITIAL){        /* find by MinPrice */
            token = strtok(NULL,delim);
            keys->minP = atoi(token);
        }
        else if(strcmp(token,findByCommands[4]) == INITIAL){        /* find by Entry date in to apartment  */
            token = strtok(NULL,delim);
            collectDate(token,keys);
        }
        else if(strcmp(token,findByCommands[5]) == INITIAL){        /* find by database entry date */
            token = strtok(NULL,delim);
            keys->lastXdays_enter = atoi(token);
        }
        else if(strcmp(token,findByCommands[6]) == INITIAL)      /* find all apartments - low to high*/
            keys->order = INCREASE ;
        else if(strcmp(token,findByCommands[N]) == INITIAL)      /* find all apartments - high to low*/
            keys->order = DECREASE ;
        token = strtok(NULL,delim);
    }
    free(temp);
}

/* searching and print apartments by the searching key values */
static void findAndPrint(List* price_lst, Find_keys* keys)
{
    LNode* curr;
    if( keys->order == DECREASE )
    {
        for ( curr = price_lst->tail ; curr != NULL ; curr = curr->prev )
            if(checkAptMatch(curr->apartment, keys) == TRUE)
                PrintApt(curr->apartment);
    }
    else
    {
        for( curr = price_lst->head ; curr != NULL ; curr = curr->next )
            if ( checkAptMatch(curr->apartment,keys) == TRUE )
                PrintApt(curr->apartment);
    }
}

/* collecting days range, string to numeric value*/
static int collectDaysRange(char* line)
{
    return atoi(line+18);
}