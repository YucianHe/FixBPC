#include "yc_dynamic.h"

void writeCheckFile(FILE **st, pcSTAT in) 
{
	fprintf(*st, "==================================================\n");
	
	fprintf(*st, "period = %d\n", in.period);

	fprintf(*st, "total_pages = %d\n", in.total_pages);
	fprintf(*st, "PageCache_HitCount = %d\n", in.PageCache_Hit);

	if(( in.T1_Read_Hit_Count + in.T1_Write_Hit_Count + in.T2_Read_Hit_Count + in.T2_Write_Hit_Count ) == in.PageCache_Hit)
		fprintf(*st, "Hit count is correct\n");
	else
		fprintf(*st, "Hit count is noooooooooooooooot correct\n");

	double PageCacheHitRatio;
	if(in.PageCache_Hit != 0 || in.total_pages != 0)
	{
		PageCacheHitRatio = (double)in.PageCache_Hit / in.total_pages;
		fprintf(*st, "PageCacheHitRatio = %lf\n", PageCacheHitRatio);
	}
	else
	{
		PageCacheHitRatio = 0;
		fprintf(*st, "PageCacheHitRatio = %lf\n", (double)PageCacheHitRatio);
	}


	int total_READ_pages = 0, total_WRITE_pages = 0;
	total_READ_pages = in.T1_Read_Hit_Count + in.T2_Read_Hit_Count + in.PCReadMiss_to_SSDRead;
	total_WRITE_pages = in.T1_Write_Hit_Count + in.T2_Write_Hit_Count + in.PCWrite_Miss;

	fprintf(*st, "Ttotal_READ_pages = %d\n", total_READ_pages);
	fprintf(*st, "total_WRITE_pages = %d\n", total_WRITE_pages);

	fprintf(*st, "++++++++++++++++++++++++++++++++++++++++++++++++\n");

	int ii;
	double T1ReadHitsum = 0;
	//fprintf(*st, "T1Size = %d\n", T1Size);
	fprintf(*st, "T1_Read_Hit_Count = %d\n", in.T1_Read_Hit_Count);
	for(ii= 0; ii <  PageCacheSize; ii++)
	{	
		if(T1ReadHit[ii] != 0){
			fprintf(*st, "T1ReadHit[%d] = %d\t", ii, T1ReadHit[ii]);
			T1ReadHitsum += T1ReadHit[ii];
			//printf("T1ReadHitsum[%d] = %d\n", i, T1ReadHitsum);

			fprintf(*st, "T1ReadHitRatio[%d] = %f\n", ii, (double)T1ReadHitsum / in.total_pages);
		}		
	}
	fprintf(*st, "++++++++++++++++++++++++++++++++++++++++++++++++\n");

	int m;
	double T1WriteHitsum = 0;
	fprintf(*st, "T1_Write_HitCount = %d\t\n", in.T1_Write_Hit_Count);
	for(m = 0; m <  PageCacheSize; m++)
	{
		if(T1WriteHit[m] != 0){
			fprintf(*st, "T1WriteHit[%d] = %d\t", m, T1WriteHit[m]);
			T1WriteHitsum += T1WriteHit[m];
			//printf("T1Hitsum[%d] = %f\n", i, T1Hitsum);
			fprintf(*st, "T1WriteHitRatio[%d] = %f\n", m, (double)T1WriteHitsum / in.total_pages);
		}
		
	}
	fprintf(*st, "++++++++++++++++++++++++++++++++++++++++++++++++\n");

	int j;
	double T2ReadHitsum = 0;
	//fprintf(*st, "T2Size = %d\n", T2Size);
	fprintf(*st, "T2_Read_HitCount = %d\t\n", in.T2_Read_Hit_Count);
	for(j = 0; j < PageCacheSize; j++)
	{	
		if(T2ReadHit[j] != 0){
			fprintf(*st, "T2ReadHit[%d] = %d\t", j, T2ReadHit[j]);
			T2ReadHitsum += T2ReadHit[j];
			//printf("T2Hitsum[%d] = %f\n", j, T2Hitsum);
			fprintf(*st, "T2ReadHitRatio[%d] = %f\n", j, (double)T2ReadHitsum / in.total_pages);
		}
		
	}
	fprintf(*st, "++++++++++++++++++++++++++++++++++++++++++++++++\n");

	int n;
	double T2WriteHitsum = 0;
	fprintf(*st, "T2_Write_HitCount = %d\t\n", in.T2_Write_Hit_Count);
	for(n = 0; n <  PageCacheSize; n++)
	{
		if(T2WriteHit[n] != 0){
			fprintf(*st, "T2WriteHit[%d] = %d\t", n, T2WriteHit[n]);
			T2WriteHitsum += T2WriteHit[n];
			//printf("T1Hitsum[%d] = %f\n", i, T1Hitsum);
			fprintf(*st, "T2WriteHitRatio[%d] = %f\n", n,(double) T2WriteHitsum / in.total_pages);
		}	
		
	}
	fprintf(*st, "++++++++++++++++++++++++++++++++++++++++++++++++\n");

}

