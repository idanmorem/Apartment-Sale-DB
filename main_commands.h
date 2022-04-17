#ifndef PROJECT_MAIN_COMMANDS_H
#define PROJECT_MAIN_COMMANDS_H
#include "FilesOp.h"

/* ====== Functions Declarations ======*/
void checkCommand(char* line, List* lstByCode, List* lstByPrice, char** shortHistory, CList* history, int* numOfApts);
void find(List* price_lst, char* line);
void addApt(char* line, List* lstByCode, List* lstByPrice, int* numOfApts);
void buyApt(char* line, List* lstByCode, List* lstByPrice, int* numOfApts);
void deleteApt(char *line ,List *lstByPrice, List *lstByCode, int* numOfApts);
void exitProg(CList *history,char** shortHistory,List* lstByCode,List* lstByPrice, int* numOfApts);
#endif