#include "main_commands.h"
static int last_cmd = INITIAL ; /* represent the last command that has been received from txt file */

/* ==================================== Static Functions Declarations ====================================*/

static Apt *ReadSingleAptBin(FILE *f);
static Uchar* CollectRoomsAndDate(Apt* apt);
static BYTE ExtractRooms(Sint numOfRooms);
static void ExtractDate(BYTE* res,BYTE* currByte,Apt* apt);
static void CollectMonth(BYTE* currByte,Sint month);
static void CollectYear(BYTE* res, BYTE* currByte,Sint year);
static void CollectDay(BYTE* res, BYTE* currByte,Sint day);
static BOOL isBitISet(BYTE ch, int i);
static void setBit(BYTE* currByte,int i);
static void getRoomsAndDate(FILE* f,Sint* num_of_rooms,Sint* year,Sint*month,Sint* day);
static void WriteCmdArrToFile(FILE* f,char* shortHistory[],int start_ind);
static void WriteCListToFile(FILE* f,CList *lst_history);
static void WriteAptToBinFile(FILE* f,LNode* curr);

/* ==================================== Functions Implementation ====================================*/

/* writing History to txt file in the end of the program - HistoryDoc.txt */
void WriteHistoryToFile(CList* lst_history, char* shortHistory[])
{
    FILE *f = fopen("HistoryDoc.txt","a");
    check_file(f);
    if(lst_history->head == NULL )    /* if database B (Cmd LIST) is empty , write only database A (shortTerm) to file */
    {
        if(last_cmd == INITIAL)
            WriteCmdArrToFile(f,shortHistory,1);
        else
            WriteCmdArrToFile(f,shortHistory,last_cmd+1);
    }
    else                            /* there is more than 7 commands, write the arr and the Clist */
    {
        if(lst_history->tail->commandNum - last_cmd < INITIAL )   /* new cmd only in shortHistory */
            WriteCmdArrToFile(f,shortHistory,last_cmd - lst_history->tail->commandNum + 1 );
        else
        {
            WriteCListToFile(f,lst_history);
            WriteCmdArrToFile(f,shortHistory,1);
        }

    }
    fclose(f);
}

/* Read history from txt file in start of the program - HistoryDoc.txt  */
void getHistoryFromFile(char* short_term_history[] ,CList* history)
{
    int num,offset;
    long fsize;
    char ch = INITIAL;
    char *temp = NULL;
    FILE *f = fopen("HistoryDoc.txt","r");
    if(!f)
    {
        f = fopen("HistoryDoc.txt", "w");
        check_file(f);
        fclose(f);
        return;
    }
    fseek(f,INITIAL,SEEK_END);
    fsize = ftell(f);
    fseek(f,INITIAL,SEEK_SET);
    if( fsize != INITIAL )
    {
        while( ch != EOF )
        {
            fseek(f,-1,SEEK_CUR);
            temp = get_line(f);
            sscanf(temp,"%d",&num);
            offset = checkNumLength(num) + 2;
            recordHistory(temp+offset,short_term_history,history);
            ch = fgetc(f);
        }
        last_cmd = num;
    }
    fclose(f);
}

/* writing apartments database to bin file  - DB_Binfile.bin */
void WriteDatabaseToBinFile(List* lst, int* numOfApts)
{
    LNode *curr ;
    FILE * f = fopen("DB_Binfile.bin","wb");
    check_file(f);

    if(*numOfApts != INITIAL)
        fwrite(numOfApts, sizeof(int),1, f);
    for( curr = lst->head ;  curr != NULL ; curr = curr->next )
        WriteAptToBinFile(f,curr);

    fclose(f);
}