BLOCK_miss_record keep_Hit_ratio_in_BLOCKS(BLOCK_miss_record mbr)
{
	printf("[keep_Hit_ratio_in_BLOCKS]\n");

	int T1_i, T2_i, j;
	int  pages_in_block = PageCacheSize / NUM_BLOCKS;
	int tmp_T2Size = 0,Read_hit_to_miss_count = 0, Write_hit_to_miss_count = 0;
	NODE *win_head;

	fprintf(statistics, "#dy.c 107 NUM_BLOCKS : %d, pages_in_block : %d\n", NUM_BLOCKS, pages_in_block);
		
	mbr = Clean_MBR(mbr);
	printf("116[Clean_MBR DONE]\n");
	//PrintMissRatio_in_BLOCKS(mbr, "#dy.c#110 before keep to Clean_MBR");
	//KeepT2CleanArray();
	//printf("118[KeepT2CleanArray DONE]\n");

	T1_i = NUM_BLOCKS -1;
	while(T1_i >= 0)
	{
		//fprintf(statistics, "i : %d\n", i);			
		if(T1_i < NUM_BLOCKS -1)
		{
			for(j = 0; j < pages_in_block; j++)
			{	
				int postion = j + (T1_i+1) * pages_in_block;
				
				mbr.C1rm[T1_i] += T1ReadHit[ postion ]; 
				mbr.C1wm[T1_i] += T1WriteHit[ postion ]; 
				mbr.C2rm[T1_i] += T2ReadHit[ postion ]; 
				mbr.C2wm[T1_i]  += T2WriteHit[ postion ]; 						
			}
			
			mbr.C1rm[T1_i] += mbr.C1rm[T1_i+1];
			mbr.C1wm[T1_i] += mbr.C1wm[T1_i+1];
			mbr.C2rm[T1_i] += mbr.C2rm[T1_i+1];
			mbr.C2wm[T1_i] += mbr.C2wm[T1_i+1];

			/*
			tmp_T2Size = (T1_i+1)*pages_in_block;
			//fprintf(statistics, "tmp_T2Size : %d\n", tmp_T2Size);	
			int m = 0;
			for(m = tmp_T2Size - (tmp_T2Size/4)+1; m < tmp_T2Size; m++)
			{
				//fprintf(statistics, "tmp_T2Size = %d\n", tmp_T2Size);
				if(T2CleanArray[m] == 1)
				{
					Read_hit_to_miss_count += T2ReadHit[m];
					Write_hit_to_miss_count += T2WriteHit[m];

					if(T2ReadHit[m] != 0 || T2WriteHit[m] != 0)
					{
						fprintf(statistics, "T2ReadHit[%d] = %d\n", m, T2ReadHit[m]);
						fprintf(statistics, "T2WriteHit[%d] = %d\n", m, T2WriteHit[m]);				
					
						fprintf(statistics, "Read_hit_to_miss_count = %d\n", Read_hit_to_miss_count);
						fprintf(statistics, "Write_hit_to_miss_count = %d\n", Write_hit_to_miss_count);
					}					
				}
			}

			mbr.C2rm[T1_i] = mbr.C2rm[T1_i] + Read_hit_to_miss_count / 2;
			mbr.C2wm[T1_i] = mbr.C2wm[T1_i] + Write_hit_to_miss_count / 2;
			*/
			
			fprintf(statistics, "++++++++++++++++++++++++++++++++++++++++++++++++\n");
			fprintf(statistics, "mbr.C1rm[%d] : %d\n", T1_i, mbr.C1rm[T1_i]);
			fprintf(statistics, "mbr.C1wm[%d] : %d\n", T1_i, mbr.C1wm[T1_i]);
			fprintf(statistics, "mbr.C2rm[%d] : %d\n", T1_i, mbr.C2rm[T1_i]);
			fprintf(statistics, "mbr.C2wm[%d] : %d\n", T1_i, mbr.C2wm[T1_i]);
			fprintf(statistics, "++++++++++++++++++++++++++++++++++++++++++++++++\n");
									
		}		
		else
		{
			//fprintf(statistics, "else+++++++++++++++++++++++++++++++++++++++++\n");			
			
			mbr.C1rm[T1_i]  = T1ReadHit[ PageCacheSize ];
			mbr.C1wm[T1_i]  = T1WriteHit[ PageCacheSize ]; 
			mbr.C2rm[T1_i]  = T2ReadHit[ PageCacheSize ];
			mbr.C2wm[T1_i]  = T2WriteHit[ PageCacheSize ]; 
			/*
			fprintf(statistics, "mbr.C1rm[%d] : %d\n", i, mbr.C1rm[i]);
			fprintf(statistics, "mbr.C1wm[%d] : %d\n", i, mbr.C1wm[i]);	
			fprintf(statistics, "mbr.C2rm[%d] : %d\n", i, mbr.C2rm[i]);
			fprintf(statistics, "mbr.C2wm[%d] : %d\n", i, mbr.C2wm[i]);		
			fprintf(statistics, "++++++++++++++++++++++++++++++++++++++++++++++++\n");
			*/

		}
		T1_i--;
	}	

	//PrintMissRatio_in_BLOCKS(mbr, "dy.c keep record of MRB");
	return mbr;
}


pcSTAT Predict_Record(pcSTAT in)
{
	if(in.period == 0)
	{

		fprintf(statistics, "in.period = %d!\n", in.period);

		//predict_info = in;
		//PrintINFO(in, "predict_info[preiod 0]");
		//actual_info = in;
		//PrintINFO(in, "actual_info[preiod 0]");

		predict_next_info = Calculate_predict(in, in);
		//PrintINFO(predict_next_info, "predict_next_info[preiod 0]");
		
		//fprintf(statistics, "Adjust T1 T2 by  predict_next_info\n");
	}
	else
	{
		fprintf(statistics, "in.period = %d!\n", in.period);

		//PrintINFO(in, "actual_info");
		//PrintINFO(predict_next_info, "predict_last_info");
	
		predict_next_info = Calculate_predict(in, predict_next_info);
		//PrintINFO(predict_next_info, "[After set]predict_next_info");

		//printf(statistics, "Adjust T1 T2 by  predict_next_info\n");
	}

	return predict_next_info;
}

