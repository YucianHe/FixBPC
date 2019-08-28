#include "ycsim.h"

pid_t SSDsimProc, HDDsimProc;              //Sub-process id: SSD and HDD simulator
//FILE *trace,*statistics, *result;      	//Trace file; Stat file; Result file
char *par[6];                           		//Arguments in yusim execution
//double scheduleTime = 0;                	//Simulation time
//unsigned long period = 0;               	//The counter of time period

int T1Size = PageCacheSize*0.5;
int T2Size = PageCacheSize *0.5;//at least two space
int windowSize = PageCacheSize *0.5 / 4;

int T1Used = 0, T2Used = 0, G1Used = 0, G2Used = 0;

double Time_SSDRead = 0.0;
double Time_HDDRead = 0.0;
double Time_SSDWrite = 0.0;
double Time_HDDWrite = 0.0;

double TotalPeriod_Time_SSDRead = 0.0;
double TotalPeriod_Time_SSDWrite = 0.0;
double TotalPeriod_Time_HDDRead = 0.0;
double TotalPeriod_Time_HDDWrite = 0.0;

int counts_SSDRead = 0;
int counts_SSDWrite = 0;
int counts_HDDRead = 0;
int counts_HDDWrite = 0;

int total_counts_SSDRead = 0;
int total_counts_SSDWrite = 0;
int total_counts_HDDRead = 0;
int total_counts_HDDWrite = 0;

double The_Time = 0.0;
double t = 0.0;

double Ptime_SSD = 0.0;
double Ptime_HDD = 0.0;

double SSD_period_First_Time = 0.0;
double SSD_period_Last_Time = 0.0;
double HDD_period_First_Time = 0.0;
double HDD_period_Last_Time = 0.0;

int PCHit = 0;
int PCMiss = 0;
int SSD_R_Hit = 0, SSD_W_Hit = 0;
int SSD_R_Miss = 0, SSD_W_Miss = 0;

int c_LAZY = 0;

double avg_response_time = 0.0;
double total_response_time = 0.0;

/*DISKSIM INITIALIZATION*/
/**
 * [Disksim的初始化，利用兩個Process各自執行Disksim，作為SSDsim和HDDsim，
 *  接續MESSAGE QUEUE INITIALIZATION]
 */
void initDisksim() {
	pid_t procid;
	//Fork process to execute SSD simulator
	procid = fork();
	if (procid == 0) {
		SSDsimProc = getpid();
		//printf("SSDsimProc ID: %d\n", SSDsimProc);
		exec_SSDsim("SSDsim", par[1], par[2]);
		exit(0);
	}
	else if (procid < 0) {
		PrintError(-1, "SSDsim process fork() error");
		exit(1);
	}

	//Fork process to execute HDD simulator
	procid = fork();
	if (procid == 0) {
		HDDsimProc = getpid();
		//printf("HDDsimProc ID: %d\n", HDDsimProc);
		exec_HDDsim("HDDsim", par[3], par[4]);
		exit(0);
	}
	else if (procid < 0) {
		PrintError(-1, "HDDsim process fork() error");
		exit(1);
	}

	//After the initialization of simulators, initialize message queue
	initMSQ();
}

/*DISKSIM SHUTDOWN*/
/**
 * [Disksim的關閉，傳送Control message告知其Process進行Shutdown，並等待回傳結果message]
 */
double rmDisksim() {
	REQ *ctrl, *ctrl_rtn;
	ctrl = calloc(1, sizeof(REQ));
	ctrl_rtn = calloc(1, sizeof(REQ));      //Receive message after control message
	ctrl->reqFlag = MSG_REQUEST_CONTROL_FLAG_FINISH; //Assign a finish flag (ipc)
	
	//Send a control message to finish SSD simulator
	sendFinishControl(KEY_MSQ_DISKSIM_SSD, MSG_TYPE_DISKSIM_SSD);

	//Receive the last message from SSD simulator
	if(recvRequestByMSQ(KEY_MSQ_DISKSIM_SSD, ctrl_rtn, MSG_TYPE_DISKSIM_SSD_SERVED) == -1)
		PrintError(-1, "A served request not received from MSQ in recvRequestByMSQ():");
	//else
		//PrintREQ(ctrl_rtn ,"ycsim.c#63");
	double SSD_total_response_time = ctrl_rtn->responseTime;
	printf("[YCSIM]SSDsim response time = %lf\n", SSD_total_response_time);
	//fprintf(result, "[YCSIM]SSDsim response time = %lf\n", ctrl_rtn->responseTime);

	//Send a control message to finish HDD simulator
	sendFinishControl(KEY_MSQ_DISKSIM_HDD, MSG_TYPE_DISKSIM_HDD);

	//Receive the last message from HDD simulator
	if(recvRequestByMSQ(KEY_MSQ_DISKSIM_HDD, ctrl_rtn, MSG_TYPE_DISKSIM_HDD_SERVED) == -1)
		PrintError(-1, "A served request not received from MSQ in recvRequestByMSQ():");
	//else
		//PrintREQ(ctrl_rtn ,"ycsim.c#75");
	double HDD_total_response_time = ctrl_rtn->responseTime;
	printf("[YCSIM]HDDsim response time = %lf\n", HDD_total_response_time);
	//fprintf(result, "[YCSIM]HDDsim response time = %lf\n", ctrl_rtn->responseTime);
	rmMSQ();

	total_response_time = SSD_total_response_time + HDD_total_response_time;
	return total_response_time;
	//After that, remove message queues
	
}

