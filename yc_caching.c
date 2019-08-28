/*LRU+CFLRU(PageCache) and LARC(SSD)*/
#include <stdlib.h>
#include <stdio.h> 

#include "yc_caching.h"

int T1ReadHit[PageCacheSize+1] = {0} , T2ReadHit[PageCacheSize+1] = {0} ;
int T1WriteHit[PageCacheSize+1] = {0} , T2WriteHit[PageCacheSize+1]  = {0} ;
int T2CleanArray[PageCacheSize] = {0};

void PUSH_Toatl_Node_QUE(unsigned long page_no)
{
	printf("[PUSH_Toatl_Node_QUE]page_no = %lu\n", page_no);
	
	if(searchTNQ(page_no) == NULL)
	{
		fprintf(result, "Creat a New Node in TNQUE\n");
		NODE *node;
		node = malloc(sizeof(NODE));
		node -> page_no = page_no;

		if(TNQ_head == NULL)
		{
			TNQ_head = node;
			TNQ_Size = 1;
		}
		else
		{
			node -> next = TNQ_head;
			TNQ_head -> pre = node;
			TNQ_head = node;
			TNQ_Size++;
		}
	}
	else
		printf("NODE has existed!!!!!!!!!!!!!!!!\n");
	//printf("PUSH_Toatl_Node_QUE done.\n");	
}

NODE *searchTNQ(unsigned long page_no)
{
	//printf("[searchTNQ]want to search in TNQ : %lu\n",  page_no);
	
	NODE *search;
	search = malloc(sizeof(NODE));

	search = TNQ_head;	

	while(search != NULL)
	{
		if(search -> page_no == page_no)
		{
			//printf("TNQ search done.\n");
			return search;
		}
		else
		{
			search = search -> next;
		}
			
	}
	//printf("searchTNQ = NULL\n");
	return NULL;
}

NODE *searchT1queue(unsigned long page_no, int flag)//
{
	//printf("[searchPageCache]want to search in T1 : %lu\n",  page_no);
	
	NODE *search;
	search = malloc(sizeof(NODE));

	search = T1head;
	int p = 0;
	while(search != NULL)
	{
		//printf(result, "searchT1 : %lu, c = %d\n", search -> page_no, p);				
		if(search -> page_no == page_no)  
		{
			if(flag == 1)//Read hit
			{
				T1ReadHit[p]++;				
				//fprintf(result, "T1ReadHit[%d] = %d\n",  p, T1ReadHit[p]);
			}
			else//Write hit
			{
				T1WriteHit[p]++;				
				//fprintf(result, "T2WriteHit[%d] = %d\n",  p, T1WriteHit[p]);
			}
			return search;		
		}
		else
		{
			search = search -> T1_next;
			p++;
		}
	}

	return NULL;
	
}

NODE *searchT2queue(unsigned long page_no, int flag)//
{
	//printf("[searchPageCache]want to search in T2 : %lu\n",  page_no);
	NODE *search;
	search = malloc(sizeof(NODE));

	search = T2head;
	int p = 0;
	while(search != NULL)
	{
		//fprintf(result, "searchT2 : %lu, c = %d\n", search -> page_no, p);
		if(search -> page_no == page_no)  
		{
			if(flag == 1)//Read hit
			{
				T2ReadHit[p]++;			
				//fprintf(result, "T2ReadHit[%d] = %d\n",  p, T2ReadHit[p]);
			}
			else//Write hit
			{
				T2WriteHit[p]++;				
				//fprintf(result, "T2WriteHit[%d] = %d\n",  p, T2WriteHit[p]);
			}
			return search;		
		}
		else
		{
			search = search -> T2_next;
			p++;
		}
	}
	return NULL;
}

NODE *searchSSD(unsigned long page_no)
{
	//printf("[searchPageCache]want to search %lu\n",  page_no);
	NODE *search;
	search = malloc(sizeof(NODE));

	int c = 0;
	search = SSDhead;
	while(search != NULL)
	{
		//fprintf(result, "search : %lu, c = %d\n", search->page_no, c);
		if(search -> page_no == page_no)  
			return search;		
		else
		{
			search = search -> SSD_next;
			c++;
		}
	}
	return NULL;
}

NODE *searchGHOSTbuffer(unsigned long page_no)
{
	//printf("[searchPageCache]want to search %lu\n",  page_no);
	NODE *search;
	search = malloc(sizeof(NODE));

	search = Ghead;
	while(search != NULL)
	{
		//fprintf(result, "search : %lu\n", search->page_no);
		if(search -> page_no == page_no)  
			return search;		
		else
		{
			search = search -> ghost_next;
		}
	}
	return NULL;
}

GHOST *searchG1queue(unsigned long page_no, int flag)
{
	GHOST *search;
	search = malloc(sizeof(GHOST));

	search = G1head;
	int p = 0;
	while(search != NULL)
	{
		//fprintf(result, "searchG2 : %lu, c = %d\n", search -> page_no, p);
		if(search -> page_no == page_no)  
		{
			if(flag == 1)//Read hit
			{
				T1ReadHit[T1Size + p]++;			
				fprintf(result, "T1ReadHit[%d] = %d\n",  p, T1ReadHit[T1Size + p]);
			}
			else//Write hit
			{
				T1WriteHit[T1Size + p]++;				
				fprintf(result, "T2WriteHit[%d] = %d\n",  p, T1WriteHit[T1Size + p]);
			}
			return search;		
		}
		else
		{
			search = search -> G1_next;
			p++;
		}
	}
	return NULL;

}

