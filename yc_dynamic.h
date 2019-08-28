#ifndef YC_DYMAMIC_H
#define YC_DYMAMIC_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "yc_caching.h"
#include "yc_parameter.h"

typedef struct PCSTATUS{
	int total_pages;

	int period;

	int PageCache_Hit;

	int T1_Read_Hit_Count;			//case1
	int T1_Write_Hit_Count;			//case5
	int T2_Read_Hit_Count;			//case2
	int T2_Write_Hit_Count;			//case6
	//for check
	int PageCache_Miss;
	//PageCache_Miss = total_pages - PageCache_Hit;

	//for check
	int PCReadMiss_to_SSDRead;		//case3+case4

	int PCReadMiss_to_SSDRead_Hit; 	//case3
	int T2victim_c3;
	int T2victim_WSSD_c3;//write SSD					
	//T2victim_WSSD_c3 / T2victim_c3 = ratio of dirty victim in T2
	int SSDvictim_c3;
	int SSDvictim_WHDD_c3;
	//SSDvictim_WHDD_c3 / SSDvictim_c3 = ratio of dirty victim in SSD
	/*
	Trssd + Twmem + ...
	
	*/
	int PCReadMiss_to_SSDRead_Miss;	//case4
	//PCReadMiss_to_SSDRead_Miss = PCReadMiss_to_SSDRead - PCReadMiss_to_SSDRead_Hit

	int upingpage_GB_Hit;
	int T2victim_c4;
	int T2victim_WSSD_c4;//write SSD
	int T2e_SSDvictim_c4;
	int T2e_SSDvictim_WHDD_c4;

	int upingpage_GB_Miss;
	int T1victim_c4;
	int T1e_GB_Hit_c4; 
	int T1victim_WSSD_c4;//write SSD
	int T1e_SSDvictim_c4;
	int T1e_SSDvictim_WHDD_c4;
	int T1e_GB_Miss_c4;
	/* 
	*/
	//for check
	int PCWrite_Miss;			//case7+case8
	//PCWrite_Miss = PageCache_Miss - PCReadMiss_to_SSDRead

	int T2victim_c7;				//case7
	int T2victim_WSSD_c7;//write SSD
	int SSDvictim_c7;
	int SSDvictim_WHDD_c7;	

	int T1victim_c8;				//case8
	int T1victim_WSSD_c8;//write SSD
	int T1e_GB_Hit_c8;
	int SSDvictim_c8;
	int SSDvictim_WHDD_c8;
	int T1e_GB_Miss_c8;

}pcSTAT;

//tmp infromation
static pcSTAT info = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static pcSTAT predict_info;
static pcSTAT actual_info; 

static pcSTAT predict_next_info;

static double SSDPeriodTime = 0, HDDPeriodTime = 0;

extern int T1ReadHit[PageCacheSize+1], T2ReadHit[PageCacheSize+1];
extern int T1WriteHit[PageCacheSize+1], T2WriteHit[PageCacheSize+1];
extern int T2CleanArray[PageCacheSize];

typedef struct HitSTATUS{

	int C1rm[NUM_BLOCKS];
	int C1wm[NUM_BLOCKS];
	int C2rm[NUM_BLOCKS];
	int C2wm[NUM_BLOCKS];

}BLOCK_miss_record;

static BLOCK_miss_record MRB = { {0}, {0} };//miss counts record in every blocks
static BLOCK_miss_record keeped_MRB;

void writeCheckFile(FILE **st, pcSTAT in);
pcSTAT Predict_Record(pcSTAT in);
pcSTAT Calculate_predict(pcSTAT actual, pcSTAT predict); 
pcSTAT Clean_Record(pcSTAT in);
BLOCK_miss_record keep_Hit_ratio_in_BLOCKS();
BLOCK_miss_record Clean_MBR(BLOCK_miss_record mbr);

void AdjustT1T2Size(double max_arrival_SSD, double max_arrival_HDD, BLOCK_miss_record mrb, pcSTAT in);

static double CostTime[NUM_BLOCKS] = {0.0};
static double Time_T1ReadMiss = 0.0;
static double Time_T2ReadMiss = 0.0;
static double Time_T1WriteMiss = 0.0;
static double Time_T2WriteMiss = 0.0;

extern double Time_SSDRead;
extern double Time_HDDRead;
extern double Time_SSDWrite;
extern double Time_HDDWrite;
extern double TotalPeriod_Time_SSDRead;
extern int counts_SSDRead;
extern double TotalPeriod_Time_SSDWrite;
extern int counts_SSDWrite;
extern double TotalPeriod_Time_HDDRead;
extern int counts_HDDRead;
extern double TotalPeriod_Time_HDDWrite;
extern int counts_HDDWrite;

//QD
static int Period_pages_in_SSD = 0;
static int Period_pages_in_HDD = 0;
static double AvgTime_dealing_pages_SSD = 0.0;
static double AvgTime_dealing_pages_HDD = 0.0;
static double inter_arrival_time_SSD = 0.0;
static double inter_arrival_time_HDD = 0.0;
static double Avg_SSD_QD = 0.0, Avg_HDD_QD = 0.0;
static double old_Avg_SSD_QD = 0.0, old_Avg_HDD_QD = 0.0;

//Time_T1ReadMiss
static double Ratio_upcoming_Hit_GB;
static double Ratio_upcoming_Miss_GB;
static double Ratio_dirty_T2_c4;
static double Ratio_dirty_T2_SSD_c4;
static double Ratio_dirty_T1_c4;
static double Ratio_dirty_T1_Hit_GB_c4;
static double Ratio_dirty_T1_Miss_GB_c4;
static double Ratio_dirty_T1_SSD_c4;
//Time_T1WriteMiss
static double Ratio_dirty_T1_c8;
static double Ratio_dirty_T1_Hit_GB_c8;
static double Ratio_dirty_T1_Miss_GB_c8;
static double Ratio_dirty_T1_SSD_c8;
//Time_T2ReadMiss
static double Ratio_dirty_T2_c3;
static double Ratio_dirty_T2_SSD_c3;
//Time_T2WriteMiss
static double Ratio_dirty_T2_c7;
static double Ratio_dirty_T2_SSD_c7;

//static double Ratio_upcoming_Hit_GB;




#endif
