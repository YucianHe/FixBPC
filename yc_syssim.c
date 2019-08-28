#include "yc_syssim.h"

void panic(const char *s)
{
	perror(s);
	exit(1);
}

void IntqBuffering(IntqBufReq *req) {
	if (IBRhead == NULL) {
		IBRhead = req;
		IntqBufSize = 1;
	}
	else {
		req->next = IBRhead;
		IBRhead->pre = req;
		IBRhead = req;
		IntqBufSize++;
	}
}
//x
void DeIntqBuffering() {
	IntqBufReq *tmp;
	while(IBRhead != NULL) {
		tmp = IBRhead;
		IBRhead = IBRhead->next;
		free(tmp);
	}
	IntqBufSize = 0;
}

/*
 * Schedule next callback at time t.
 * Note that there is only *one* outstanding callback at any given time.
 * The callback is for the earliest event.
 */
void syssim_schedule_callback(disksim_interface_callback_t fn, SysTime t, void *ctx)
{
	next_event = t;
	now = next_event;
	//printf("Now = %lf , next_event = %lf\n", now, next_event);
}
/*de-scehdule a callback.*/
void syssim_deschedule_callback(double t, void *ctx)
{
	next_event = -1;
}

/*針對一個Request做完後回饋*/
/*若為一連串的request, 則回傳的r將視為最後一個*/
void ssdsim_report_completion(SysTime t, struct disksim_request *r, void *ctx)
{
	//PrintSomething(">>>>>[ssdsim_report_completion] !");
	completed--;
	now = t;
	st.stime += (t - r->start);
	//Statistic
	st.servedIORequest++;
	//printf("[SSDSIM]systime = %lf, start = %lf, *responseTime = %lf, blkno = %lu\n", st.stime, r->start, t-r->start, r->blkno);
	//printf("Now = %lf\n", now);
	REQ *re;
	re = calloc(1, sizeof(REQ));
	//ATTENTION!! We ignore the information of each returned request except 'response time'. If you need that for users, you should do something!
	re->responseTime = t - r->start;
	if(sendRequestByMSQ(KEY_MSQ_DISKSIM_SSD, re, MSG_TYPE_DISKSIM_SSD_SERVED) == -1)
		PrintError(KEY_MSQ_DISKSIM_SSD, "A control request not sent to MSQ in sendRequestByMSQ() return:");
	free(re);
}

double total_SSD_Queue_Delay = 0.0;
double total_HDD_Queue_Delay = 0.0;

void hddsim_report_completion(SysTime t, struct disksim_request *r, void *ctx) 
{
	//PrintSomething(">>>>>[hddsim_report_completion] !");
	completed--;
	now = t;
	st.stime += (t-r->start);
	
	//Statistic
	st.servedIORequest++;
	//printf("[HDDSIM]systime = %lf, start = %lf, *responseTime = %lf, blkno = %lu\n", st.stime, r->start, t-r->start, r->blkno);
	//printf("Now = %lf\n", now);
	REQ *re;
	re = calloc(1, sizeof(REQ));
	//ATTENTION!! We ignore the information of each returned request except 'response time'. If you need that for users, you should do something!
	re -> responseTime = t - r->start;
	if(sendRequestByMSQ(KEY_MSQ_DISKSIM_HDD, re, MSG_TYPE_DISKSIM_HDD_SERVED) == -1)
		PrintError(KEY_MSQ_DISKSIM_HDD, "A control request not sent to MSQ in sendRequestByMSQ() return:");
	free(re);
}

