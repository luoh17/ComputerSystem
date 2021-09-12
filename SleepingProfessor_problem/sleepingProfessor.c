#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "./ezipc.h"

pid_t pid;

/* inputs */
int students_max;
int chairs_max;

void quitHandler(int);

void Professor(int students, int professor, int chairs, char* free_chairs, char* summary)
{
 int pid_p = getpid();
 int count = 0;

 printf("professor id: %d\n", pid_p);

 while(1)
 {
 P(professor);
 P(chairs);
 /*a free chair available*/
 *free_chairs +=2;
 count += 2;
 *summary = count;
 printf("%d students already got helped.\n", count);
 V(students);
 V(students);
 V(chairs);

 /* answer_questions */

 }

}

void Student(int students, int professor, int chairs, char* free_chairs, 
    int binary, int x, char* wait_students)
{
 /* sleep(3);*/

 int officeTime = rand()%10 + 1;
 sleep(officeTime);

 printf("student id: %d comes in\n", x);

 P(chairs);

 if (*free_chairs > 0){

     *free_chairs -=1;
     
     V(chairs);
     P(students);
     P(binary);
     *wait_students += 1;
     if(*wait_students == 2){
        *wait_students =0;
        V(professor);
     }

     V(binary);
     /*get help from professor*/
   }
 else{
  /* release the mutex*/
   V(chairs);
   }
exit(0); 
}


int main(int argc, char* argv[])
{
  int x, status, child_pid, parent_pid;
 
  SETUP();

  printf("maximum number of student:" );
  scanf("%d", &students_max);
  printf("maximum number of chairs:");
  scanf("%d", &chairs_max);
 
  int students = SEMAPHORE(SEM_CNT, 2);
  int professor = SEMAPHORE(SEM_BIN, 0);
  int mutex_chairs = SEMAPHORE(SEM_BIN, 1);
  int binary = SEMAPHORE(SEM_BIN, 1);

  char *free_chairs = SHARED_MEMORY(1);
  *free_chairs = chairs_max;

  char *wait_students =  SHARED_MEMORY(1);
  *wait_students = 0;
 
  char *summary = SHARED_MEMORY(1);

  switch (x= COBEGIN(students_max+1))
  {
  case 0: break;

  case 1: signal(SIGUSR1, quitHandler);Professor(students, professor, mutex_chairs,
                 free_chairs, summary); break;

  default: Student(students, professor, mutex_chairs, free_chairs, binary, x,
                  wait_students); break;
  }
  
  COEND(x);   
}


void quitHandler(int theInt){
  exit(0);
}

