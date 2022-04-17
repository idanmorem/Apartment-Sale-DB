#ifndef PROJECT_DATABASE_H
#define PROJECT_DATABASE_H
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

/* Define and MACROS */
#define Uchar unsigned char
#define Sint short int
#define N 7
#define MEM_ALLOC_ERR (-1)
#define FILE_ERROR (-2)
#define NUM_OF_DELIM 3
#define NUM_OF_FIND_COMMANDS 8
#define INCREASE 1
#define DECREASE (-1)
#define MAX_INIT 2147483647 // 2^31;
#define INITIAL 0
#define TRUE 1
#define MAXROOMS 15
#define FALSE 0
#define BOOL int
#define BYTE unsigned char
#define PRICELIST 3
#define CODELIST 2

/* Data structure implementation */

typedef struct FindKeys{
    short int year,month,day;
    short int lastXdays_enter;
    short int minR;
    short int maxR;
    short int order;
    int maxP;
    int minP;
}Find_keys;

typedef struct Apartment{        /* apartment details */
    int code ;
    char* address;
    int price ;
    short int num_of_rooms;

    short int day, month,year;
    time_t Database_entry_date;
}Apt;

typedef struct LIST{             /* singly linked list struct (using also for sorted list) */
    struct ListNode* head;
    struct ListNode* tail;
}List;

typedef struct ListNode {        /* doubly linked list node struct */
    struct ListNode* prev;
    struct ListNode* next;
    Apt* apartment;
}LNode;

typedef struct Commands_List {   /* singly linked list struct (commands) */
    struct CListNode* head;
    struct CListNode* tail;
}CList;

typedef struct CListNode{      /* commands singly linked list nodes  */
    struct CListNode* next;
    char* command;
    int commandNum;
}CLnode;

/* Functions Implementation */
/* ====== Functions Declarations ======*/
Apt *AllocateApt (Sint code, Sint len, Sint day, Sint month, Sint year, Sint num_of_rooms, int price, char* address, time_t EntryToDB);
Apt* getAptParameters(char* line, int currCode);
LNode *AllocateLNode(Apt *apt);
void AddToEmptyList (List *list,CList *Clist, LNode *node, CLnode *Cnode);
void AddToEndOfList(List *list, LNode *node, CList *Clist, CLnode *Cnode);
void AddToListByPrice (List *list, Apt* apt);
void DeleteFromList (List *list, List *listByPrice, int code);
void PrintApt(Apt *apt);
void AddToCList(CList *list, char* command);
void PrintCList(CList *list);
void PrintShortHArray(char **array, CList *list);
void ClearDB(CList *history, char *short_term_history[N], List *lst, List *lstPrice);
void* check_malloc (int num_of_bytes);
void check_file (FILE *file);
void InitializeDataBase(List *lst,List* lstPrice,char** short_term_history,CList* history);
#endif //PROJECT_DATABASE_H