GHOST *searchG2queue(unsigned long page_no, int flag)
{
	GHOST *search;
	search = malloc(sizeof(GHOST));

	search = G2head;
	int p = 0;
	while(search != NULL)
	{
		//fprintf(result, "searchG2 : %lu, c = %d\n", search -> page_no, p);
		if(search -> page_no == page_no)  
		{
			if(flag == 1)//Read hit
			{
				T2ReadHit[T2Size + p]++;			
				fprintf(result, "T2ReadHit[%d] = %d\n",  p, T2ReadHit[T2Size + p]);
			}
			else//Write hit
			{
				T2WriteHit[T2Size + p]++;				
				fprintf(result, "T2WriteHit[%d] = %d\n",  p, T2WriteHit[T2Size + p]);
			}
			return search;		
		}
		else
		{
			search = search -> G2_next;
			p++;
		}
	}
	return NULL;	
}

int CheckSSDTable(unsigned long page_no, int flag)
{
	printf("[CheckSSDTable]search: %lu\n",  page_no);
	
	NODE *search;
	search = malloc(sizeof(NODE));
	search = searchSSD(page_no);

	/*
	if(search  == NULL)
		printf("[CheckSSDTable]search result: NULL\n");
	else
		printf("[CheckSSDTable]search result: %lu\n", search -> page_no);
	*/

	if(search == NULL)//the page miss in SSD
	{
		GHOSTsize1 = 0.9 * SSDSize;
		GHOSTsize2 = GHOSTbufferSize + ( SSDSize / GHOSTbufferSize );
		GHOSTbufferSize = min(GHOSTsize1, GHOSTsize2);

		/*
		printf(COLOR_MAGENTA"[B is not in Q]Adjust the GHOSTbufferSize\n"COLOR_RESET);
		printf(COLOR_MAGENTA"GHOSTbufferSize = %d\n", GHOSTbufferSize);
		printf(COLOR_RESET);
		*/
		
		return 0;	
	}
	else//the page hit in SSD
	{

		printf("[caching.c#304]Hit in SSD Table\n");

		//Move to MRU of SSD
		//printf(COLOR_YELLOW"Move the page to MRU of SSD\n"COLOR_RESET);
		if(search->SSD_pre != NULL)//not the first node
		{
			if(search->SSD_next == NULL)//hit the last node
			{
				//printf("Hit in SSD last!\n");
				SSDlast = SSDlast -> SSD_pre;
				SSDlast->SSD_next = NULL;		

				fprintf(result, "[After set]SSDlast -> page_no = %lu\n", SSDlast ->page_no);										
			}
			else//hit in the middle node
			{
				//printf("Hit in SSD middle!\n");						
				search->SSD_pre->SSD_next = search->SSD_next;
				search->SSD_next->SSD_pre = search->SSD_pre;				
			}
			SSDhead->SSD_pre = search;
			search->SSD_next = SSDhead;						
			search->SSD_pre=NULL;
			SSDhead = SSDhead->SSD_pre;
		}

		//printf(COLOR_MAGENTA"[B is in Q]Adjust the Size of GHOSTbuffer!!!\n"COLOR_RESET);

		GHOSTsize1 = 0.1 * SSDSize;
		GHOSTsize2 = GHOSTbufferSize - ( SSDSize / ( SSDSize - GHOSTbufferSize ));
		
		int Old_GBSize = GHOSTbufferSize;
		GHOSTbufferSize = max(GHOSTsize1, GHOSTsize2);
		int New_GBSize = GHOSTbufferSize;

		if(New_GBSize < Old_GBSize)
		{
			printf("New_GBSize(%d) < Old_GBSize(%d)\n", New_GBSize, Old_GBSize);
			Adjust_Glast(New_GBSize);
		}		
		//printf(COLOR_MAGENTA"GHOSTbufferSize = %d\n", GHOSTbufferSize);
		//printf(COLOR_RESET);
		
		return 1;
	}
}

void Adjust_Glast(int New_GBSize)
{	
	//fprintf(result, "Adjust_Glast\n");
	//fprintf(result, "=======================================\n");
	//fprintf(result, "New_GBSize = %d\n", New_GBSize);
	//fprintf(result, "GHOSTbufferSize = %d\n", GHOSTbufferSize);
	//fprintf(result, "GHOSTbufferUsed = %d\n", GHOSTbufferUsed);

	if(GHOSTbufferUsed > New_GBSize)
	{
		fprintf(result, "GHOSTbufferUsed > New_GBSize\n");

		NODE *tmp;
		tmp = malloc(sizeof(NODE));

		tmp = Ghead;
		int p = 1;
		while(tmp != NULL)
		{
			if(p == New_GBSize)  
			{
				fprintf(result, "p(%d) == New_GBSize(%d)\n", p, New_GBSize);
				
				fprintf(result, "tmp = %lu, p = %d\n", tmp->page_no, p);
				
				Glast = tmp;
				Glast -> ghost_pre = tmp -> ghost_pre;
				Glast -> ghost_next = NULL;			

				//fprintf(result, "Glast-> pre = %lu, p = %d\n", Glast->ghost_pre->page_no, p-1);
				//fprintf(result, "[After set]Glast = %lu, p = %d\n", Glast->page_no, p);

				if(Glast -> ghost_next == NULL)
					fprintf(result, "Glast -> ghost_next == NULL\n");
				else
					printf("caching.c#176 error!\n");

				GHOSTbufferUsed = New_GBSize;
				fprintf(result, "[After set]GHOSTbufferUsed = %d\n", GHOSTbufferUsed);

				//displayGHOSTbuffer();

				//free(tmp);

				break;	
			}
			else
			{
				tmp = tmp -> ghost_next;
				p++;
			}
		}
	}	
	//fprintf(result, "=======================================\n");	
}