/* reading Bin file and updating lists according to file info */
void ReadBinFile(char* fname, List* lstByCode, List* lstByPrice, int* numOfApts)
{
    int i =  INITIAL;
    long fsize;
    FILE* f = fopen(fname, "rb");
    if(!f)
    {
        f = fopen(fname, "wb");
        check_file(f);
        fclose(f);
        return;
    }
    fseek(f,INITIAL,SEEK_END);
    fsize = ftell(f);
    fseek(f,INITIAL,SEEK_SET);
    if(fsize != INITIAL )
    {
        fread(numOfApts,sizeof(int),1,f);
        while( i < *numOfApts )
        {
            Apt* apt = ReadSingleAptBin(f);
            if(lstByCode->head == NULL)
                AddToEmptyList(lstByCode, NULL, AllocateLNode(apt), NULL);
            else
                AddToEndOfList(lstByCode, AllocateLNode(apt), NULL, NULL);
            AddToListByPrice(lstByPrice, apt);
            i++;
        }
    }
    fclose(f);
}

/* ==================================== Static Functions Implementation ====================================*/

/* collect num of rooms and date - 3 Bytes total */
static BYTE* CollectRoomsAndDate(Apt* apt)
{
    BYTE* res = (BYTE*)check_malloc(sizeof (BYTE)*3);
    BYTE CurrByte = INITIAL;
    Sint numOfRooms = apt->num_of_rooms;
    CurrByte = ExtractRooms(numOfRooms);        /* Extract first 4 bits */
    ExtractDate(res,&CurrByte,apt);

    return res;
}

/* Extract apt's num of rooms ( bits -> actual val ) */
static BYTE ExtractRooms(Sint numOfRooms)
{
    BYTE res = 0x0F & numOfRooms;
    res <<= 4;

    return res;
}

/* Extract apt entry date   */
static void ExtractDate(BYTE* res,BYTE* currByte,Apt* apt)
{
    CollectDay(res,currByte,apt->day);
    CollectMonth(currByte,apt->month);
    CollectYear(res,currByte,apt->year);
}

/* collect apt entry day of date */
static void CollectDay(BYTE* res, BYTE* currByte,Sint day)
{
    int wi = 7, ri = INITIAL ;                /* wi -bit location to write in currByte,  ri - bit location to read from day */
    *currByte |= (day >> 1) ;
    *res = *currByte ;
    *currByte = INITIAL ;
    if( isBitISet(day,ri) == TRUE )
        setBit(currByte,wi);
}

/* collect apt entry month  */
static void CollectMonth(BYTE* currByte,Sint month)
{
    int ri = 3, wi = 6;     /* wi -bit location to write in currByte,  ri - bit location to read from month */
    while(ri >= INITIAL)
    {
        if( isBitISet(month,ri) == TRUE )
            setBit(currByte,wi);
        ri--;
        wi--;
    }
}

/* collect apt entry year of date */
static void CollectYear(BYTE* res,BYTE* currByte,Sint year)
{
    int ri = 6 , wi = 2;        /* wi -bit location to write in currByte,  ri - bit location to read from year */
    BYTE MASK = 0x0F;
    while(wi >= INITIAL )
    {
        if( isBitISet(year,ri) == TRUE )
            setBit(currByte,wi);
        wi--;
        ri--;
    }
    res[1] = *currByte;
    *currByte = INITIAL;
    MASK = year & MASK ;
    *currByte = MASK << 4 ;
    res[2] = *currByte;
}

/* set Bit i */
static void setBit(BYTE* currByte,int i)
{
    BYTE mask = 1 << i;
    *currByte |= mask;
}

/* check if Bit i set */
static BOOL isBitISet(BYTE ch, int i)
{
    BYTE mask = 1 << i;
    if( (mask & ch) == INITIAL)
        return FALSE;
    else
        return TRUE;
}

/* get line from txt file */
char* get_line(FILE* f)
{
    char ch = ' ';
    int l_size = INITIAL, p_size = 2;
    char* line = (char*)check_malloc(sizeof(char)*p_size);
    while( (ch = fgetc(f)) != '\n')
    {
        line[l_size] = ch;
        l_size++;
        if(l_size == p_size)
        {
            p_size *= 2;
            line = (char*)realloc(line,p_size);
            if(!line)
                exit(MEM_ALLOC_ERR);
        }
    }
    line = (char*)realloc(line,l_size+1);
    if(!line)
        exit(MEM_ALLOC_ERR);
    line[l_size] = '\0';

    return line;
}