void exec_SSDsim(char *name, const char *parm_file, const char *output_file) {
	printf("[SSDSIM INFO]Disksim name:%s (%d)\n[SSDSIM INFO]Parameter File:%s\n[SSDSIM INFO]Output File:%s\n", name, getpid(), parm_file, output_file);

	struct disksim_interface *disksim;
	struct stat buf;
	IntqBufReq *pendReq;

	st.stime = 0;
	st.pendIORequest = 0;
	st.servedIORequest = 0;

	if (stat(parm_file, &buf) < 0)
		panic(parm_file);

	disksim = disksim_interface_initialize(parm_file, 
					 output_file,
					 ssdsim_report_completion,
					 syssim_schedule_callback,
					 syssim_deschedule_callback,
					 0,
					 0,
					 0);
	PrintSomething(">>>>>[SSDSIM] Launch!");

	REQ *rp = NULL;
	rp = calloc(1, sizeof(REQ));
	for (;;)
	{
		if(recvRequestByMSQ(KEY_MSQ_DISKSIM_SSD, rp, MSG_TYPE_DISKSIM_SSD) == -1)
			PrintError(-1, "[SSDSIM]A request not received from MSQ in recvRequestByMSQ():");
		//else
			//PrintREQ(rp,"#123fromSSD");
		
		if (rp->reqFlag == MSG_REQUEST_CONTROL_FLAG_FINISH)
		{
			REQ *ctrl;
			ctrl = calloc(1, sizeof(REQ));
			ctrl->responseTime = st.stime;
			if(sendRequestByMSQ(KEY_MSQ_DISKSIM_SSD, ctrl, MSG_TYPE_DISKSIM_SSD_SERVED) == -1)
				PrintError(KEY_MSQ_DISKSIM_SSD, "A control request not sent to MSQ in sendRequestByMSQ() return:");
			
			disksim_interface_shutdown(disksim, now);
			printf(COLOR_YELLOW"[SSDSIM]System Time:%lf, Pending IO Requests:%lu, Served IO Requests:%lu\n"COLOR_RESET, st.stime, st.pendIORequest, st.servedIORequest);
			PrintSomething("<<<<<[SSDSIM] Shutdown!");
			exit(0);
		}
		else if(rp -> reqFlag == MSG_REQUEST_PERIOD_SSD_TIME)
		{
			fprintf(statistics, "rp -> reqFlag == MSG_REQUEST_PERIOD_SDD_TIME!\n");

			REQ *ctrl;
			ctrl = calloc(1, sizeof(REQ));
			ctrl->responseTime = st.stime;
			if(sendRequestByMSQ(KEY_MSQ_DISKSIM_SSD, ctrl, MSG_TYPE_DISKSIM_SSD_SERVED) == -1)
				PrintError(KEY_MSQ_DISKSIM_SSD, "A control request not sent to MSQ in sendRequestByMSQ() return:");
		}
		else {
			pendReq = calloc(1, sizeof(IntqBufReq));
			if (rp->reqFlag == 1)
				pendReq->req.flags = DISKSIM_READ;
			else if (rp->reqFlag == 0)
				pendReq->req.flags = DISKSIM_WRITE;

			pendReq->req.start = rp->arrivalTime;
			pendReq->req.devno = rp->devno;
			/*
			 *blkno: 單位為512Bytes。
			 *以HDD而言，大小任意；
			 *以SSD而言，4K對齊，則blkno須為8的整數倍
			 *blkno=>第?個sector (512Bytes)
			 *bytecount=>一共存取?個sector
			*/
			pendReq->req.blkno = rp->diskBlkno;
			pendReq->req.bytecount = rp->reqSize*DISKSIM_SECTOR;
			/*
			printf("pendReq->req.start=%lf\n", pendReq->req.start);
			printf("pendReq->req.flags=%u\n", pendReq->req.flags);
			printf("pendReq->req.devno=%u\n", pendReq->req.devno);
			printf("pendReq->req.blkno=%lu\n", pendReq->req.blkno);
			printf("pendReq->req.bytecount=%u\n", pendReq->req.bytecount);
			*/
			//
			//Record pending requests by buffering
			IntqBuffering(pendReq);
			//Statistic
			st.pendIORequest++;

			completed++;
			disksim_interface_request_arrive(disksim, pendReq->req.start, &pendReq->req);
			//disksim_interface_request_arrive(disksim, now, &pendReq->req);
			/* Process events until this I/O is completed */
			if (completed != 0) {
				while(next_event >= 0) {
					now = next_event;
					next_event = -1;
					disksim_interface_internal_event(disksim, now, 0);
				}
				if (completed != 0) {
					PrintError(completed, "[SSDSIM]internal error. Some events not completed:");
					exit(1);
				}
			}
			//Delete pending requests by de-buffering
			DeIntqBuffering();			
		}
	}
}