int checkGHOSTbuffer(unsigned long page_no, int flag)
{
	/*
	printf(COLOR_MAGENTA"GHOSTbufferSize = %d\n", GHOSTbufferSize);
	printf(COLOR_RESET);
	*/
	//fprintf(result, "checkGHOSTbuffer\n");
	//check if exist in GHOSTbuffer or not
	NODE *search;
	search = malloc(sizeof(NODE));
	search = searchGHOSTbuffer(page_no);

	if( search == NULL )//[B is not in Qr]metadata put in MRU of ghost buffer
	{	
		//fprintf(result, "Miss in GHOSTbuffer\n");
		return  0;
	}
	else//[B is in Qr]find in ghost buffer, means the page is be access twice//1.get rid of form ghost buffer//2.put in SSD
	{
		
		/*printf(COLOR_MAGENTA"===========================================\n"COLOR_RESET);
		printf(COLOR_MAGENTA"hit in GHOST buffer, so remove it from GHOST buffer!\n"COLOR_RESET);
		printf(COLOR_MAGENTA"===========================================\n"COLOR_RESET);
		*/

		//evcit the node in ghost buffer queue
		if(search->ghost_pre != NULL)//not the first node
		{
			if(search->ghost_next == NULL)//hit the last node
			{
				Glast = Glast -> ghost_pre;	
				Glast -> ghost_next = NULL;					
			}
			else//hit in the middle node
			{
				search->ghost_pre->ghost_next = search->ghost_next;
				search->ghost_next->ghost_pre = search->ghost_pre;
			}
		}
		else//hit in first node of ghost buffer
		{
			Ghead = search -> ghost_next;
			Ghead -> ghost_pre = NULL;
		}
		//fprintf(result, "Hit in GHOSTbuffer\n");
		GHOSTbufferUsed--;
		return 1;	
	}

}
int CheckPageCache(unsigned long page_no, int flag)
{
	//printf("[CheckPageCache]page_no = %lu, flag = %d\n", page_no, flag);

	NODE *searchT1, *searchT2;
	searchT1 = malloc(sizeof(NODE));
	searchT2 = malloc(sizeof(NODE));

	printf(COLOR_BLUE"A NEW PAGE COMES!!! page_no: %lu, flag = %d\n"COLOR_RESET, page_no, flag);
	
	fprintf(result, "\n");
	fprintf(result, "++++++++++++++++++++++++++++++\n");
	fprintf(result, "A NEW PAGE COMES!!! page_no: %lu, flag = %d\n", page_no, flag);
		
	PUSH_Toatl_Node_QUE(page_no);
	//displayTNQ();	
	
	searchT1 = searchT1queue(page_no, flag);
	searchT2 = searchT2queue(page_no, flag);	

	if( searchT1 != NULL && searchT2 != NULL )
	{
		printf(COLOR_RED"ERROR in searchT1 & searchT2\n"COLOR_RESET);
	}
	else if( searchT1 != NULL && searchT2 == NULL )
	{
		//Hit in T1
		printf("Hit in T1!\n");
		//if request page is write, change flag to dirty
		switch(flag)
		{
			case CLEAN_PAGE:				
				break;
			case DIRTY_PAGE:				
				searchT1 -> flag = flag;
				break;
			default:
				printf(COLOR_RED"Caching Error\n"COLOR_RESET);
		}

		//Move the page to T1's MRU end
		if(searchT1 -> T1_pre != NULL)//not the first node
		{
			
			if(searchT1 -> T1_next == NULL)//hit the last node
			{
				printf(COLOR_RED"Hit in T1 last!\n"COLOR_RESET);
				fprintf(result, "Hit in T1 last!\n");
				T1last = searchT1 -> T1_pre;
				T1last -> T1_next = NULL;	
			}
			else//hit in the middle node
			{
				printf(COLOR_RED"Hit in T1 middle!\n"COLOR_RESET);
				fprintf(result, "Hit in T1 middle!\n");
				searchT1 -> T1_pre -> T1_next = searchT1 -> T1_next;
				searchT1 -> T1_next -> T1_pre = searchT1 -> T1_pre;	
			}
			T1head -> T1_pre = searchT1;
			searchT1 -> T1_next = T1head;						
			searchT1 -> T1_pre = NULL;
			T1head = T1head ->T1_pre;

			//printf("[AdjustPageCache]T1head = %lu\n", T1head -> page_no);
			//printf("[AdjustPageCache]T1last = %lu\n", T1last -> page_no);	
		}
		//Hit in T1
		return 11;
	}
	else if( searchT1 == NULL && searchT2 != NULL )
	{
		//Hit in T2
		printf("Hit in T2 !\n");
		fprintf(result, "Hit in T2!\n");
		//printf("[AdjustPageCache]searchT2 = \n", searchT2 -> page_no);	

		//if request page is write, change flag to dirty
		switch(flag)
		{
			case CLEAN_PAGE:
				break;
			case DIRTY_PAGE:
				searchT2 -> flag = flag;
				break;
			default:
				printf(COLOR_RED"Caching Error\n"COLOR_RESET);
		}

		//Move the page to T2's MRU end
		if(searchT2 -> T2_pre != NULL)//not the first node
		{
			if(searchT2 -> T2_next == NULL)//hit the last node
			{
				printf("Hit in T2 last!\n");
				fprintf(result, "Hit in T2 last!\n");
				T2last = searchT2 -> T2_pre;
				T2last -> T2_next = NULL;					
			}
			else//hit in the middle node
			{
				printf("Hit in T2 middle!\n");
				fprintf(result, "Hit in T2 middle!\n");
				searchT2 -> T2_pre -> T2_next = searchT2 -> T2_next;
				searchT2 -> T2_next -> T2_pre = searchT2 -> T2_pre;	
			}			
			T2head -> T2_pre = searchT2;
			searchT2 -> T2_next = T2head;						
			searchT2 -> T2_pre = NULL;
			T2head = T2head ->T2_pre;			

			//printf("[AdjustPageCache]T2head = %lu\n", T2head -> page_no);
			//printf("[AdjustPageCache]T2last = %lu\n", T2last -> page_no);		
		}
		//Hit in T2
		return 12;
	}
	else// searchT1 == NULL && searchT2 == NULL 
	{
	
		//PageCache miss
		printf("PageCache miss!\n");
		return 10;		
	}
	
}		