pcSTAT Calculate_predict(pcSTAT actual, pcSTAT predict)
{
	pcSTAT cal;

	cal.period = actual.period;

	double Alpha = 0.5;

	cal.PageCache_Hit = Alpha*actual.PageCache_Hit + (1- Alpha)*predict.PageCache_Hit;

	cal.T1_Read_Hit_Count = Alpha*actual.T1_Read_Hit_Count + (1- Alpha)*predict.T1_Read_Hit_Count;		//case1
	cal.T1_Write_Hit_Count = Alpha*actual.T1_Write_Hit_Count + (1- Alpha)*predict.T1_Write_Hit_Count;		//case5
	cal.T2_Read_Hit_Count = Alpha*actual.T2_Read_Hit_Count + (1- Alpha)*predict.T2_Read_Hit_Count;		//case2
	cal.T2_Write_Hit_Count = Alpha*actual.T2_Write_Hit_Count + (1- Alpha)*predict.T2_Write_Hit_Count;		//case6

	cal.PageCache_Miss = Alpha*actual.PageCache_Miss + (1- Alpha)*predict.PageCache_Miss;

	cal.PCReadMiss_to_SSDRead = Alpha*actual.PCReadMiss_to_SSDRead + (1- Alpha)*predict.PCReadMiss_to_SSDRead;			//case3+case4

	cal.PCReadMiss_to_SSDRead_Hit = Alpha*actual.PCReadMiss_to_SSDRead_Hit + (1- Alpha)*predict.PCReadMiss_to_SSDRead_Hit; 	//case3
	cal.T2victim_c3 = Alpha*actual.T2victim_c3 + (1- Alpha)*predict.T2victim_c3;
	cal.T2victim_WSSD_c3 = Alpha*actual.T2victim_WSSD_c3 + (1- Alpha)*predict.T2victim_WSSD_c3;
	cal.SSDvictim_c3 = Alpha*actual.SSDvictim_c3 + (1- Alpha)*predict.SSDvictim_c3;
	cal.SSDvictim_WHDD_c3 = Alpha*actual.SSDvictim_WHDD_c3 + (1- Alpha)*predict.SSDvictim_WHDD_c3;
	

	cal.PCReadMiss_to_SSDRead_Miss = Alpha*actual.PCReadMiss_to_SSDRead_Miss + (1- Alpha)*predict.PCReadMiss_to_SSDRead_Miss;	//case4
	
	cal.upingpage_GB_Hit = Alpha*actual.upingpage_GB_Hit + (1- Alpha)*predict.upingpage_GB_Hit;
	cal.T2victim_c4 = Alpha*actual.T2victim_c4 + (1- Alpha)*predict.T2victim_c4;
	cal.T2victim_WSSD_c4 = Alpha*actual.T2victim_WSSD_c4 + (1- Alpha)*predict.T2victim_WSSD_c4;
	cal.T2e_SSDvictim_c4 = Alpha*actual.T2e_SSDvictim_c4 + (1- Alpha)*predict.T2e_SSDvictim_c4;
	cal.T2e_SSDvictim_WHDD_c4 = Alpha*actual.T2e_SSDvictim_WHDD_c4 + (1- Alpha)*predict.T2e_SSDvictim_WHDD_c4;

	cal.upingpage_GB_Miss = Alpha*actual.upingpage_GB_Miss + (1- Alpha)*predict.upingpage_GB_Miss;
	cal.T1victim_c4 = Alpha*actual.T1victim_c4 + (1- Alpha)*predict.T1victim_c4;
	cal.T1e_GB_Hit_c4 = Alpha*actual.T1e_GB_Hit_c4 + (1- Alpha)*predict.T1e_GB_Hit_c4;
	cal.T1victim_WSSD_c4 = Alpha*actual.T1victim_WSSD_c4 + (1- Alpha)*predict.T1victim_WSSD_c4;
	cal.T1e_SSDvictim_c4 = Alpha*actual.T1e_SSDvictim_c4 + (1- Alpha)*predict.T1e_SSDvictim_c4;
	cal.T1e_SSDvictim_WHDD_c4 = Alpha*actual.T1e_SSDvictim_WHDD_c4 + (1- Alpha)*predict.T1e_SSDvictim_WHDD_c4;
	cal.T1e_GB_Miss_c4 = Alpha*actual.T1e_GB_Miss_c4 + (1- Alpha)*predict.T1e_GB_Miss_c4;
	/* 
	*/
	cal.PCWrite_Miss = Alpha*actual.PCWrite_Miss + (1- Alpha)*predict.PCWrite_Miss;				//case7+case8

	cal.T2victim_c7 = Alpha*actual.T2victim_c7 + (1- Alpha)*predict.T2victim_c7;				//case7
	cal.T2victim_WSSD_c7= Alpha*actual.T2victim_WSSD_c7 + (1- Alpha)*predict.T2victim_WSSD_c7;
	cal.SSDvictim_c7 = Alpha*actual.SSDvictim_c7 + (1- Alpha)*predict.SSDvictim_c7;
	cal.SSDvictim_WHDD_c7 = Alpha*actual.SSDvictim_WHDD_c7 + (1- Alpha)*predict.SSDvictim_WHDD_c7;

	cal.T1victim_c8 = Alpha*actual.T1victim_c8 + (1- Alpha)*predict.T1victim_c8;				//case8
	cal.T1victim_WSSD_c8 = Alpha*actual.T1victim_WSSD_c8 + (1- Alpha)*predict.T1victim_WSSD_c8;
	cal.T1e_GB_Hit_c8 = Alpha*actual.T1e_GB_Hit_c8 + (1- Alpha)*predict.T1e_GB_Hit_c8;
	cal.SSDvictim_c8 = Alpha*actual.SSDvictim_c8 + (1- Alpha)*predict.SSDvictim_c8;
	cal.SSDvictim_WHDD_c8 = Alpha*actual.SSDvictim_WHDD_c8 + (1- Alpha)*predict.SSDvictim_WHDD_c8;
	cal.T1e_GB_Miss_c8 = Alpha*actual.T1e_GB_Miss_c8 + (1- Alpha)*predict.T1e_GB_Miss_c8;

	PrintINFO(cal, "after predict");
	return cal;
}