/*MESSAGE QUEUE INITIALIZATION*/
/**
 * [Message queue初始化，使用系統定義的Key值、Type和IPC function]
 */
void initMSQ() {
	//Create message queue for SSD simulator
	if(createMessageQueue(KEY_MSQ_DISKSIM_SSD, IPC_CREAT) == -1)
		PrintError(-1, " MSQ create error in createMessageQueue():");
	
	//Create message queue for HDD simulator
	if(createMessageQueue(KEY_MSQ_DISKSIM_HDD, IPC_CREAT) == -1)
		PrintError(-1, " MSQ create error in createMessageQueue():");
}

/*MESSAGE QUEUE REMOVE*/
/**
 * [Message queue刪除，使用系統定義的Key值和IPC function]
 */
void rmMSQ() {
	struct msqid_ds ds;
	//Remove message queue for SSD simulator
	if(removeMessageQueue(KEY_MSQ_DISKSIM_SSD, &ds) == -1)
		PrintError(KEY_MSQ_DISKSIM_SSD, "Not remove message queue:(key)");
	//Remove message queue for HDD simulator
	if(removeMessageQueue(KEY_MSQ_DISKSIM_HDD, &ds) == -1)
		PrintError(KEY_MSQ_DISKSIM_HDD, "Not remove message queue:(key)");
}

double Update_SSDTimes(unsigned long page_no, int flag){

	if(counts_SSDRead == 0 && counts_SSDWrite == 0){
		SSD_period_First_Time = The_Time;
		//fprintf(statistics, "SSD_period_First_Time = %lf\n", SSD_period_First_Time);
	}
	else
	{
		SSD_period_Last_Time = The_Time;
		//fprintf(statistics, "SSD_period_Last_Time = %lf\n", SSD_period_Last_Time);
	}
	
	if(flag == 1){
		//Read miss : send a request to SSD to get the READ SSD time
		counts_SSDRead++;
		total_counts_SSDRead++;
		fprintf(result, "total_counts_SSDRead : %d (SSD_R_Hit = %d, SSD_R_Miss = %d)\n", total_counts_SSDRead, SSD_R_Hit, SSD_R_Miss);
		
		Time_SSDRead = sendRequest2SSD_to_getServiceTime(page_no, flag);
		fprintf(result, "Time_SSDRead = %lf, counts_SSDRead = %d\n", Time_SSDRead, counts_SSDRead);	
		TotalPeriod_Time_SSDRead += Time_SSDRead;
		//fprintf(result, "TotalPeriod_Time_SSDRead = %lf\n", TotalPeriod_Time_SSDRead);

		return Time_SSDRead;				
	}
	else
	{
		//Read miss : send a request to SSD to get the READ SSD time
		counts_SSDWrite++;		
		total_counts_SSDWrite++;
		fprintf(result, "total_counts_SSDWrite : %d (SSD_W_Hit = %d, SSD_W_Miss = %d)\n", total_counts_SSDWrite, SSD_W_Hit, SSD_W_Miss);
		
		if(flag == 0)				
			Time_SSDWrite = sendRequest2SSD_to_getServiceTime(page_no, flag);
		else
		{
			Time_SSDWrite = sendRequest2SSD_to_getServiceTime(page_no, 0);
			fprintf(result, "Time_SSDWrite = %lf\n", Time_SSDWrite);	
		}
		
		fprintf(result, "Time_SSDWrite = %lf\n", Time_SSDWrite);	
		TotalPeriod_Time_SSDWrite += Time_SSDWrite;
		//fprintf(result, "TotalPeriod_Time_SSDWrite = %lf\n", TotalPeriod_Time_SSDWrite);

		return Time_SSDWrite;				
	}
}

