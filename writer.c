
/** Compilation: gcc -o writer writer.c -lrt -lpthread **/
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <sys/shm.h>


/*******************************************error meg*****************************/
void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}
/*********************************************************************************/

/*********************************main()******************************************/
int main(){


/*************************declaration*************************************/
const char *BackingFile = "memEx";
mode_t AccessPerms = 0644;
const int ByteSize = 1024;
const char *SemaphoreName = "semaphore01";

int CHAR =0;
/**************************************************************************/
//shm_open
while(1){
  int fd = shm_open(BackingFile,      
                    O_RDWR | O_CREAT, 
                    AccessPerms);
  
    if (fd < 0) report_and_exit("Can't open shared mem segment...");

/*****************************************************************************/
//get the bytes size
  ftruncate(fd, ByteSize);
  
/****************************************************************************/

  caddr_t memptr = mmap(NULL,       
                        ByteSize,   
                        PROT_READ | PROT_WRITE, 
                        MAP_SHARED, 
                        fd,         
                        0); 
                                
  if ((caddr_t) -1  == memptr) report_and_exit("Can't get segment...");

  
/*****************************************************************************/

  fprintf(stderr, "shared mem address: %p [0..%d]\n", memptr, ByteSize - 1);
  fprintf(stderr, "backing file:       /dev/shm%s\n", BackingFile );
  printf("______________________________________\n");
  
/*****************************************************************************/

//semaphore

  sem_t* semptr = sem_open(SemaphoreName, 
                           O_CREAT,       
                           AccessPerms,   
                           0);     
                           
   if (semptr == (void*) -1) report_and_exit("sem_open");   


 
  char Prt='0';
  if (CHAR == 0) Prt='0';
  if (CHAR == 1) Prt='1';
  if (CHAR == 2) Prt='2';
  if (CHAR == 3) Prt='3';
  if (CHAR == 4) Prt='4';
  if (CHAR == 5) Prt='5';
  if (CHAR == 6) Prt='6';
  if (CHAR == 7) Prt='7';
  if (CHAR == 8) Prt='8';
  if (CHAR == 9) Prt='9';
  

  
  memset(memptr,Prt,1024);
  if (CHAR >9) CHAR=0;

 
  
  if (sem_post(semptr) < 0) report_and_exit("sem_post");
  CHAR++;

 
  
  
  munmap(memptr, ByteSize); /* unmap the storage */
  close(fd);
  sem_close(semptr);
 }
  return 0;


}
