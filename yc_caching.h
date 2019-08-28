#ifndef YC_CACHING_H
#define YC_CACHING_H

#include <stdlib.h> 
#include <stdio.h>

#include "yc_dynamic.h"
#include "yc_parameter.h"


/*STRUCTURE DEFINITION:REQUEST*/
typedef struct req {
	double arrivalTime;		//抵達時間
	unsigned devno;		//裝置編號(預設為0)
	unsigned long diskBlkno;	//Block編號(根據Disksim格式)
	unsigned reqSize;		//Block連續數量(至少為1)
	unsigned reqFlag;		//讀:1;寫:0
	unsigned userno;		//使用者編號(1~n)
	double responseTime;		//反應時間(初始為0)
} REQ;

typedef struct node {
	//unsigned long diskBlkno;
	unsigned long page_no;
	int flag;				//1 :  clean,  0 : dirty,  2 :  LAZY + clean
	int LAZY;

	struct node *next;
	struct node *pre;

	struct node *T1_next; 		//(PC_HDD)LRU
	struct node *T1_pre;   		//(PC_HDD)MRU
	struct node *T2_next; 		//(PC_SSD)LRU
	struct node *T2_pre;   		//(PC_SSD)MRU
	struct node *ghost_next; 	//LRU
	struct node *ghost_pre;   	//MRU
	struct node *SSD_next; 	//LRU
	struct node *SSD_pre;   		//MRU
} NODE;

static NODE *TNQ_head = NULL;
static int TNQ_Size = 0;

static NODE *T1head = NULL, *T1last = NULL, *T2head = NULL, *T2last = NULL;
static NODE *Windowhead = NULL, *Windowlast = NULL;
static NODE *Ghead = NULL, *Glast = NULL;
static NODE *SSDhead = NULL, *SSDlast = NULL;

extern int T1Size,T2Size;
extern int T1Used, T2Used;
extern int windowSize;
static int win_count = 0;

static int GHOSTbufferUsed = 0;
static int SSDUsed = 0;

static int GHOSTbufferSize = SSDSize*0.1;
static int GHOSTsize1, GHOSTsize2;

void PUSH_Toatl_Node_QUE(unsigned long page_no);
//for deal with the coming request 
int CheckPageCache(unsigned long page_no, int flag);

//for search 
NODE *searchTNQ(unsigned long page_no);
NODE *searchT1queue(unsigned long page_no, int flag);
NODE *searchT2queue(unsigned long page_no, int flag);
NODE *searchSSD(unsigned long page_no);
NODE *searchGHOSTbuffer(unsigned long page_no);

//for checking the page is in which area
int CheckSSDTable(unsigned long page_no, int flag);
void Adjust_Glast(int New_GBSize);
int checkGHOSTbuffer(unsigned long page_no, int flag);

//metain the queues
int insertT1(unsigned long page_no, int flag);
int insertT2(unsigned long page_no, int flag);
int insertGHOSTbuffer(unsigned long page_no, int flag);
int insertSSD(unsigned long page_no, int flag);

NODE *FindingT1victim();
NODE *FindingT2victim();
NODE *FindingSSDvictim();
void *Evict_T1victim(NODE *eviction);
void *Evict_T2victim(NODE *eviction);
void  *Evict_SSDvictim(NODE *eviction);

void KeepT2CleanArray();

void displayT1Table();
void displayT2Table();
void displayGHOSTbuffer();
void displaySSDTable();

REQ *PageReqMaker(unsigned long page_no, int flag);

typedef struct GHOST {
	
	unsigned long page_no;
	int flag;

	struct  GHOST *G1_next; //(PC_HDD)LRU
	struct GHOST *G1_pre;   //(PC_HDD)MRU
	struct  GHOST *G2_next; //(PC_SSD)LRU
	struct GHOST *G2_pre;   //(PC_SSD)MRU

}GHOST;

static GHOST *G1head = NULL, *G2head = NULL;
static GHOST *G1last = NULL, *G2last = NULL;

extern int G1Used, G2Used;

GHOST *searchG1queue(unsigned long page_no, int flag);
GHOST *searchG1queue(unsigned long page_no, int flag);
void insertG1(unsigned long page_no, int flag);
void insertG2(unsigned long page_no, int flag);
void  *Evict_G1last();
void  *Evict_G2last();
void displayG1Table();
void displayG2Table();
void displayTNQ();

int get_NODE_counts();

/*
typedef struct total_node_queue {
	struct node n;
	struct diff_node_queue *pre;
	struct diff_node_queue *next;
} TN_QUE;

static TN_QUE *TNQ_head = NULL;
static int TNQ_Size = 0;
void PUSH_Toatl_Node_QUE(unsigned long page_no);
TN_QUE *searchTNQ(unsigned long page_no);
void displayTNQ();
*/

#endif