double Update_HDDTimes(unsigned long page_no, int flag)
{

	if(counts_HDDRead == 0 && counts_HDDWrite == 0){
		HDD_period_First_Time = The_Time;		
		//fprintf(statistics, "HDD_period_First_Time = %lf\n", HDD_period_First_Time);
	}
	else
	{
		HDD_period_Last_Time = The_Time;		
		//fprintf(statistics, "HDD_period_Last_Time = %lf\n", HDD_period_Last_Time);
	}
	
	if(flag == 1){
		//Read miss : send a request to SSD to get the READ SSD time
		counts_HDDRead++;
		total_counts_HDDRead++;
		
		Time_HDDRead = sendRequest2HDD_to_getServiceTime(page_no, flag);
		//fprintf(result, "Time_HDDRead = %lf\n", Time_HDDRead);	
		TotalPeriod_Time_HDDRead += Time_HDDRead;
		//fprintf(result, "TotalPeriod_Time_HDDRead = %lf\n", TotalPeriod_Time_HDDRead);

		return Time_HDDRead;			
	}
	else
	{
		//Read miss : send a request to SSD to get the READ SSD time
		counts_HDDWrite++;
		total_counts_HDDWrite++;
		
		Time_HDDWrite = sendRequest2HDD_to_getServiceTime(page_no, flag);
		//fprintf(result, "Time_HDDWrite = %lf\n", Time_HDDWrite);	
		TotalPeriod_Time_HDDWrite += Time_HDDWrite;
		//fprintf(result, "TotalPeriod_Time_HDDWrite = %lf\n", TotalPeriod_Time_HDDWrite);	

		return Time_HDDWrite;
	}
}