pcSTAT Clean_Record(pcSTAT in)
{
	fprintf(result, "Clean_Record!\n");

	in.PageCache_Hit = 0;

	in.T1_Read_Hit_Count = 0;			//case1
	in.T1_Write_Hit_Count = 0;			//case5
	in.T2_Read_Hit_Count = 0;			//case2
	in.T2_Write_Hit_Count =0;			//case6

	in.PageCache_Miss = 0;

	in.PCReadMiss_to_SSDRead= 0;			//case3+case4

	in.PCReadMiss_to_SSDRead_Hit = 0; 	//case3
	in.T2victim_c3 = 0;
	in.T2victim_WSSD_c3 = 0;
	//T2victim_WSSD_c3 / T2victim_c3 = ratio of dirty victim in T2
	in.SSDvictim_c3 = 0;
	in.SSDvictim_WHDD_c3 = 0;
	//SSDvictim_WHDD_c3 / SSDvictim_c3 = ratio of dirty victim in SSD
	/*
	Trssd + Twmem + ...
	
	*/

	in.PCReadMiss_to_SSDRead_Miss = 0;	//case4
	
	in.upingpage_GB_Hit = 0;
	in.T2victim_c4 = 0;
	in.T2victim_WSSD_c4 = 0 ;
	in.T2e_SSDvictim_c4 = 0;
	in.T2e_SSDvictim_WHDD_c4 = 0;

	in.upingpage_GB_Miss = 0;
	in.T1victim_c4 = 0;
	in.T1e_GB_Hit_c4 = 0;
	in.T1victim_WSSD_c4 = 0;
	in.T1e_SSDvictim_c4 = 0;
	in.T1e_SSDvictim_WHDD_c4 = 0;
	in.T1e_GB_Miss_c4 = 0;
	/* 
	*/
	in.PCWrite_Miss = 0;			//case7+case8

	in.T2victim_c7 = 0;				//case7
	in.T2victim_WSSD_c7 = 0;
	in.SSDvictim_c7 = 0;
	in.SSDvictim_WHDD_c7 = 0;	

	in.T1victim_c8 = 0;				//case8
	in.T1victim_WSSD_c8 = 0;
	in.T1e_GB_Hit_c8 = 0;
	in.SSDvictim_c8 = 0;
	in.SSDvictim_WHDD_c8 = 0;
	in.T1e_GB_Miss_c8 = 0;	


	int i, j;
	for(i = 0; i <= PageCacheSize ; i++)
	{
		T1ReadHit[i] = 0;
		T1WriteHit[i] = 0;
		T2ReadHit[i] = 0;
		T2WriteHit[i] = 0;		
		
		/*
		if(T1ReadHit[i] ||T2ReadHit[i]||T1WriteHit[i]||T2WriteHit[i] != 0)
		{
			fprintf(statistics, "dy.c#351 ++++++++++++++++++++++++++++++++++++++++++++++++\n");

			fprintf(statistics, "T1ReadHit[ %d ] : %d\n", i, T1ReadHit[ i ]);
			fprintf(statistics, "T1WriteHit[ %d ] : %d\n", i, T1WriteHit[ i ]);
			fprintf(statistics, "T2ReadHit[ %d ] : %d\n", i, T2ReadHit[ i ]);
			fprintf(statistics, "T2WriteHit[ %d ] : %d\n", i, T2WriteHit[ i ]);	

			fprintf(statistics, "++++++++++++++++++++++++++++++++++++++++++++++++\n");
		}
		*/
	}

	return in;
}

BLOCK_miss_record Clean_MBR(BLOCK_miss_record mbr)
{
	int i;
	for(i = 0; i < NUM_BLOCKS; i++)	{	

		mbr.C1rm[i] = 0;
		mbr.C1wm[i] = 0;
		mbr.C2rm[i] = 0;
		mbr.C2wm[i] = 0;

	}
	//PrintMissRatio_in_BLOCKS(mbr, "dy.c#Clean_MBR");

	return mbr;
}

void Clean_Times()
{
	Time_SSDRead = 0.0;
	Time_HDDRead = 0.0;
	Time_SSDWrite = 0.0;
	Time_HDDWrite = 0.0;

	TotalPeriod_Time_SSDRead = 0.0;
	TotalPeriod_Time_SSDWrite = 0.0;
	TotalPeriod_Time_HDDRead = 0.0;
	TotalPeriod_Time_HDDWrite = 0.0;

	counts_SSDRead = 0;
	counts_SSDWrite = 0;
	counts_HDDRead = 0;
	counts_HDDWrite = 0;

}

