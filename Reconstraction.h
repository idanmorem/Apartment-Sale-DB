#ifndef PROJECT_RECONSTRACTION_H
#define PROJECT_RECONSTRACTION_H
#include "DataBase.h"
/* ====== Functions Declarations ======*/
void checkReconstraction(char* line, List* lstByCode, List* lstByPrice, char** shortHistory, CList* history, int* numOfApts);
void InitializeSHistory(char** shortHistory);
void recordHistory(char* line, char** shortHistory, CList* history);
int checkNumLength(int num);
#endif