int main(int argc, char *argv[]) {
	//Check arguments
	if (argc != 8) {
		fprintf(stderr, "usage: %s <Trace file> <param file for SSD> <output file for SSD> <param file for HDD> <output file for HDD><output file for result><output file for statistics>\n", argv[0]);
		exit(1);
	}

	//Record arguments by variables
	par[0] = argv[1]; //trace
	par[1] = argv[2]; //SSD prav
	par[2] = argv[3]; //SSD output
	par[3] = argv[4]; //HDD prav
	par[4] = argv[5]; //HDD output
	par[5] = argv[6]; //r.txt
	par[6] = argv[7]; //s.txt

	//Open trace file
	trace = fopen(par[0], "r");
	if (!trace)
		PrintError(-1, "[YCSIM]trace file open error");

	//Open result file
	result = fopen(par[5], "w");

	//Open result file
	statistics = fopen(par[6], "w");

	//Initialize Disksim(SSD and HDD simulators)
	initDisksim();

	fprintf(result, "T2_CFLRU!!!!\n\n");
	
	//PrintMissRatio_in_BLOCKS(MRB, "yc.c#init");
	fprintf(result, "PageCacheSize : %d\n", PageCacheSize);
	fprintf(result, "T1Size : %d\n", T1Size);
	fprintf(result, "T2Size : %d\n", T2Size);
	fprintf(result, "SSDSize : %d\n", SSDSize);
	fprintf(result, "PREIOD_SERVED_PAGES : %d\n", PREIOD_SERVED_PAGES);
	fprintf(result, "NUM_BLOCKS : %d\n", NUM_BLOCKS);

	MRB = Clean_MBR(MRB);
	//PrintMissRatio_in_BLOCKS(MRB, "main.c#init");

	//From trace file, insert user requests into user queues
	REQ *input;
	int check_pages = 0;
	input = calloc(1, sizeof(REQ));
	printf("[YCSIM]Trace loading...............\n");
	while(!feof(trace)) {
		//get every request form trace
		fscanf(trace, "%lf%u%lu%u%u", &input->arrivalTime, &input->devno, &input->diskBlkno, &input->reqSize, &input->reqFlag);
		
		//DEBUG:
		//PrintREQ(input, "Trace");
		//fprintf(statistics, "input->arrivalTime = %lf\n", input->arrivalTime);	
		
		int page_count = input->reqSize / 8;//r->reqSize/SSD_PAGE2SECTOR;
		info.total_pages += page_count;			
		
		fprintf(result, "\nThis request page_count:%d\n", page_count);	
		fprintf(result, "total pages:%d\n", info.total_pages);
		
		

		int i = 0;
		for (i = 0; i < page_count; i++)
		{
			The_Time = input->arrivalTime;
			//fprintf(statistics, "%d.The_Time:%lf\n", check_pages, The_Time);

			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
			//////////////////////////////////////////////for every period to adjust T1 T2 size////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
			if(check_pages >= PREIOD_SERVED_PAGES)
			{
				printf("\ncheck_pages = %d\n", check_pages);
				printf("PREIOD_SERVED_PAGES = %d\n", PREIOD_SERVED_PAGES);

				fprintf(result, "\n[#190]PREIOD_SERVED_PAGES =%d\n", PREIOD_SERVED_PAGES);

				fprintf(result, "\n[#192]check_pages =%d\n", check_pages);
				fprintf(result, "[#193]Need to adjust T1 T2 Size!\n");

				fprintf(statistics, "SSD_period_Last_Time = %lf\n", SSD_period_Last_Time);
				fprintf(statistics, "SSD_period_First_Time = %lf\n", SSD_period_First_Time);
				
				fprintf(statistics, "HDD_period_Last_Time = %lf\n", HDD_period_Last_Time);
				fprintf(statistics, "HDD_period_First_Time = %lf\n", HDD_period_First_Time);
				
				Ptime_SSD = SSD_period_Last_Time - SSD_period_First_Time;
				fprintf(statistics, "Ptime_SSD = %lf\n", Ptime_SSD); 
				Ptime_HDD = HDD_period_Last_Time - HDD_period_First_Time; 
				fprintf(statistics, "Ptime_HDD = %lf\n", Ptime_HDD);

				printf("test\n");
				//writeCheckFile(&statistics, info);
				keeped_MRB = keep_Hit_ratio_in_BLOCKS(MRB);	
				printf("test1\n");					
				predict_next_info = Predict_Record(info);
				printf("test2\n");
			
				//double Ptime_SSD = max_Time_SSD - start_Time;
				//double Ptime_HDD = max_Time_HDD - start_Time;

				AdjustT1T2Size(Ptime_SSD, Ptime_HDD, keeped_MRB, predict_next_info);
				printf("test3\n");

				info = Clean_Record(info);	
				Clean_Times();					
				printf("test4\n");
				info.period++;
				check_pages = 0;				
			}
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
			check_pages++;

			int page_no = (input -> diskBlkno + i*8) / SSD_PAGE2SECTOR;
			int flag = input -> reqFlag;

			printf("\ntotal page = %d\n", info.total_pages);					

			int isPCHit = CheckPageCache(page_no, flag);
			if(isPCHit == 11)//1.T1(HDD)Read Hit
			{
				fprintf(result, "hit in Page Cache(T1)!\n");

				info.PageCache_Hit++;
				PCHit++; 

				if(flag == 1)
					info.T1_Read_Hit_Count++;
				else
					info.T1_Write_Hit_Count++;
			}
			else if(isPCHit == 12)//2.T2(HDD)Read Hit
			{
				fprintf(result, "hit in Page Cache(T2)!\n");

				info.PageCache_Hit++;
				PCHit++;
				printf("PCHit = %d\n", PCHit);

				if(flag == 1)
					info.T2_Read_Hit_Count++;
				else
					info.T2_Write_Hit_Count++;			
			}
			else//isPCHit == 10
			{
				info.PageCache_Miss++;
				PCMiss++;

				fprintf(result, "PageCache_Miss = %d\n", info.PageCache_Miss);

				if(flag == 1)//PC Read miss
				{
					printf("read miss in Page Cache!\n");
					fprintf(result, "Read miss in Page Cache!\n");

					//SSD READ for check if in SSD
					info.PCReadMiss_to_SSDRead++;										

					if(CheckSSDTable(page_no, flag) == 1)//3. Read Hit in SSD
					{
						SSD_R_Hit++;
						fprintf(result, "SSD_R_Hit = %d\n", SSD_R_Hit);

						The_Time += t;
						t = Update_SSDTimes(page_no, flag);//SSD Read

						printf("Read miss in Page Cache but hit in SSD!\n");
						fprintf(result, "Read miss in Page Cache but hit in SSD!\n");

						info.PCReadMiss_to_SSDRead_Hit++;
						
						T2ReadHit[PageCacheSize]++;
						//fprintf(statistics, "T2ReadHit[%d] = %d!\n", PageCacheSize, T2ReadHit[PageCacheSize]);

						if(insertT2(page_no, flag) == 1)//T2 is full
						{
							fprintf(result, "T2 is full!\n");
							
							NODE  *e;
							e = malloc(sizeof(NODE));
							e = FindingT2victim();
							info.T2victim_c3++;

							if(e -> flag == 0)
							{	
								info.T2victim_WSSD_c3++;

								fprintf(result, "T2 eviction is dirty!\n");	

								if(CheckSSDTable(e -> page_no, e -> flag) == 0)
								{
									//dirty victim miss in SSD
									SSD_W_Miss++;

									The_Time += t;
									t = Update_SSDTimes(e -> page_no, e ->flag);//SSD Write								
																		
									if(insertSSD(e -> page_no, e ->flag) == 1)
									{
										printf("SSD is full!\n");
										NODE  *e_SSD;
										e_SSD = malloc(sizeof(NODE));
										e_SSD = FindingSSDvictim();
										info.SSDvictim_c3++;
										
										if(e_SSD -> flag == 0)//SSD last is dirty
										{
											printf("SSDlast is dirty!\n");
											info.SSDvictim_WHDD_c3++;

											The_Time += t;
											t = Update_SSDTimes(e_SSD -> page_no, 1);//SSD Read

											The_Time += t;
											t = Update_HDDTimes(e_SSD -> page_no, e_SSD -> flag);
											//fprintf(statistics, "#409 t = %lf\n", t);										
										}						
										//evciton SSDlast
										Evict_SSDvictim(e_SSD);																			
									}																			
								}
								else
								{
									SSD_W_Hit++;
									//fprintf(statistics, "SSD_W_Hit = %d\n", SSD_W_Hit);
									The_Time += t;
									t = Update_SSDTimes(e -> page_no, e ->flag);//SSD Write									
								}
							}
							insertG2(e->page_no, e->flag);							
							Evict_T2victim(e);							
						}
						else
						{
							if(T1Used + T2Used > PageCacheSize)
							{
								fprintf(result, "T1Used + T2Used > PageCacheSize (%d)\n", T1Used+T2Used);
								NODE  *tmp_e;
								tmp_e = malloc(sizeof(NODE));
								tmp_e = FindingT1victim();

								insertG1(tmp_e->page_no, tmp_e->flag);	
								Evict_T1victim(tmp_e);
							}

							if( T2Used + G2Used > PageCacheSize )	
							{
								fprintf(result, "T2Used + G2Used > PageCacheSize(%d)\n", T2Used+G2Used);
								Evict_G2last();
							}
						}						
					}
					else//4. Read Miss in SSD
					{
						printf("4.read miss in Page Cache and miss in SSD too!\n");
						fprintf(result, "read miss in Page Cache and miss in SSD too!\n");
						
						info.PCReadMiss_to_SSDRead_Miss++;	
						SSD_R_Miss++;
						fprintf(result, "SSD_R_Miss = %d\n", SSD_R_Miss);

						T1ReadHit[PageCacheSize]++;
						printf("T1ReadHit[%d] = %d!\n", PageCacheSize, T1ReadHit[PageCacheSize]);			
						
						//HDD READ for miss in SSD
						The_Time += t;
						t = Update_HDDTimes(page_no, flag);
						//fprintf(statistics, "#446 t = %lf\n", t);							

						//check the page is in GB or not
						if(checkGHOSTbuffer(page_no, flag) == 1)//Hit in GB, has accessed twice
						{
							printf("Missing page is hit in GB!\n");
							info.upingpage_GB_Hit++;
							
							fprintf(result, "Missing page in GB is inserted to T2!\n");

							//Double caching
							The_Time += t;
							t = Update_SSDTimes(page_no, 0);
							if(insertSSD(page_no, flag) == 1)
							{
								printf("SSD is full!\n");
								NODE  *e_SSD;
								e_SSD = malloc(sizeof(NODE));
								e_SSD = FindingSSDvictim();
																
								if(e_SSD -> flag == 0)//SSD last is dirty
								{
									fprintf(result, "SSDlast is dirty!\n");
									
									The_Time += t;
									t = Update_SSDTimes(e_SSD -> page_no, 1);	

									The_Time += t;
									t = Update_HDDTimes(e_SSD -> page_no, e_SSD -> flag);
									//fprintf(statistics, "#489 t = %lf\n", t);											
								}
								//evciton SSDlast
								Evict_SSDvictim(e_SSD);
							}										

							if(insertT2(page_no, flag) == 1)//T2 is full
							{
								printf("T2 is full!\n");							
								fprintf(result, "T2 is full!\n");  
								NODE  *e;
								e = malloc(sizeof(NODE));
								e = FindingT2victim();
								info.T2victim_c4++;

								if(e -> flag == 0)
								{
									fprintf(result, "Page Cache T2 evict dirty victim!\n");
									
									if(CheckSSDTable(e -> page_no, e -> flag) == 0)
									{
										SSD_W_Miss++;

										fprintf(result, "T2's eviction miss in SSD, insert to SSD !\n");

										info.T2victim_WSSD_c4++;

										The_Time += t;
										t = Update_SSDTimes(e -> page_no, e -> flag);
										if(insertSSD(e -> page_no, e ->flag) == 1)
										{
											printf("SSD is full!\n");
											NODE  *e_SSD;
											e_SSD = malloc(sizeof(NODE));
											e_SSD = FindingSSDvictim();
											info.T1e_SSDvictim_c4++;
											
											if(e_SSD -> flag == 0)//SSD last is dirty
											{
												printf("SSDlast is dirty!\n");
												info.T1e_SSDvictim_WHDD_c4++;

												The_Time += t;
												t = Update_SSDTimes(e_SSD -> page_no, 1);

												The_Time += t;
												t = Update_HDDTimes(e_SSD -> page_no, e_SSD -> flag);
												//fprintf(statistics, "#409 t = %lf\n", t);										
											}						
											//evciton SSDlast
											Evict_SSDvictim(e_SSD);																		
										}																														
									}
									else
									{
										SSD_W_Hit++;
										//fprintf(statistics, "SSD_W_Hit = %d\n", SSD_W_Hit);
										info.T2victim_WSSD_c4++;

										The_Time += t;
										t = Update_SSDTimes(e -> page_no, e -> flag);
									}
								}
								insertG2(e->page_no, e->flag);																
								Evict_T2victim(e);
							}
							else
							{
								if(T1Used + T2Used > PageCacheSize)
								{
									fprintf(result, "T1Used + T2Used > PageCacheSize (%d)\n", T1Used+T2Used);
									NODE  *tmp_e;
									tmp_e = malloc(sizeof(NODE));
									tmp_e = FindingT1victim();

									insertG1(tmp_e->page_no, tmp_e->flag);	
									Evict_T1victim(tmp_e);
								}

								if( T2Used + G2Used > PageCacheSize )
								{
									fprintf(result, "T2Used + G2Used > PageCacheSize(%d)\n", T2Used+G2Used);
									Evict_G2last();
								}
							}
							
						}
						else//miss in GB, first access in the period
						{
							printf("Missing page is miss in GB!\n");
							info.upingpage_GB_Miss++;

							fprintf(result, "Missing page in GB is inserted to T1!\n");
							if(insertT1(page_no, flag) == 1)//T1 is full
							{
								NODE  *e;
								e = malloc(sizeof(NODE));
								e = FindingT1victim();
								info.T1victim_c4++;

								if(e -> flag == 0)
								{
									fprintf(result, "T1 evict dirty victim!\n");
									info.T1victim_WSSD_c4++;

									if(checkGHOSTbuffer(e -> page_no, e ->flag) == 1)
									{
										fprintf(result, "Dirty victim page is hit in GB!\n");
										//dirty eviction hit in GB, so write the eviction in SSD
										info.T1e_GB_Hit_c4++;

										The_Time += t;
										t = Update_SSDTimes(e -> page_no, e -> flag);
										//fprintf(statistics, "#544 t = %lf\n", t);

										fprintf(result, "Victim page miss in SSD, insert to SSD!\n");
										if(insertSSD(e -> page_no, e ->flag) == 1)
										{
											printf("SSD is full!\n");
											NODE  *e_SSD;
											e_SSD = malloc(sizeof(NODE));
											e_SSD = FindingSSDvictim();
											info.T1e_SSDvictim_c4++;												

											fprintf(result, "SSD is full!\n");
											if(e_SSD -> flag == 0)//SSD last is dirty
											{
												printf("SSDlast is dirty!\n");
												fprintf(result, "SSD ecivt dirty victim!\n");
												info.T1e_SSDvictim_WHDD_c4++;

												The_Time += t;
												t = Update_SSDTimes(e_SSD -> page_no, 1);

												The_Time += t;
												t = Update_HDDTimes(e_SSD -> page_no, e_SSD -> flag);
												//fprintf(statistics, "#565 t = %lf\n", t);
											}
											//evciton SSDlast
											Evict_SSDvictim(e_SSD);																			
										}
									}
									else
									{
										fprintf(result, "Dirty victim page is miss in GB!\n");
										info.T1e_GB_Miss_c4++;										

										The_Time += t;
										t = Update_HDDTimes(e -> page_no, e -> flag);
										//fprintf(statistics, "#578 t = %lf\n", t);
										
										fprintf(result, "Victim page insert to GB!\n");
										insertGHOSTbuffer(e -> page_no, e -> flag);
									}
								}
								insertG1(e->page_no, e->flag);
								Evict_T1victim(e);						
							}
							else
							{
								if(T1Used + T2Used > PageCacheSize)
								{
									fprintf(result, "T1Used + T2Used > PageCacheSize (%d)\n", T1Used+T2Used);
									NODE  *tmp_e;
									tmp_e = malloc(sizeof(NODE));
									tmp_e = FindingT2victim();

									printf("test\n");
									insertG2(tmp_e->page_no, tmp_e->flag);
									Evict_T2victim(tmp_e);
								}

								if( T1Used + G1Used > PageCacheSize )	{
									fprintf(result, "T1Used + G1Used > PageCacheSize(%d)\n", T1Used+G1Used);
									Evict_G1last();
								}
							}

							printf("Missing page is inserted to GB!\n");
							fprintf(result, "Missing page is inserted to GB!\n");
							insertGHOSTbuffer(page_no, flag);							
						}
					}				
	
				}
				else//PC Write miss
				{
					printf("Write miss in Page Cache!\n");
					fprintf(result, "Write miss in Page Cache!\n");
					info.PCWrite_Miss++;

					NODE *check;
					check = malloc(sizeof(NODE));
					check = searchSSD(page_no);
					
					if(check == NULL)
					{
						printf("insert into T1!\n");
						fprintf(result, "insert into T1!\n");
						T1WriteHit[PageCacheSize]++;	
						//fprintf(statistics, "T1WriteHit[%d] = %d!\n", PageCacheSize, T1WriteHit[PageCacheSize]);
				
						if(insertT1(page_no, flag) == 1)
						{
							printf("T1 is full!\n");

							NODE  *e;
							e = malloc(sizeof(NODE));
							e = FindingT1victim();
							info.T1victim_c8++;

							if(e -> flag == 0)
							{
								fprintf(result, "T1 evict dirty victim!\n");
								info.T1victim_WSSD_c8++;								

								if(checkGHOSTbuffer(e -> page_no, e ->flag) == 1)
								{
									fprintf(result, "Victim page is hit in GB!\n");
									//dirty eviction hit in GB, so write the eviction in SSD
									info.T1e_GB_Hit_c8++;
									
									fprintf(result, "Victim page miss in SSD, write to SSD!\n");
									The_Time += t;
									t = Update_SSDTimes(e -> page_no, e -> flag);	
									if(insertSSD(e -> page_no, e ->flag) == 1)
									{
										printf("SSD is full!\n");
										NODE  *e_SSD;
										e_SSD = malloc(sizeof(NODE));
										e_SSD = FindingSSDvictim();										
										info.SSDvictim_c8++;											

										if(e_SSD -> flag == 0)//SSD last is dirty
										{
											printf("SSDlast is dirty!\n");
											fprintf(result, "SSD ecivt dirty victim!\n");
											info.SSDvictim_WHDD_c8++;

											The_Time += t;
											t = Update_SSDTimes(e_SSD -> page_no, 1);

											The_Time += t;
											t = Update_HDDTimes(e_SSD -> page_no, e_SSD -> flag);
										}											
										Evict_SSDvictim(e_SSD);																			
									}										
								}
								else
								{
									fprintf(result, "Victim page is miss in GB!\n");
									info.T1e_GB_Miss_c8++;	

									The_Time += t;
									t = Update_HDDTimes(e -> page_no, e -> flag);
									
									fprintf(result, "Victim page insert to GB!\n");
									insertGHOSTbuffer(e -> page_no, e -> flag);
								}										
								
							}
							insertG1(e->page_no, e->flag);
							Evict_T1victim(e);	
						}
						else
						{
							if(T1Used + T2Used > PageCacheSize)
							{
								fprintf(result, "T1Used + T2Used > PageCacheSize (%d)\n", T1Used+T2Used);
								NODE  *tmp_e;
								tmp_e = malloc(sizeof(NODE));
								tmp_e = FindingT2victim();

								insertG2(tmp_e->page_no, tmp_e->flag);
								Evict_T2victim(tmp_e);
							}

							if( T1Used + G1Used > PageCacheSize )	{
								fprintf(result, "T1Used + G1Used > PageCacheSize(%d)\n", T1Used+G1Used);
								Evict_G1last();
							}
						}
					}
					else//7.
					{
						printf("insert into T2!\n");
						fprintf(result, "insert into T2!\n");
						T2WriteHit[PageCacheSize]++;	
						//fprintf(statistics, "T2WriteHit[%d] = %d!\n", PageCacheSize, T2WriteHit[PageCacheSize]);
				
						if(insertT2(page_no, flag) == 1)//T2 is full
						{							
							NODE  *e;
							e = malloc(sizeof(NODE));
							e = FindingT2victim();
							info.T2victim_c7++;
							
							if(e -> flag == 0)
							{
								info.T2victim_WSSD_c7++;

								fprintf(result, "check T2's eviction is in SSD or not!\n");
								if(CheckSSDTable(e -> page_no, e ->flag) == 0)//miss in SSD
								{
									SSD_W_Miss++;

									The_Time += t;
									t = Update_SSDTimes(e -> page_no, e -> flag);									

									fprintf(result, "T2's eviction miss in SSD, so need to insert to SSD !\n");							
									if(insertSSD(e -> page_no, e ->flag) == 1)
									{
										printf("SSD is full!\n");
										NODE  *e_SSD;
										e_SSD = malloc(sizeof(NODE));
										e_SSD = FindingSSDvictim();
										info.SSDvictim_c7++;
										
										if(e_SSD -> flag == 0)//SSD last is dirty
										{
											printf("SSDlast is dirty!\n");
											info.SSDvictim_WHDD_c7++;

											The_Time += t;
											t = Update_SSDTimes(e_SSD -> page_no, 1);

											The_Time += t;
											t = Update_HDDTimes(e_SSD -> page_no, e_SSD -> flag);
										}
										//evciton SSDlast
										Evict_SSDvictim(e_SSD);																			
									}
								}
								else{
									SSD_W_Hit++;
									//fprintf(statistics, "SSD_W_Hit = %d\n", SSD_W_Hit);
									The_Time += t;
									t = Update_SSDTimes(e -> page_no, e -> flag);														
								}
							}
							insertG2(e->page_no, e->flag);
							Evict_T2victim(e);				
						}
						else
						{
							if(T1Used + T2Used > PageCacheSize)
							{
								fprintf(result, "T1Used + T2Used > PageCacheSize (%d)\n", T1Used+T2Used);
								NODE  *tmp_e;
								tmp_e = malloc(sizeof(NODE));
								tmp_e = FindingT1victim();

								insertG1(tmp_e->page_no, tmp_e->flag);
								Evict_T1victim(tmp_e);
							}

							if( T2Used + G2Used > PageCacheSize )	{
								fprintf(result, "T2Used + G2Used > PageCacheSize(%d)\n", T2Used+G2Used);
								Evict_G2last();
							}
						}												
					}
				}

				t = 0.0;


			}//end of is_PC_Hit == 10

			printf("deal the page over.\n");
		}//end of page_counts

	}//end of while read trace	
	
	/*
	displayT1Table();
	displayG1Table();
	displayT2Table();
	displayG2Table();
	displayGHOSTbuffer();
	displaySSDTable();
	*/
	
	printf("[Trace Reading Finish----CFLRU]\n");
	
	printf("PageCacheSize : %d\n", PageCacheSize);
	printf("SSDSize : %d\n\n", SSDSize);

	printf("PCHit = %d\n", PCHit);
	printf("PCMiss = %d\n", PCMiss);

	double PC_Hit_Ratio = (double)PCHit / (PCHit + PCMiss);
	printf("PC_Hit_Ratio = %lf\n", PC_Hit_Ratio);

	printf("\ntotal_counts_SSDRead = %d\n", total_counts_SSDRead);
	printf("SSD_R_Hit = %d\n", SSD_R_Hit);
	printf("SSD_R_Miss = %d\n", SSD_R_Miss);
	printf("total_counts_SSDWrite = %d\n", total_counts_SSDWrite);
	printf("SSD_W_Hit = %d\n", SSD_W_Hit);	
	printf("SSD_W_Miss = %d\n", SSD_W_Miss);	

	double SSD_Hit_Ratio = (double)(SSD_R_Hit + SSD_W_Hit) / (SSD_R_Hit + SSD_W_Hit + SSD_R_Miss + SSD_W_Miss);
	printf("SSD_Hit_Ratio = %lf\n\n", SSD_Hit_Ratio);

	printf("\ntotal_counts_HDDRead = %d\n", total_counts_HDDRead);
	printf("total_counts_HDDWrite = %d\n", total_counts_HDDWrite);

	int total_NODE_counts = get_NODE_counts();
	printf("\ntotal_NODE_counts = %d\n", total_NODE_counts);	
	printf("c_LAZY = %d\n", c_LAZY);

	//Remove Disksim(SSD and HDD simulators)
	total_response_time = rmDisksim();
	printf("total_response_time = %lf\n", total_response_time);
	printf("info.total_pages = %d\n", info.total_pages);
	avg_response_time = total_response_time / (double)info.total_pages;
	printf("avg_response_time = %lf\n", avg_response_time);

	//Total requests output
	//printf("[YCSIM]Receive requests:%lu\n", getTotalReqs());
	
	//Statistics output in Prize caching (prize)
	//pcStatistics();

	//User statistics output
	//printUSERSTAT(scheduleTime);

	//Result file
	//fprintf(result, "[YUSIM]Receive requests:%lu\n", getTotalReqs());
	//CACHEWriteResultFile(&result);
	//pcWriteResultFile(&result);
	//writeResultFile(&result, scheduleTime);

	// Waiting for SSDsim and HDDsim process
	wait(NULL);
	wait(NULL);
	//OR
	//printf("Main Process waits for: %d\n", wait(NULL));
	//printf("Main Process waits for: %d\n", wait(NULL));

	//Release files items
	fclose(trace);
	fclose(statistics);
	fclose(result);

	exit(0);

}
