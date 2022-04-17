#include "main_commands.h"

static int commandNum = 1;
static int i = INITIAL;

/* ==================================== Static Functions Declarations ====================================*/
static void switchCommand(char *line, char *command, List *lstByCode, List *lstByPrice, char **shortHistory, CList *history, int* numOfApts);
static void findCommand(char *line, int num, List* lstByCode, List* lstByPrice, char** shortHistory, CList* history, int* numOfApts);

/* ==================================== Functions Implementation ====================================*/

/* Checking commands that start with ! */
void checkReconstraction(char* line, List* lstByCode, List* lstByPrice, char** shortHistory, CList* history, int* numOfApts)
{
    int len, num;
    if(line[INITIAL] == '!')
        checkCommand(shortHistory[i-1], lstByCode, lstByPrice, shortHistory, history, numOfApts);
    else
    {
        num = atoi(line);
        len = checkNumLength(num);
        if( *(line + len) == '\0')
            findCommand(NULL,num, lstByCode, lstByPrice, shortHistory, history, numOfApts);
        else
            findCommand(line+len+1, num, lstByCode, lstByPrice, shortHistory, history, numOfApts);
    }
}

/* Checking how many digits is the parameter num */
int checkNumLength(int num)
{
    int  counter = 1 ;
    while( num >= 10 )
    {
        counter++;
        num= num/10;
    }

    return counter;
}

/* Initializing short history (maagar A) */
void InitializeSHistory(char** shortHistory)
{
    int j;
    for(j = INITIAL; j < N; j++)
        shortHistory[j] = NULL;
}

/* Recording every command in the short history array and history linked list */
void recordHistory(char* line, char** shortHistory, CList* history)
{
    int j, k;
    if(i != N){ /* First filling up the array */
        shortHistory[i] = strdup(line);
        commandNum++;
        i++;
    }
    else{ /* Then when we have seven commands in the array so we add to the list the first one, moving every command one place to the left and adding the new command to the last place */
        AddToCList(history, shortHistory[INITIAL]);
        for (j = INITIAL, k = 1; j < N-1; j++, k++)
            shortHistory[j] = shortHistory[k];
        shortHistory[N-1] = strdup(line);
        commandNum++;
    }
}

/* ==================================== Static Functions Implementation ====================================*/

/* Switching the command's old parameter to the new one */
static void switchCommand(char *line, char *command, List *lstByCode, List *lstByPrice, char **shortHistory, CList *history, int* numOfApts)
{
    if(line == NULL) // nothing to change
    {
        checkCommand(command, lstByCode, lstByPrice, shortHistory, history, numOfApts);
        return;
    }
    char *newCommand, *newParameter, *oldParameter, *temp, *currCommand, *tempLine;
    tempLine = strdup(line);
    oldParameter = strtok(tempLine, "^"); // extracts the parameter that needs to be changed
    newParameter = strtok(NULL, "^"); // extracts the new parameter that needs to be written
    temp = strstr(command, oldParameter); // searches for the first performance of the old parameter in command
    newCommand = (char*)check_malloc(strlen(command) - strlen(oldParameter) + strlen(newParameter));
    if(temp != NULL) { // found the old parameter in the command
        strncpy(newCommand, command, strlen(command) - strlen(temp)); // copies the part before the first performance of the old parameter
        while (temp != NULL) {
            currCommand = strdup(temp + strlen(oldParameter)); // copies the part after the first performance of the old parameter
            newCommand[strlen(command) - strlen(temp)] = '\0';
            temp = strstr(temp + strlen(oldParameter), oldParameter);
            if (newCommand[strlen(command) - strlen(currCommand) - 1] == ' ') // checks if the current performance of the old parameter isn't part of a bigger string
                strcat(newCommand, newParameter); // if not than copies it to the new command
            else
                strcat(newCommand, oldParameter); // if its not the right place to switch than copies the old parameter again
            if (temp != NULL && temp[strlen(oldParameter)] == ' ')
                strncat(newCommand, currCommand, strlen(currCommand) - strlen(temp));
            else if (temp != NULL) {
                temp = strstr(temp + strlen(oldParameter), oldParameter); // looking for another performance
                if (temp != NULL)
                    strncat(newCommand, currCommand, strlen(currCommand) - strlen(temp));
            }
        }
        strcat(newCommand, currCommand); // completes the rest of the line
        checkCommand(newCommand, lstByCode, lstByPrice, shortHistory, history, numOfApts);
    }
    else
        checkCommand(command, lstByCode, lstByPrice, shortHistory, history, numOfApts);
    free(tempLine);
}

/* Searching for the command in the data base (array or list) */
static void findCommand(char *line, int num, List* lstByCode, List* lstByPrice, char** shortHistory, CList* history, int* numOfApts)
{
    CLnode *curr = history->head;
    while(curr != NULL)
    {
        if(curr->commandNum == num)
        {
            switchCommand(line, curr->command, lstByCode, lstByPrice, shortHistory, history, numOfApts);
            break;
        }
        curr = curr->next;
    }
    if(curr == NULL)
        switchCommand(line, shortHistory[N-(commandNum-num)], lstByCode, lstByPrice, shortHistory, history, numOfApts);
}


