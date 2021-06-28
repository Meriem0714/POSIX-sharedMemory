
/** Compilation: gcc -o reader reader.c -lrt -lpthread **/
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

const char *BackingFile = "memEx";
mode_t AccessPerms = 0644;
const int ByteSize = 1024;
const char *SemaphoreName = "semaphore01";

/**************************************************************************/
while(1){
  int fd = shm_open(BackingFile, O_RDWR, AccessPerms);  /* empty to begin */
  if (fd < 0) report_and_exit("Can't get file descriptor...");

  
/****************************************************************************/

  caddr_t memptr = mmap(NULL,       
                        ByteSize,   
                        PROT_READ | PROT_WRITE, 
                        MAP_SHARED, 
                        fd,         
                        0);         
  if ((caddr_t) -1 == memptr) report_and_exit("Can't access segment...\n");
 
  
/*****************************************************************************/
//semaphore

  sem_t* semptr = sem_open(SemaphoreName, /* name */
                           O_CREAT,       /* create the semaphore */
                           AccessPerms,   /* protection perms */
                           0);            /* initial value */
  if (semptr == (void*) -1) report_and_exit("sem_open");



/* use semaphore as a mutex (lock) by waiting for writer to increment it */
  if (!sem_wait(semptr)) { /* wait until semaphore != 0 */
    int i;
    for (i = 0; i < ByteSize; i++)
      write(STDOUT_FILENO, memptr + i, 1); /* one byte at a time */
      printf("\n");
    sem_post(semptr);
  }




/* cleanup */
  munmap(memptr, ByteSize);
  close(fd);
  sem_close(semptr);
  unlink(BackingFile);
  }
  return 0;



}