void exec_HDDsim(char *name, const char *parm_file, const char *output_file) {
	printf("[HDDSIM INFO]Disksim name:%s (%d)\n[HDDSIM INFO]Parameter File:%s\n[HDDSIM INFO]Output File:%s\n", name, getpid(), parm_file, output_file);

	struct disksim_interface *disksim;
	struct stat buf;
	IntqBufReq *pendReq;

	st.stime = 0;
	st.pendIORequest = 0;
	st.servedIORequest = 0;

	if (stat(parm_file, &buf) < 0)
		panic(parm_file);

	disksim = disksim_interface_initialize(parm_file, 
					 output_file,
					 hddsim_report_completion,
					 syssim_schedule_callback,
					 syssim_deschedule_callback,
					 0,
					 0,
					 0);
	PrintSomething(">>>>>[HDDSIM] Launch!");

	REQ *rp = NULL;
	rp = calloc(1, sizeof(REQ));
	for (;;) 
	{
		if(recvRequestByMSQ(KEY_MSQ_DISKSIM_HDD, rp, MSG_TYPE_DISKSIM_HDD) == -1)
			PrintError(-1, "[HDDSIM]A request not received from MSQ in recvRequestByMSQ():");
		
		if (rp->reqFlag == MSG_REQUEST_CONTROL_FLAG_FINISH) {
			REQ *ctrl;
			ctrl = calloc(1, sizeof(REQ));
			ctrl->responseTime = st.stime;
			if(sendRequestByMSQ(KEY_MSQ_DISKSIM_HDD, ctrl, MSG_TYPE_DISKSIM_HDD_SERVED) == -1)
				PrintError(KEY_MSQ_DISKSIM_HDD, "A control request not sent to MSQ in sendRequestByMSQ() return:");

			disksim_interface_shutdown(disksim, now);
			printf(COLOR_YELLOW"[HDDSIM]System Time:%lf, Pending IO Requests:%lu, Served IO Requests:%lu\n"COLOR_RESET, st.stime, st.pendIORequest, st.servedIORequest);
			PrintSomething("<<<<<[HDDSIM] Shutdown!");
			exit(0);
		}
		else if(rp -> reqFlag == MSG_REQUEST_PERIOD_HDD_TIME)
		{
			fprintf(statistics, "rp -> reqFlag == MSG_REQUEST_PERIOD_HDD_TIME!\n");

			REQ *ctrl;
			ctrl = calloc(1, sizeof(REQ));
			ctrl->responseTime = st.stime;
			if(sendRequestByMSQ(KEY_MSQ_DISKSIM_SSD, ctrl, MSG_TYPE_DISKSIM_SSD_SERVED) == -1)
				PrintError(KEY_MSQ_DISKSIM_SSD, "A control request not sent to MSQ in sendRequestByMSQ() return:");
		}
		else {
			pendReq = calloc(1, sizeof(IntqBufReq));
			if (rp->reqFlag == 1)
				pendReq->req.flags = DISKSIM_READ;
			else if (rp->reqFlag == 0)
				pendReq->req.flags = DISKSIM_WRITE;

			pendReq->req.start = rp->arrivalTime;
			pendReq->req.devno = rp->devno;
			/*
			 *blkno: 單位為512Bytes。
			 *以HDD而言，大小任意；
			 *以SSD而言，4K對齊，則blkno須為8的整數倍
			 *blkno=>第?個sector (512Bytes)
			 *bytecount=>一共存取?個sector
			*/
			pendReq->req.blkno = rp->diskBlkno;
			pendReq->req.bytecount = rp->reqSize*DISKSIM_SECTOR;
			/*
			printf("pendReq->req.start=%lf\n", pendReq->req.start);
			printf("pendReq->req.flags=%u\n", pendReq->req.flags);
			printf("pendReq->req.devno=%u\n", pendReq->req.devno);
			printf("pendReq->req.blkno=%lu\n", pendReq->req.blkno);
			printf("pendReq->req.bytecount=%u\n", pendReq->req.bytecount);
			*/
			//
			//Record pending requests by buffering
			IntqBuffering(pendReq);
			//Statistic
			st.pendIORequest++;

			completed++;
			disksim_interface_request_arrive(disksim, pendReq->req.start, &pendReq->req);/////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			/* Process events until this I/O is completed */
			if (completed != 0) {
				while(next_event >= 0) {
					now = next_event;
					next_event = -1;
					disksim_interface_internal_event(disksim, now, 0);
				}

				if (completed != 0) {
					PrintError(completed, "[HDDSIM]internal error. Some events not completed:");
					exit(1);
				}
			}
			//Delete pending requests by de-buffering
			DeIntqBuffering();			
		}
	}
}