/* read single apartment from binary file, and returns allocated apartment*/
static Apt *ReadSingleAptBin(FILE *f)
{
    Sint code,len,day,month,year,num_of_rooms ;
    int price;
    char* address;
    time_t EntryToDB ;
    Apt* apt = NULL;

    fread(&code,sizeof(Sint),1,f);
    fread(&len,sizeof(Sint),1,f);
    address =(char*)check_malloc(sizeof(char)*len+1);
    fread(address,sizeof(BYTE),len,f);
    address[len] = '\0';
    fread(&price,sizeof(int),1,f);
    fread(&EntryToDB,sizeof(time_t),1,f);
    getRoomsAndDate(f,&num_of_rooms,&year,&month,&day);
    apt = AllocateApt(code, len, day, month, year, num_of_rooms, price, address, EntryToDB);

    return apt;
}

/* The function gets 3B array and collect apt's num_of_rooms and Entry date (bits -> actual_val) */
static void getRoomsAndDate(FILE* f,Sint* num_of_rooms,Sint* year,Sint*month,Sint* day)
{
    int ri = INITIAL ;
    BYTE* BYTES = (BYTE*)check_malloc(sizeof(BYTE)*3);  /*{B1: nnnn dddd } {B2: dmmm myyy } {B3: yyyy 0000 } */
    BYTE currByte;
    fread(BYTES,sizeof(BYTE),3,f);

    currByte = BYTES[ri++];
    *num_of_rooms = (0xF0 & currByte ) >> 4;

    *day = (0x0F & currByte);
    *day <<= 1;
    currByte = BYTES[ri++] ;
    if( isBitISet(currByte,7) )
        *day|= 0x01;

    *month = ( 0x78 & currByte) >> 3 ;
    *year = (0x07 & currByte) << 4 ;
    currByte = BYTES[ri] >> 4 ;
    *year |= currByte;

    free(BYTES);
}

/* writing commands list (Maagar B) to txt file - HistoryDoc.txt */
static void WriteCListToFile(FILE* f,CList *lst_history)
{
    CLnode *curr = lst_history->head;
    while(curr != NULL)
    {
        if(curr->commandNum > last_cmd )
        {
            fprintf(f,"%d: %s\n",curr->commandNum, curr->command);
            last_cmd++;
        }
        curr = curr->next;
    }
}

/* writing commands array (Maagar A) to txt file - HistoryDoc.txt */
static void WriteCmdArrToFile(FILE* f,char* shortHistory[],int start_ind)
{
    int  i ;
    i = start_ind - 1 ;
    int curr_cmd_ind = last_cmd+1;
    while( shortHistory[i] != NULL && i < N )
    {
        fprintf(f,"%d: %s\n",curr_cmd_ind,shortHistory[i]);
        curr_cmd_ind++;
        i++;
    }
}

/* writing apartment's details to BinFile */
static void WriteAptToBinFile(FILE* f,LNode* curr)
{
    BYTE *RoomsAndDate;
    Sint len = (Sint)strlen(curr->apartment->address);
    fwrite(&curr->apartment->code,sizeof(Sint),1,f);
    fwrite(&len,sizeof(Sint),1,f);
    fwrite(curr->apartment->address,sizeof(char),len,f);
    fwrite(&curr->apartment->price,sizeof(int),1,f);
    fwrite(&curr->apartment->Database_entry_date,sizeof(time_t),1,f);
    RoomsAndDate = CollectRoomsAndDate(curr->apartment);
    fwrite(RoomsAndDate,sizeof(BYTE),3,f);
    free(RoomsAndDate);
}