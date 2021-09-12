#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "./ezipc.h"

void producer(char **argv, int pro, char *mem_pro, int squ, char *shared_DEF){ 
 int i;
 char EOLN[5] = "<EOL>";

 FILE *file = fopen(argv[1], "r");
 int count=0;
 char temp;
 
 if (file == NULL)
 exit(0);

 do{  
     if (feof(file)){
       *shared_DEF = 'e';
       break;
     }
     
     temp = fgetc(file);
     if(temp == '\n'){
       continue;
     }

     P(pro);
     *mem_pro = temp;
     count ++;
     V(squ);   
   
     
     if (count == 20){
       for(i=0; i<5; i++){
       	 P(pro);
       	 *mem_pro = EOLN[i];
         V(squ);
        }
        count = 0;
    }
  
    } while(1);
    
    fclose(file);
    
}

void squash(int pro, char *mem_pro, int squ, char *mem_squ, int squ_2, int pri, char *shared_DEF){
 
   char buff;
   P(squ);
   buff = *mem_pro;
   V(pro);

 while(1){  

 if( *shared_DEF == 'e'){
     P(squ_2);
     *shared_DEF = 'z';
     V(pri);
     return; 
  }

  P(squ);
  if ( buff == '*' && *mem_pro == '*'){
  
   P(squ_2);
   *mem_squ = '#';
   V(pri);
   V(pro);
   P(squ);
   
   buff = *mem_pro;
  }

  else{
   P(squ_2);
   *mem_squ = buff;
   V(pri);

   buff = *mem_pro;
  }
 
  V(pro);
}
}
   
void printer(int squ_2, char *mem_squ, int pri, char *shared_DEF){
  
   int count = 0;

   FILE *fptr = fopen("output.txt", "w+");

   while (1){
     
    P(pri);
    if (*shared_DEF == 'z'){
        V(squ_2);
        printf("\nthis is the end ...");
        break;
    }
     
    

         fprintf(fptr, "%c", *mem_squ);
         count ++;
         V(squ_2);
     
     if(count == 25){
        fprintf(fptr, "\n");
        count = 0;
     }
    }
     
   fclose(fptr);

}


int main(int argc, char* argv[])
{

  char *mem_pro;
  char *mem_squ;
  char *shared_DEF;
  int x;

  SETUP();
  
  shared_DEF = SHARED_MEMORY(1);
  *shared_DEF = 'o';
 
  int pro = SEMAPHORE(SEM_BIN, 1);
  int squ  = SEMAPHORE(SEM_BIN, 0);
  int squ_2  = SEMAPHORE(SEM_BIN, 1);
  int pri  = SEMAPHORE(SEM_BIN, 0);

  mem_pro = SHARED_MEMORY(1);
  mem_squ = SHARED_MEMORY(1);

  *mem_pro = '\0';
  *mem_squ = '\0';

  switch (x= COBEGIN(3))
  {
  case 1: producer(argv, pro, mem_pro, squ, shared_DEF); break;

  case 2: squash(pro, mem_pro, squ, mem_squ, squ_2, pri, shared_DEF); break;

  case 3: printer(squ_2, mem_squ, pri, shared_DEF); break;
  }

  COEND(x);

}
