#include  <stdio.h>
#include  <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int i, status, length;
 pid_t child_pid;
 pid_t pid;
 int back = 0;

char *pch[1000];
char *delimiter = " ";

char *getLine(char *line, int size){
  if(fgets(line, size, stdin)){
    char* newline = strchr(line, '\n');
    if(newline){
     *newline = '\0';
    }
 
 length = strlen(line)-1;
 while (isspace(*(line + length))){
   length --;
}
 *(line + length + 1) = '\0';
 }
  return line;
}

int main () {
 char text[1000] = "";
while(1){
 fputs("my prompt: ", stdout);
 fflush(stdout);
 getLine(text, sizeof text);
 
 while(strlen(text) == 0){
    fputs("my prompt: ", stdout);
    getLine(text, sizeof text);
 }
 if (!strcmp(text, "exit")) {
   break;
 }

  length = strlen(text);
   if (text[strlen(text)-1] == '&'){
     text[strlen(text)-1] = '\0';
     if (strlen(text) == 0){
        continue;
     }

   back = 1;
  }

  i = 0;
  pch[i] = strtok (text, delimiter);
  while (pch[i] != NULL)
  { 
  pch[++i] = strtok (NULL, delimiter);
  }

  pid = fork();

  if(pid == -1){
   perror("error creating process");
   exit(1);
 }

  if(pid == 0){
        execvp(pch[0], &pch[0]);
        perror("exec failure ");
        return 1;
  }

  if(back == 0){
    fflush(stdout);
    while (( child_pid = wait(&status)) && child_pid != -1);
  }

  back=0;
  fflush(stdout);
  }

printf("\n go back home\n\n");
return 0;

}