int insertT1(unsigned long page_no, int flag)
{
	//The page can't find in T1 queue.
	printf("[insertT1]page_no = %lu\n", page_no);
	fprintf(result, "[insertT1]page_no = %lu\n", page_no);
		
	NODE *node;
	node = malloc(sizeof(NODE));
	node = searchTNQ(page_no);	

	if(T1head == NULL)
	{
		T1head = malloc(sizeof(NODE));

		T1head = node;
		T1head -> flag = flag;

		T1head -> T1_next = NULL;
		T1head -> T1_pre = NULL;
		
		T1last = malloc(sizeof(NODE));
		T1last = T1head;
		
		//printf("T1last = %lu\n", T1last -> page_no);

		T1Used++;
		fprintf(result, "T1Used = %d\n", T1Used);
	}	
	else
	{
		//put this node in MRU of SSD list
		node -> flag = flag;

		T1head -> T1_pre = node;
		node -> T1_next = T1head;
		node -> T1_pre = NULL;
		T1head = T1head -> T1_pre;

		T1Used++;	
		fprintf(result, "T1Used = %d\n", T1Used);

		fprintf(result, "After insertT1 : T1head = %lu\n", T1head -> page_no);
		fprintf(result, "After insertT1 : T1last = %lu\n", T1last -> page_no);

		//printf("[insertT1]T1head = %lu\n", T1head -> page_no);	
		//printf("[insertT1]T1last = %lu\n", T1last -> page_no);	
		if( T1Used > T1Size )
		{
			//evict the last node in queue
			printf("[T1 is full](T1Size = %d, T1Used = %d)\n", T1Size, T1Used);	
			//printf("[insertT1]T1last flag= %lu\n", T1last -> flag);		

			return 1;			
		}			
	}
	
	
}

int insertT2(unsigned long page_no, int flag)
{
	fprintf(result, "[insertT2]page_no = %lu\n", page_no);
	
	NODE *node;
	node = malloc(sizeof(NODE));
	node = searchTNQ(page_no);	

	if(T2head == NULL)
	{
		T2head = malloc(sizeof(NODE));
		T2head = node;		
		if(flag == 2){
			T2head -> flag = 1;
			T2head -> LAZY = 1;
		}
		else
		{
			T2head -> flag = flag;
			T2head -> LAZY = 0;
		}
		//printf("[insertT2]T2head = %lu\n", T2head -> page_no);
		
		T2head -> T2_next = NULL;
		T2head -> T2_pre = NULL;
		
		T2last = malloc(sizeof(NODE));
		T2last = T2head;
		//printf("[insertT2]T2last = %lu\n", T2last -> page_no);
		T2Used++;
		fprintf(result, "T2Used = %d\n", T2Used);
	}
	else
	{		
		//put this node in MRU of T2 list
		if(flag == 2){
			node -> flag = 1;
			node -> LAZY = 1;
		}
		else
		{
			node -> flag = flag;
			node -> LAZY = 0;		
		}

		printf("[insertT2]the node = %lu, flag = %d, LAZY = %d\n", node -> page_no, node -> flag, node -> LAZY);

		T2head -> T2_pre = node;
		node -> T2_next = T2head;
		node -> T2_pre = NULL;
		T2head = T2head -> T2_pre;

		T2Used++;
		fprintf(result, "T2Used = %d\n", T2Used);
		printf("T2Used = %d\n", T2Used);

		if( T2Used > T2Size )//pick 
		{
			//evict the last node in queue and insert to G2		
			fprintf(result, "[T2 is full](T2Size = %d)\n", T2Size);
			printf("[T2 is full]\n");
			return 1;
		}

		fprintf(result, "[insertT2]T2head = %lu\n", T2head -> page_no);
		fprintf(result, "[insertT2]T2last = %lu\n", T2last -> page_no);				
	}	
		
}

