#ifndef YC_PARAMETER_H
#define YC_PARAMETER_H
	
FILE *trace,*statistics, *result;      	//Trace file; Stat file; Result file
/*SIMULATOR*/
#define DISKSIM_SECTOR   512  //(bytes)
#define SSD_PAGE_SIZE    4096 //(bytes)
#define SSD_PAGE2SECTOR (SSD_PAGE_SIZE/DISKSIM_SECTOR)
#define SSD_PAGES_PER_BLOCK 64
#define SSD_BLOCK_SIZE   (SSD_PAGE_SIZE*SSD_PAGES_PER_BLOCK) //(bytes)
//#define SSD_BLOCK2SECTOR (SSD_BLOCK_SIZE/DISKSIM_SECTOR)

//#define FREE_PAGE 0
#define CLEAN_PAGE 1
#define DIRTY_PAGE 0

#define PageCacheSize 4096
#define SSDSize PageCacheSize*5

#define PREIOD_SERVED_PAGES PageCacheSize*2
#define NUM_BLOCKS 16

/*ipc*/
//One message is considered as one request. The control message is a flag which used to control simulator
#define KEY_MSQ_DISKSIM_SSD 0x0015				//The key of message queue for SSD simulator
#define KEY_MSQ_DISKSIM_HDD 0x0026			//The key of message queue for HDD simulator
#define MSG_TYPE_DISKSIM_SSD 100				//The type of message for SSD simulator
#define MSG_TYPE_DISKSIM_SSD_SERVED 101			//The type of served message for SSD simulator
#define MSG_TYPE_DISKSIM_HDD 200				//The type of message for HDD simulator
#define MSG_TYPE_DISKSIM_HDD_SERVED 201			//The type of served message for HDD simulator

#define MSG_REQUEST_CONTROL_FLAG_FINISH 999		//The type of control message for simulator
#define MSG_REQUEST_PERIOD_SSD_TIME 1000
#define MSG_REQUEST_PERIOD_HDD_TIME 1001

/*cache*/
#define PAGE_FLAG_FREE 0		//The flag of free page in cache table 
#define PAGE_FLAG_NOT_FREE 1	//The flag of non-free page in caches table 
#define PAGE_FLAG_CLEAN 1		//The flag of clean page in cache table 
#define PAGE_FLAG_DIRTY -1		//The flag of dirty page in cache table 
#define CACHE_FULL 1		//The flag of cache means that cache is full
#define CACHE_NOT_FULL 0	//The flag of cache means that cache is not full


//ASCII Escape Code
/*Color*/
#define COLOR_RED "\x1b[;31;1m"
#define COLOR_GREEN "\x1b[;32;1m"
#define COLOR_YELLOW "\x1b[;33;1m"
#define COLOR_BLUE "\x1b[;34;1m"
#define COLOR_MAGENTA "\x1b[;35;1;1m"
#define COLOR_CYAN "\x1b[;36;1;1m"
#define COLOR_RESET "\x1b[0;m"



#endif