void AdjustT1T2Size(double Ptime_SSD, double Ptime_HDD, BLOCK_miss_record mrb, pcSTAT in)
{
	fprintf(statistics, "dy.c#388 SSDPeriodTime = %lf\n", TotalPeriod_Time_SSDRead + TotalPeriod_Time_SSDWrite);	
	fprintf(statistics, "dy.c#389 HDDPeriodTime = %lf\n", TotalPeriod_Time_HDDRead + TotalPeriod_Time_HDDWrite);
	//PrintMissRatio_in_BLOCKS(MRB, "dy.c#640 in AdjustT1T2Size");
	
	fprintf(statistics, "T1Size : %d\n", T1Size);
	fprintf(statistics, "T2Size : %d\n", T2Size);
	fprintf(statistics, "PageCacheSize : %d\n", PageCacheSize);
	fprintf(statistics, "NUM_BLOCKS : %d\n\n", NUM_BLOCKS);	

	//To get average SSDRead time
	fprintf(statistics, "TotalPeriod_Time_SSDRead : %lf\n", TotalPeriod_Time_SSDRead);
	fprintf(statistics, "counts_SSDRead : %d\n", counts_SSDRead);
	if(counts_SSDRead == 0)
		Time_SSDRead = 0.130600;
	else
		Time_SSDRead = TotalPeriod_Time_SSDRead / counts_SSDRead;
	fprintf(statistics, "Avg_Time_SSDRead : %lf\n", Time_SSDRead);

	//To get average SSDWrite time
	fprintf(statistics, "TotalPeriod_Time_SSDWrite : %lf\n", TotalPeriod_Time_SSDWrite);
	fprintf(statistics, "counts_SSDWrite : %d\n", counts_SSDWrite);	
	if(counts_SSDWrite == 0)
		Time_SSDWrite = 0.305600;
	else
		Time_SSDWrite = TotalPeriod_Time_SSDWrite / counts_SSDWrite;
	fprintf(statistics, "Avg_Time_SSDWrite : %lf\n\n", Time_SSDWrite);

	//To get average HDDRead time
	fprintf(statistics, "TotalPeriod_Time_HDDRead : %lf\n", TotalPeriod_Time_HDDRead);
	fprintf(statistics, "counts_HDDRead : %d\n", counts_HDDRead);
	if(counts_HDDRead == 0)
		Time_HDDRead = 0.5;
	else
		Time_HDDRead = TotalPeriod_Time_HDDRead / counts_HDDRead;
	fprintf(statistics, "Avg_Time_HDDRead : %lf\n", Time_HDDRead);

	//To get average HDDWrite time
	fprintf(statistics, "TotalPeriod_Time_HDDWrite : %lf\n", TotalPeriod_Time_HDDWrite);
	fprintf(statistics, "counts_HDDWrite : %d\n", counts_HDDWrite);	
	if(counts_HDDWrite == 0)
		Time_HDDWrite = 0.5;
	else
		Time_HDDWrite = TotalPeriod_Time_HDDWrite / counts_HDDWrite;
	fprintf(statistics, "Avg_Time_HDDWrite : %lf\n\n", Time_HDDWrite);
	fprintf(statistics, "++++++++++++++++++++++++++++++++++++++++++++++++\n");


	//calculate QD
	fprintf(statistics, "Ptime_SSD : %lf\n", Ptime_SSD);
	fprintf(statistics, "Ptime_HDD : %lf\n", Ptime_HDD);

	Period_pages_in_SSD = counts_SSDRead + counts_SSDWrite;
	Period_pages_in_HDD = counts_HDDRead + counts_HDDWrite;
	fprintf(statistics, "Period_pages_in_SSD : %d\n", Period_pages_in_SSD);
	fprintf(statistics, "Period_pages_in_HDD : %d\n\n", Period_pages_in_HDD);
	if(Period_pages_in_SSD != 0)
		AvgTime_dealing_pages_SSD = (double)(TotalPeriod_Time_SSDRead + TotalPeriod_Time_SSDWrite) / Period_pages_in_SSD;
	if(Period_pages_in_HDD != 0)
		AvgTime_dealing_pages_HDD = (double)(TotalPeriod_Time_HDDRead + TotalPeriod_Time_HDDWrite) / Period_pages_in_HDD;
	fprintf(statistics, "AvgTime_dealing_pages_SSD : %lf\n", AvgTime_dealing_pages_SSD);
	fprintf(statistics, "AvgTime_dealing_pages_HDD : %lf\n\n", AvgTime_dealing_pages_HDD);

	if(Period_pages_in_SSD == 0)
		inter_arrival_time_SSD = 0.0;
	else
		inter_arrival_time_SSD = Ptime_SSD / Period_pages_in_SSD;

	if(Period_pages_in_HDD == 0)
		inter_arrival_time_HDD = 0.0;
	else
		inter_arrival_time_HDD = Ptime_HDD / Period_pages_in_HDD;
	
	fprintf(statistics, "inter_arrival_time_SSD : %lf\n", inter_arrival_time_SSD);
	fprintf(statistics, "inter_arrival_time_HDD : %lf\n\n", inter_arrival_time_HDD);
	
	if(AvgTime_dealing_pages_SSD > inter_arrival_time_SSD)
	{
		double d = AvgTime_dealing_pages_SSD - inter_arrival_time_SSD;

		Avg_SSD_QD = 0.5*(( 2*d + (Period_pages_in_SSD -1)*d) /2) + 0.5*old_Avg_SSD_QD;
		fprintf(statistics, "Avg_SSD_QD : %lf\n", Avg_SSD_QD);

		old_Avg_SSD_QD = Avg_SSD_QD;
	}
	else
	{
		Avg_SSD_QD = 0.0;
		fprintf(statistics, "Avg_SSD_QD : %lf\n\n", Avg_SSD_QD);
	}


	if(AvgTime_dealing_pages_HDD > inter_arrival_time_HDD)
	{
		double d = AvgTime_dealing_pages_HDD - inter_arrival_time_HDD;

		Avg_HDD_QD = 0.5*(( 2*d + (Period_pages_in_HDD -1)*d) /2)+ 0.5*old_Avg_HDD_QD;
		fprintf(statistics, "Avg_HDD_QD : %lf\n", Avg_HDD_QD);	

		old_Avg_HDD_QD = Avg_HDD_QD;
	}
	else
	{
		Avg_HDD_QD = 0.0;
		fprintf(statistics, "Avg_HDD_QD : %lf\n\n", Avg_HDD_QD);
	}
	
	int tmp_i ;
	//int tmp_i = ((double)T1Size / PageCacheSize) * (double)NUM_BLOCKS -1;
	//fprintf(statistics, "tmp_i : %d\n", tmp_i);

	fprintf(statistics, "++++++++++++++++++++++++++++++++++++++++++++++++\n");

	//Case4
	if(in.upingpage_GB_Hit + in.upingpage_GB_Miss != 0){
		Ratio_upcoming_Hit_GB = (double)in.upingpage_GB_Hit / (double)(in.upingpage_GB_Hit + in.upingpage_GB_Miss);
		Ratio_upcoming_Miss_GB = (double)in.upingpage_GB_Miss / (double)(in.upingpage_GB_Hit + in.upingpage_GB_Miss);
	}
	else
	{
		Ratio_upcoming_Hit_GB = 0.0;
		Ratio_upcoming_Miss_GB = 0.0;
	}
	
	if( in.T2victim_c4 != 0)
		Ratio_dirty_T2_c4 = (double)in.T2victim_WSSD_c4 / (double)in.T2victim_c4;
	else
		Ratio_dirty_T2_c4 = 0.0;

	if( in.T2e_SSDvictim_c4 != 0 )
		Ratio_dirty_T2_SSD_c4 = (double)in.T2e_SSDvictim_WHDD_c4 / (double)in.T2e_SSDvictim_c4;
	else
		Ratio_dirty_T2_SSD_c4 = 0.0;

	if( in.T1victim_c4 != 0)
		Ratio_dirty_T1_c4 = (double)in.T1victim_WSSD_c4 / (double)in.T1victim_c4;
	else
		Ratio_dirty_T1_c4 = 0.0;

	if( in.T1e_GB_Hit_c4 + in.T1e_GB_Miss_c4 != 0){
		Ratio_dirty_T1_Hit_GB_c4 = (double)in.T1e_GB_Hit_c4 / (double)(in.T1e_GB_Hit_c4 + in.T1e_GB_Miss_c4);
		Ratio_dirty_T1_Miss_GB_c4 = (double)in.T1e_GB_Miss_c4 / (double)(in.T1e_GB_Hit_c4 + in.T1e_GB_Miss_c4);
	}
	else
	{
		Ratio_dirty_T1_Hit_GB_c4 = 0.0;
		Ratio_dirty_T1_Miss_GB_c4 = 0.0;
	}

	if( in.T1e_SSDvictim_c4 != 0)
		Ratio_dirty_T1_SSD_c4  = (double)in.T1e_SSDvictim_WHDD_c4 / (double)in.T1e_SSDvictim_c4;
	else
		Ratio_dirty_T1_SSD_c4 = 0.0; 
	
	
	fprintf(statistics, "Time_T1ReadMiss-----------------------------------------------------------------\n");
	fprintf(statistics, "upingpage_GB_Hit : %d\n", in.upingpage_GB_Hit);
	fprintf(statistics, "upingpage_GB_Miss : %d\n", in.upingpage_GB_Miss);
	fprintf(statistics, "Ratio_upcoming_Hit_GB : %lf\n", Ratio_upcoming_Hit_GB);
	fprintf(statistics, "Ratio_upcoming_Miss_GB : %lf\n\n", Ratio_upcoming_Miss_GB);

	fprintf(statistics, "T2victim_WSSD_c4 : %d\n", in.T2victim_WSSD_c4);
	fprintf(statistics, "T2victim_c4 : %d\n", in.T2victim_c4);
	fprintf(statistics, "Ratio_dirty_T2_c4 : %lf\n\n", Ratio_dirty_T2_c4);

	fprintf(statistics, "T2victim_WSSD_c4 : %d\n", in.T2victim_WSSD_c4);
	fprintf(statistics, "T2victim_c4 : %d\n", in.T2victim_c4);
	fprintf(statistics, "Ratio_dirty_T2_SSD_c4 : %lf\n\n", Ratio_dirty_T2_SSD_c4);

	fprintf(statistics, "T2victim_WSSD_c4 : %d\n", in.T1victim_WSSD_c4);
	fprintf(statistics, "T2victim_c4 : %d\n", in.T1victim_c4);
	fprintf(statistics, "Ratio_dirty_T1_c4 : %lf\n\n", Ratio_dirty_T1_c4);

	fprintf(statistics, "T1e_GB_Hit_c4 : %d\n", in.T1e_GB_Hit_c4);
	fprintf(statistics, "T1e_GB_Miss_c4 : %d\n", in.T1e_GB_Miss_c4);
	fprintf(statistics, "Ratio_dirty_T1_Hit_GB_c4 : %lf\n", Ratio_dirty_T1_Hit_GB_c4);
	fprintf(statistics, "Ratio_dirty_T1_Miss_GB_c4 : %lf\n\n", Ratio_dirty_T1_Miss_GB_c4);

	fprintf(statistics, "T1e_SSDvictim_WHDD_c4 : %d\n", in.T1e_SSDvictim_WHDD_c4);
	fprintf(statistics, "T1e_SSDvictim_c4 : %d\n", in.T1e_SSDvictim_c4);
	fprintf(statistics, "Ratio_dirty_T1_SSD_c4 : %lf\n", Ratio_dirty_T1_SSD_c4);
	fprintf(statistics, "-----------------------------------------------------------------\n");
	
	//Case8
	if( in.T1victim_c8 != 0)
		Ratio_dirty_T1_c8 = (double)in.T1victim_WSSD_c8 / (double)in.T1victim_c8;
	else
		Ratio_dirty_T1_c8 = 0.0;

	if( in.T1e_GB_Hit_c8 + in.T1e_GB_Miss_c8 != 0)
	{
		Ratio_dirty_T1_Hit_GB_c8 = (double)in.T1e_GB_Hit_c8 / (double)(in.T1e_GB_Hit_c8 + in.T1e_GB_Miss_c8);
		Ratio_dirty_T1_Miss_GB_c8 = (double)in.T1e_GB_Miss_c8 / (double)(in.T1e_GB_Hit_c8 + in.T1e_GB_Miss_c8);
	
	}
	else{

		Ratio_dirty_T1_Hit_GB_c8 = 0.0;
		Ratio_dirty_T1_Miss_GB_c8 = 0.0;
	}
	
	if( in.SSDvictim_c8 != 0)
		Ratio_dirty_T1_SSD_c8 = (double)in.SSDvictim_WHDD_c8 / (double)in.SSDvictim_c8;
	else
		Ratio_dirty_T1_SSD_c8 = 0.0;

	fprintf(statistics, "Time_T1WriteMiss-----------------------------------------------------------------\n");
	fprintf(statistics, "T1victim_WSSD_c8 : %d\n", in.T1victim_WSSD_c8);
	fprintf(statistics, "T1victim_c8 : %d\n", in.T1victim_c8);
	fprintf(statistics, "Ratio_dirty_T1_c8 : %lf\n\n", Ratio_dirty_T1_c8);

	fprintf(statistics, "T1e_GB_Hit_c8 : %d\n", in.T1e_GB_Hit_c8);
	fprintf(statistics, "T1e_GB_Miss_c8 : %d\n", in.T1e_GB_Miss_c8);
	fprintf(statistics, "Ratio_dirty_T1_Hit_GB_c8 : %lf\n", Ratio_dirty_T1_Hit_GB_c8);
	fprintf(statistics, "Ratio_dirty_T1_Miss_GB_c8 : %lf\n\n", Ratio_dirty_T1_Miss_GB_c8);

	fprintf(statistics, "SSDvictim_WHDD_c8 : %d\n", in.SSDvictim_WHDD_c8);
	fprintf(statistics, "SSDvictim_c8 : %d\n", in.SSDvictim_c8);
	fprintf(statistics, "Ratio_dirty_T1_SSD_c8 : %lf\n", Ratio_dirty_T1_SSD_c8);
	fprintf(statistics, "-----------------------------------------------------------------\n");

	//case3
	if( in.T2victim_c3 != 0)
		Ratio_dirty_T2_c3 = (double)in.T2victim_WSSD_c3 / (double)in.T2victim_c3;
	else
		Ratio_dirty_T2_c3 = 0.0;

	if( in.SSDvictim_c3 != 0)
		Ratio_dirty_T2_SSD_c3 = (double)in.SSDvictim_WHDD_c3 / (double)in.SSDvictim_c3;
	else
		Ratio_dirty_T2_SSD_c3 = 0.0;
	fprintf(statistics, "Time_T2ReadMiss-----------------------------------------------------------------\n");
	fprintf(statistics, "T2victim_WSSD_c3 : %d\n", in.T2victim_WSSD_c3);
	fprintf(statistics, "T2victim_c3 : %d\n", in.T2victim_c3);
	fprintf(statistics, "Ratio_dirty_T2_c3 : %lf\n\n", Ratio_dirty_T2_c3);
	fprintf(statistics, "SSDvictim_WHDD_c3 : %d\n", in.SSDvictim_WHDD_c3);
	fprintf(statistics, "SSDvictim_c3 : %d\n", in.SSDvictim_c3);
	fprintf(statistics, "Ratio_dirty_T2_SSD_c3 : %lf\n", Ratio_dirty_T2_SSD_c3);
	fprintf(statistics, "-----------------------------------------------------------------\n");

	//case7:PageCache WriteMiss,  New Write in T2
	if( in.T2victim_c7 != 0)
		Ratio_dirty_T2_c7 = (double)in.T2victim_WSSD_c7 / (double)in.T2victim_c7;
	else
		Ratio_dirty_T2_c7 = 0.0;

	if( in.SSDvictim_c7 != 0)
		Ratio_dirty_T2_SSD_c7 = (double)in.SSDvictim_WHDD_c7 / (double)in.SSDvictim_c7;
	else
		Ratio_dirty_T2_SSD_c7 = 0.0;
	fprintf(statistics, "Time_T2WriteMiss-----------------------------------------------------------------\n");
	fprintf(statistics, "T2victim_WSSD_c7 : %d\n", in.T2victim_WSSD_c7);
	fprintf(statistics, "T2victim_c7 : %d\n", in.T2victim_c7);
	fprintf(statistics, "Ratio_dirty_T2_c7 : %lf\n\n", Ratio_dirty_T2_c7);
	fprintf(statistics, "SSDvictim_WHDD_c7 : %d\n", in.SSDvictim_WHDD_c7);
	fprintf(statistics, "SSDvictim_c7 : %d\n", in.SSDvictim_c7);
	fprintf(statistics, "Ratio_dirty_T2_SSD_c7 : %lf\n", Ratio_dirty_T2_SSD_c7);
	fprintf(statistics, "-----------------------------------------------------------------\n");
		

	tmp_i = 0;
	while( tmp_i < NUM_BLOCKS-1 )
	{
		
		//fprintf(statistics, "Avg_SSD_QD = %lf, Avg_HDD_QD = %lf\n", Avg_SSD_QD, Avg_HDD_QD);
		fprintf(statistics, "++++++++++++++++++++++++++++++++++++++++++++++++\n");
		
		//case4: PageCache ReadMiss, and SSD ReadMiss
		Time_T1ReadMiss = (double)mrb.C1rm[tmp_i] * ( (Time_HDDRead+Avg_HDD_QD) + (Ratio_upcoming_Hit_GB * (Ratio_dirty_T2_c4 * ( (Time_SSDWrite+Avg_SSD_QD) + Ratio_dirty_T2_SSD_c4 * ( (Time_SSDWrite+Avg_SSD_QD) + (Time_HDDWrite+Avg_HDD_QD) )))) + (  Ratio_upcoming_Miss_GB * ( Ratio_dirty_T1_c4 * (( Ratio_dirty_T1_Miss_GB_c4 * (Time_HDDWrite+Avg_HDD_QD) )) + Ratio_dirty_T1_Hit_GB_c4 * ( (Time_SSDWrite+Avg_SSD_QD) + Ratio_dirty_T1_SSD_c4* ( (Time_SSDWrite+Avg_SSD_QD) + (Time_HDDWrite+Avg_HDD_QD) )))));
		fprintf(statistics, "Time_T1ReadMiss : %lf\n", Time_T1ReadMiss);

		//case8: PageCache WriteMiss,  New Write in T1
		Time_T1WriteMiss = mrb.C1wm[tmp_i] * Ratio_dirty_T1_c8 * ( Ratio_dirty_T1_Hit_GB_c8*( (Time_SSDWrite+Avg_SSD_QD) + Ratio_dirty_T1_SSD_c8 * ( (Time_SSDWrite+Avg_SSD_QD) + (Time_HDDWrite+Avg_HDD_QD))) + Ratio_dirty_T1_Miss_GB_c8*((Time_HDDWrite+Avg_HDD_QD)) );
		fprintf(statistics, "Time_T1WriteMiss : %lf\n", Time_T1WriteMiss);

		//case3: PageCache ReadMiss, but SSD ReadHit
		Time_T2ReadMiss = mrb.C2rm[(NUM_BLOCKS -1) - tmp_i] * ( (Time_SSDRead+Avg_SSD_QD) + Ratio_dirty_T2_c3 * ( (Time_SSDWrite+Avg_SSD_QD) +  Ratio_dirty_T2_SSD_c3 * ( (Time_SSDWrite+Avg_SSD_QD) + (Time_HDDWrite+Avg_HDD_QD)) ));
		fprintf(statistics, "Time_T2ReadMiss : %lf\n", Time_T2ReadMiss);
		
		//case7:PageCache WriteMiss,  New Write in T2
		Time_T2WriteMiss = mrb.C2wm[(NUM_BLOCKS -1) - tmp_i] * ( Ratio_dirty_T2_c7 * ( (Time_SSDWrite+Avg_SSD_QD) + Ratio_dirty_T2_SSD_c7 * ( (Time_SSDWrite+Avg_SSD_QD) + (Time_HDDWrite+Avg_HDD_QD))) );
		fprintf(statistics, "Time_T2WriteMiss : %lf\n", Time_T2WriteMiss);

		CostTime[tmp_i] = Time_T1ReadMiss + Time_T1WriteMiss + Time_T2ReadMiss +Time_T2WriteMiss;
		fprintf(statistics, "CostTime[%d] : %lf\n",tmp_i, CostTime[tmp_i]);

		fprintf(statistics, "++++++++++++++++++++++++++++++++++++++++++++++++\n");
	
		tmp_i++;
	}
	//find the smallest CostTime
	double cost_min = CostTime[0];
	int size_i = 0;
	int i;
	for(i = 0; i < NUM_BLOCKS-1; i++)
	{
		if(CostTime[i] <= cost_min){

			cost_min = CostTime[i];
			size_i = i;

		}
	}
	fprintf(statistics, "cost_min: %lf\n", cost_min);
	fprintf(statistics, "size_i: %d\n", size_i);

	T1Size = (double)(size_i+1) / NUM_BLOCKS * PageCacheSize;
	T2Size = PageCacheSize - T1Size;

	fprintf(statistics, "dy.c#642 T1Size: %d\n", T1Size);
	fprintf(statistics, "dy.c#642 T2Size: %d\n\n\n\n", T2Size);
	
}