int insertGHOSTbuffer(unsigned long page_no, int flag)
{
	NODE *node;
	node = malloc(sizeof(NODE));
	node = searchTNQ(page_no);

	if(Ghead == NULL)
	{
		//printf("Ghead is NULL \n");
		Ghead = node;
		//printf("Ghead = %lu\n", Ghead -> page_no);
		Ghead -> ghost_next = NULL;
		Ghead -> ghost_pre = NULL;
		
		Glast = malloc(sizeof(NODE));
		Glast = Ghead;
		//printf("Glast = %lu\n", Glast -> page_no);
	}
	else
	{
		//check if out of GHOSTbuffer Size
		if( GHOSTbufferUsed >= GHOSTbufferSize )
		{
			//evict the last node in queue
			Glast = Glast -> ghost_pre;
			Glast -> ghost_next = NULL;
			//printf("Glast = %lu\n", Glast -> page_no);

			GHOSTbufferUsed--;
		}

		//put this node in MRU of GHOSTbuffer list
		Ghead -> ghost_pre = node;
		node->ghost_next = Ghead;
		node->ghost_pre = NULL;
		Ghead = Ghead->ghost_pre;							
	}
	GHOSTbufferUsed++;
	//printf("GHOSTbufferUsed = %d\n", GHOSTbufferUsed);
	
	
}
int insertSSD(unsigned long page_no, int flag)
{
	printf("[insertSSD]page_no = %lu\n", page_no);
	printf("[insertSSD]SSDUsed = %d\n", SSDUsed);
	//Only insert the page in SSD MRU end

	NODE *node;
	node = malloc(sizeof(NODE));
	node = searchTNQ(page_no);

	if(SSDhead == NULL)
	{
		//printf(COLOR_YELLOW"SSDhead is NULL \n"COLOR_RESET);
		SSDhead = node;
		//printf(COLOR_YELLOW"After set the SSDhead = %lu\n", SSDhead -> page_no);
		//printf(COLOR_RESET);
		
		SSDhead -> SSD_next = NULL;
		SSDhead -> SSD_pre = NULL;
		
		SSDlast = malloc(sizeof(NODE));
		SSDlast = SSDhead;
		//printf("SSDlast = %lu\n", SSDlast -> page_no);

		SSDUsed++;
		printf("SSDUsed = %d\n", SSDUsed);

		printf("SSDhead -> page_no = %lu\n", SSDhead ->page_no);
		printf("SSDlast -> page_no = %lu\n", SSDlast ->page_no);

	}
	else
	{

		//put this node in MRU of SSD list
		//printf(COLOR_YELLOW"the node = %lu\n", tmp -> page_no);
		//printf(COLOR_RESET);
	
		SSDhead->SSD_pre = node;
		node->SSD_next = SSDhead;
		node->SSD_pre = NULL;
		SSDhead = SSDhead->SSD_pre;	

		SSDUsed++;
		printf("SSDUsed = %d\n", SSDUsed);

		if( SSDUsed > SSDSize )
			return 1;	

		printf("SSDhead -> page_no = %lu\n", SSDhead ->page_no);
		printf("SSDlast -> page_no = %lu\n", SSDlast ->page_no);						
	}
}

void insertG1(unsigned long page_no, int flag)
{
	printf("[insertG1]page_no = %lu, flag = %d\n", page_no, flag);
	if(G1head == NULL)
	{
		G1head = malloc(sizeof(GHOST));

		G1head -> page_no = page_no;
		G1head -> flag = flag;

		//fprintf(result, "[insertG1]G1head = %lu\n", G1head -> page_no);

		//printf(COLOR_YELLOW"After set the T1head = %lu\n", T1head -> page_no);
		//printf(COLOR_RESET);

		G1head -> G1_next = NULL;
		G1head -> G1_pre = NULL;
		
		G1last = malloc(sizeof(GHOST));
		G1last = G1head;
		
		//fprintf(result, "[insertG1]G1last = %lu\n", G1last -> page_no);
		//printf("G1head -> page_no = %lu\n", G1head ->page_no);
		//printf("G1last -> page_no = %lu\n", G1last ->page_no);

	}	
	else
	{
		if( G1Used >= PageCacheSize - T1Size )
		{
			//evict the last node in queue
			//printf("[G1 is full]\n");
			printf("[G1 is full]\n");

			//printf("G1Used = %d\n", G1Used);
			//printf("PageCacheSize = %d\n", PageCacheSize);
			//printf("T1Size = %d\n", T1Size);
			//printf("PageCacheSize - T1Size = %d\n", PageCacheSize - T1Size);

			//printf("G1head -> page_no = %lu\n", G1head ->page_no);
			//printf("G1last ->pre-> page_no = %lu\n", G1last -> G1_pre->page_no);
			//printf("G1last -> page_no = %lu\n", G1last ->page_no);
			/*
			G1last = G1last -> G1_pre;
			G1last -> G1_next = NULL;
			//printf("[G1 is full]After set : G1last = %lu\n", G1last -> page_no);
			
			G1Used--;
			fprintf(result, "[765]G1Used = %d\n", G1Used);
			*/
			Evict_G1last();
		}

		//put this node in MRU of SSD list
		GHOST  *tmp;
		tmp = malloc(sizeof(GHOST));

		tmp -> page_no = page_no;
		tmp -> flag = flag;

		//printf("[insertG1]tmp node = %lu\n", tmp -> page_no);
		//printf(COLOR_RESET);

		G1head -> G1_pre = tmp;
		tmp -> G1_next = G1head;
		tmp -> G1_pre = NULL;
		G1head = G1head -> G1_pre;

		printf("[insertG1]G1head = %lu\n", G1head -> page_no);	
		printf("[insertG1]G1last = %lu\n", G1last -> page_no);				
	}
	G1Used++;	
	fprintf(result, "[After insertG1]G1Used = %d\n", G1Used);

}

