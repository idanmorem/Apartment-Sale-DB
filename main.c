#include "main_commands.h"

int main()
{
    List lst,lstPrice;
    CList history;
    int numOfApts = 0; /* tracks the current amount of apartments */
    char *short_term_history[N], *temp = NULL;
    char ans;

    InitializeDataBase(&lst,&lstPrice,short_term_history,&history); /* Initialize required Lists and array */
    ReadBinFile("DB_Binfile.bin",&lst, &lstPrice, &numOfApts); /* Read bin file and set info from it to DB */
    getHistoryFromFile(short_term_history,&history); /* Read txt file and set project History from it */

    printf("Please enter one of the following commands:\nadd-apt, find-apt, buy-apt, delete-apt or exit\n");
    printf("For reconstruction commands, please enter:\n!!, !num, history, short_history or !num^str1^str2\n");
    while(1) {
        temp = get_line(stdin);
        printf(">> %s\n",temp);
        checkCommand(temp, &lst, &lstPrice, short_term_history, &history, &numOfApts);
        if(strcmp(temp, "exit") == 0)
            break;
    }

    printf("Press any key to continue...\n");
    scanf(" %c",&ans);
    return 0;
}