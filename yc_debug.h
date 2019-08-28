#ifndef YC_DEBUG_H
#define YC_DEBUG_H

#include <stdlib.h>
#include <stdio.h>

#include "yc_caching.h"
#include "yc_parameter.h"

/*印出字串*/
void PrintSomething(char* str);
/*印出字串與錯誤碼並結束程式*/
void PrintError(int rc, char* str);
/*印出字串與錯誤碼*/
void PrintDebug(int rc, char* str);
/*印出Request資訊*/
void PrintREQ(REQ *r, char* str);
/*印出NODE資訊*/
void PrintNODE(NODE *n, char* str);
/*印出進度*/
void printProgress(unsigned long currentREQ, unsigned long totalREQ, unsigned long currentMeta, unsigned long currentCache);

void PrintINFO(pcSTAT in, char* str);

void PrintMissRatio_in_BLOCKS(BLOCK_miss_record mrib, char* str);


#endif