void insertG2(unsigned long page_no, int flag)
{
	//printf("[get in insertG2]\n");	
	
	if(G2head == NULL)
	{
		//printf("[insertG2]G2head = NULL\n");

		G2head = malloc(sizeof(GHOST));
		
		G2head -> page_no = page_no;		
		G2head -> flag = flag;

		//printf("[insertG2]G2head = %lu\n", G2head -> page_no);

		//printf(COLOR_YELLOW"After set the T1head = %lu\n"COLOR_RESET, T1head -> page_no);
		//printf(COLOR_RESET);

		G2head -> G2_next = NULL;
		G2head -> G2_pre = NULL;
		
		G2last = malloc(sizeof(GHOST));
		G2last = G2head;

		G2Used++;
		
		//printf("[insertG2]G2last = %lu\n", G2last -> page_no);

	}	
	else
	{
		//printf("[insertG2]G2head = %lu\n", G2head -> page_no);

		if( G2Used >= PageCacheSize -T2Size )
		{
			//evict the last node in queue
			printf("[G2 is full]\n");			
			
			Evict_G2last();
		}

		//put this node in MRU of SSD list
		GHOST  *tmp;
		tmp = malloc(sizeof(GHOST));

		tmp -> page_no = page_no;
		tmp -> flag = flag;

		//printf("[insertG2]tmp node = %lu\n"COLOR_RESET, tmp -> page_no);

		G2head -> G2_pre = tmp;
		tmp -> G2_next = G2head;
		tmp -> G2_pre = NULL;
		G2head = G2head -> G2_pre;

		//fprintf(result, "[insertG2]G2head = %lu\n", G2head -> page_no);	
		//fprintf(result, "[insertG2]G2last = %lu\n", G2last -> page_no);				
	}
	G2Used++;	
	fprintf(result, "G2Used = %d\n", G2Used);

	//displayG2Table();

}
NODE *FindingT1victim()
{
	return T1last;
}

void *Evict_T1victim(NODE *eviction)
{
	//fprintf(result, "eviction = %lu\n", eviction -> page_no);
	//insertG1(eviction -> page_no, eviction -> flag);

	//displayT1Table();

	//printf("eviction = %d\n", eviction->page_no);	;
	//printf("eviction->T1_pre = %d\n", eviction->T1_pre->page_no);	
		
	T1last = T1last -> T1_pre;
	T1last -> T1_next = NULL;
	//printf("[T1 is full]After set : T1last = %lu\n", T1last -> page_no);

	///printf("[After set]T1last = %d\n", T1last->page_no);
	
	//fprintf(result, "After eviction : T1head = %lu\n", T1head -> page_no);
	//fprintf(result, "After eviction : T1last = %lu\n", T1last -> page_no);

	T1Used--;
	//fprintf(result, "T1 victim is be evicted.\n");
	fprintf(result, "T1Used = %d\n", T1Used);	
	
}
NODE *FindingT2victim()
{
	//displayT2Table();

	win_count = 0;	
	windowSize = T2Size / 4;

	NODE  *evict;
	evict = malloc(sizeof(NODE));
	evict = T2last;

	//printf("windowSize = %d (T2Size = %d)\n", windowSize, T2Size);
	while(win_count < windowSize)
	{
		
		//printf("[FindingT2victim]%d. T2 tmp evict = %lu, T2 evict flag = %lu\n", win_count, evict -> page_no, evict -> flag);
		if(evict -> flag == 1 || evict -> flag == 2)//1:clean page
		{
			return evict;
			//printf("[test CFLRU]evict(T2last) and flag = %lu, %d\n", evict -> page_no, evict -> flag);
		}
		else
		{			
			evict = evict -> T2_pre;
			//printf("[test CFLRU(flag == 0)]evict = %lu, flag = %d, c = %d\n", evict -> page_no, evict -> flag, win_count);
		}
		win_count++;
	}
	evict = T2last;
	
	printf("[FindingT2victim]T2 evict = %lu, T2 evict flag = %lu\n", evict -> page_no, evict -> flag);

	return evict;

}

void  *Evict_T2victim(NODE *eviction)
{
	printf("T2 eviction = %lu, T2 eviction flag = %lu\n", eviction -> page_no, eviction -> flag);

	if(eviction -> T2_next == NULL)//last node is clean to evcit.
	{
		//fprintf(result, "eviction -> T2_next = NULL\n");

		//insertG2(eviction -> page_no, eviction -> flag);

		//printf("[test CFLRU]evict -> T2_next = NULL, evict last\n");
		T2last = T2last -> T2_pre;
		T2last -> T2_next = NULL;		

		//fprintf(result, "After set : T2head = %lu\n", T2head -> page_no);
		//fprintf(result, "After set : T2last = %lu\n", T2last -> page_no);	
	}
	else//middle node is clean to evcit.
	{
		//insertG2(eviction -> page_no, eviction -> flag);

		//fprintf(result, "[931 insertG2 Done]\n");	

		//printf("[test CFLRU]evict middle\n");
		eviction -> T2_pre -> T2_next = eviction -> T2_next;
		eviction -> T2_next -> T2_pre = eviction -> T2_pre;	

	}

	T2Used--;
	//fprintf(result, "T2 victim is be evicted.\n");
	//fprintf(result, "T2Used = %d\n", T2Used);

	printf("[Evict_T2victim Done]\n");
}

NODE *FindingSSDvictim()
{
	return SSDlast;
}
void  *Evict_SSDvictim(NODE *eviction)
{
	SSDlast = SSDlast -> SSD_pre;
	SSDlast -> SSD_next = NULL;
	//printf("SSDlast = %lu\n", SSDlast -> page_no);

	//printf("SSD victim is be evicted.\n");
	//fprintf(result, "SSD victim is be evicted.\n");
	SSDUsed--;
	//printf("SSDUsed = %d\n", SSDUsed);	
	fprintf(result, "SSDUsed = %d\n", SSDUsed);
}

void  *Evict_G1last()
{
	G1last = G1last -> G1_pre;
	G1last -> G1_next = NULL;

	//fprintf(result, "G1last is be evicted.\n");
	G1Used--;
	fprintf(result, "[After Evict_G1last]G1Used = %d\n", G1Used);
}

void  *Evict_G2last()
{
	G2last = G2last -> G2_pre;
	G2last -> G2_next = NULL;

	//fprintf(result, "G2last is be evicted.\n");
	G2Used--;
	fprintf(result, "G2Used = %d\n", G2Used);
}

void KeepT2CleanArray()
{
	/*
	printf("[KeepT2CleanArray]PageCacheSize = %d\n", PageCacheSize);

	int pos = 0;
	if (T2head != NULL)
	{
		NODE *tmpT2;
		

		for(tmpT2 = T2head; tmpT2->T2_next != NULL ; tmpT2 = tmpT2->T2_next)
		{			
			//printf("%d.tmpT2 -> flag = %d\n", pos, tmpT2->flag);							
			if(tmpT2 -> flag == 1)
			{
				T2CleanArray[pos] = 1;
				//printf("T2CleanArray[%d] = %d\n", pos, T2CleanArray[pos]);
			}
			fprintf(statistics, "T2CleanArray[%d] = %d\n", pos, T2CleanArray[pos]);			
			pos++;	
		}
		if(tmpT2 -> flag == 1)
		{
			T2CleanArray[pos] = 1;
			fprintf(statistics, "T2CleanArray[%d] = %d\n", pos, T2CleanArray[pos]);
		}
	}
	fprintf(statistics, "keep T2 clean done.(T2Size = %d)\n", T2Size);	

	printf("start keep G2 record\n");
	
	pos = 0;

	if(G2head == NULL)
		printf("G2head is NULL\n");

	if(G2head != NULL)
	{
		GHOST *tmpG2;
					
		for(tmpG2 = G2head; tmpG2->G2_next != NULL ; tmpG2 = tmpG2->G2_next)
		{	
			//printf("%d.tmpG2 -> flag = %d\n", pos, tmpG2->flag);						
			if(tmpG2 -> flag == 1){				
				T2CleanArray[T2Size+pos-1] = 1;
				//printf("[Ghost]T2CleanArray[%d] = %d\n", pos, T2CleanArray[pos]);
			}
			fprintf(statistics, "[Ghost]T2CleanArray[%d] = %d\n", T2Size+pos, T2CleanArray[T2Size+pos]);
		
			pos++;	
		}
		if(tmpG2 -> flag == 1)
		{
			T2CleanArray[T2Size+pos] = 1;
			fprintf(statistics, "[Ghost]T2CleanArray[%d] = %d\n", T2Size+pos, T2CleanArray[T2Size+pos]);
		}
	}
	fprintf(statistics, "keep G2 clean done.\n");
	*/
}
void displayTNQ()
{
	int PC_c = 0;
	unsigned long diskBlkno;
	if (TNQ_head != NULL)
	{
		NODE *tmp;
		printf(COLOR_BLUE"<TNQ Table>\n"COLOR_RESET);
		printf(COLOR_BLUE"<MRU>=============================================<MRU>\n"COLOR_RESET);
		
		for(tmp = TNQ_head; tmp->next != NULL; tmp = tmp -> next)//tmp->T1_next != NULL
		{
			PC_c++;
			if(PC_c < 10)
				printf(COLOR_BLUE"%d, Page_no = %4lu\t\n",PC_c , tmp->page_no);
			
		}
		PC_c++;
		printf(COLOR_BLUE"%d, Page_no = %4lu\t\n",PC_c , tmp->page_no);
		printf(COLOR_BLUE"<LRU>=============================================<LRU>\n"COLOR_RESET);
	}
	else
	{
		printf(COLOR_RED"No data in the TNQ\n"COLOR_RESET);
	}
}

void displayT1Table()
{
	int PC_c = 0;
	unsigned long diskBlkno;
	if (T1head != NULL)
	{
		NODE *tmp;
		fprintf(result, "<T1 Table>\n");
		fprintf(result, "T1Size = %d\n", T1Size);
		fprintf(result, "T1Used = %d\n", T1Used);
		fprintf(result, "Flag 1 = Clean Page, Flag 0 = Dirty Page \n");
		fprintf(result, "<MRU>=============================================<MRU>\n");
		
		for(tmp = T1head; tmp->T1_next != NULL; tmp = tmp -> T1_next)//tmp->T1_next != NULL
		{
			PC_c++;
			if(PC_c > T1Used - 10){
				diskBlkno = (tmp -> page_no)*8;
				fprintf(result, "%d, Page_no = %4lu\t <=> HDD Block Number = %4lu\t <=> Flag = %d\n",PC_c , tmp->page_no, diskBlkno, tmp->flag);
		
			}
		}
		PC_c++;
		diskBlkno = (tmp -> page_no)*8;
		fprintf(result, "%d, Page_no = %4lu\t <=> HDD Block Number = %4lu\t <=> Flag = %d\n",PC_c , tmp->page_no, diskBlkno, tmp->flag);
		fprintf(result, "<LRU>=============================================<LRU>\n");
	}
	else
	{
		fprintf(result, "No data in the T1 queue\n");
	}
}
void displayT2Table()
{
	int PC_c = 0;
	unsigned long diskBlkno;
	if (T2head != NULL)
	{
		NODE *tmp;
		fprintf(result ,"<T2 Table>\n");
		fprintf(result ,"T2Size = %d\n", PageCacheSize - T1Size);
		fprintf(result ,"T2Used = %d (T1Used = %d)\n", T2Used, T1Used);
		fprintf(result ,"Flag 1 = Clean Page, Flag 0 = Dirty Page \n");
		fprintf(result ,"<MRU>=============================================<MRU>\n");
		
		for(tmp = T2head; tmp->T2_next != NULL ; tmp = tmp->T2_next)
		{
			PC_c++;
			if(PC_c > T2Used - 20)
			{
				diskBlkno = (tmp -> page_no)*8;
				fprintf(result ,"%d, Page_no = %4lu\t <=> HDD Block Number = %4lu\t <=> Flag = %d\n",PC_c , tmp->page_no, diskBlkno, tmp->flag);
	
			}
		}
		PC_c++;	
		diskBlkno = (tmp -> page_no)*8;
		fprintf(result ,"%d, Page_no = %4lu\t <=> HDD Block Number = %4lu\t <=> Flag = %d\n",PC_c , tmp->page_no, diskBlkno, tmp->flag);
		fprintf(result ,"<LRU>=============================================<LRU>\n");
	}
	else
	{
		printf(COLOR_RED"No data in the T2 queue\n"COLOR_RESET);
	}
}
void displayGHOSTbuffer()
{
	int GHOST_c = 0;
	if (Ghead != NULL)
	{
		NODE *tmp;
		fprintf(result ,"<GHOST buffer Table>\n");
		fprintf(result ,"GHOSTbufferSize = %d\n", GHOSTbufferSize);
		fprintf(result ,"Flag 1 = Clean Page, Flag 0 = Dirty Page \n");
		fprintf(result ,"<MRU>=============================================<MRU>\n");
		
		
		for(tmp = Ghead; tmp->ghost_next != NULL ; tmp = tmp->ghost_next)
		{
			GHOST_c++;
			if(GHOST_c > GHOSTbufferUsed - 10)
				fprintf(result ,"%d, Page_no = %4lu\n",GHOST_c , tmp->page_no);
		}
		GHOST_c++;		
		fprintf(result ,"%d, Page_no = %4lu\n",GHOST_c , tmp->page_no);
		fprintf(result ,"<LRU>=============================================<LRU>\n");
	}
	else
	{
		printf(COLOR_RED"No data in the GHOST buffer\n"COLOR_RESET);
	}
}
void displaySSDTable()
{
	int SSD_c = 0;
	if (SSDhead != NULL)
	{
		NODE *tmp;
		printf(COLOR_YELLOW"<SSD Table>\n"COLOR_RESET);
		printf(COLOR_YELLOW"SSDSize = %d\n", SSDSize);
		printf(COLOR_YELLOW"Flag 1 = Clean Page, Flag 0 = Dirty Page \n");
		printf(COLOR_YELLOW"<MRU>=============================================<MRU>\n"COLOR_RESET);
		
		
		for(tmp = SSDhead; tmp->SSD_next != NULL ; tmp = tmp->SSD_next)
		{
			SSD_c++;
			printf(COLOR_YELLOW"%d, Page_no = %4lu\t <=> Flag = %d\n",SSD_c, tmp->page_no, tmp->flag);
		}
		SSD_c++;		
		printf(COLOR_YELLOW"%d, Page_no = %4lu\t <=> Flag = %d\n",SSD_c, tmp->page_no, tmp->flag);
		printf(COLOR_YELLOW"<LRU>=============================================<LRU>\n"COLOR_RESET);
	}
	else
	{
		printf(COLOR_YELLOW"No data in the SSD Table\n"COLOR_RESET);
	}
}
void displayG1Table()
{
	int G_c = 0;
	unsigned long diskBlkno;
	if (G1head != NULL)
	{
		GHOST *tmp;
		printf(COLOR_GREEN"<G1 Table>\n"COLOR_RESET);		

		printf(COLOR_GREEN"<MRU>=============================================<MRU>\n"COLOR_RESET);
		
		for(tmp = G1head; tmp->G1_next != NULL; tmp = tmp -> G1_next)//tmp->T1_next != NULL
		{
			G_c++;
			//diskBlkno = (tmp -> page_no)*8;
			printf(COLOR_GREEN"%d, Page_no = %4lu\t<=> Flag = %d\n",G_c , tmp->page_no, tmp -> flag);
		}
		G_c++;
		//diskBlkno = (tmp -> page_no)*8;
		printf(COLOR_GREEN"%d, Page_no = %4lu\t<=> Flag = %d\n",G_c , tmp->page_no, tmp -> flag);
		printf(COLOR_GREEN"<LRU>=============================================<LRU>\n"COLOR_RESET);
	}
	else
	{
		printf(COLOR_RED"No data in the G1 queue\n"COLOR_RESET);
	}
}
void displayG2Table()
{
	int G_c = 0;
	unsigned long diskBlkno;
	if (G2head != NULL)
	{
		GHOST *tmp;
		fprintf(result ,"<G2 Table>\n");	
		
		fprintf(result ,"<MRU>=============================================<MRU>\n");
		
		for(tmp = G2head; tmp->G2_next != NULL; tmp = tmp -> G2_next)//tmp->T1_next != NULL
		{
			G_c++;
			if(G_c == 0 || G_c > G2Used - 10)
				fprintf(result ,"%d, Page_no = %4lu\t<=> Flag = %d\n",G_c , tmp->page_no, tmp -> flag);
		}
		G_c++;
		//diskBlkno = (tmp -> page_no)*8;
		fprintf(result ,"%d, Page_no = %4lu\t<=> Flag = %d\n",G_c , tmp->page_no, tmp -> flag);
		fprintf(result ,"<LRU>=============================================<LRU>\n");
	}
	else
	{
		fprintf(result ,"No data in the G2 queue\n");
	}
}
int max(int Gs1, int Gs2)
{
	if(Gs1 > Gs2)
		return Gs1;
	else
		return Gs2;
}
int min(int Gs1, int Gs2)
{
	if(Gs1 < Gs2)
		return Gs1;
	else
		return Gs2;
}

REQ *PageReqMaker(unsigned long page_no, int flag)
{
	REQ *PageReq;
	PageReq = malloc(sizeof(REQ));

	PageReq -> diskBlkno = page_no*SSD_PAGE2SECTOR;
	PageReq -> reqFlag = flag;
	PageReq -> reqSize = 8;

	return PageReq;
}

int get_NODE_counts()
{
	return TNQ_Size;
}







