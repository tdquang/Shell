/* Simple example of using execvp with helpful function to read a line from 
 * stdin and parse it into an array of individual tokens
 * Author: Sherri Goings
 * Last Modified: 1/18/2014
 */

 // TODO - Implement array size checking (check if libraries exist)

#include    <stdlib.h>
#include    <stdio.h>
#include    <unistd.h>
#include    <string.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include    <stdbool.h>

char** readLineOfWords();
int getArraySize();

int main()
{
  size_t MAX_WORD_LENGTH = 100;
  size_t MAX_NUM_WORDS = 51;

  while (1){
    printf("enter a shell command (e.g. ls): ");
    fflush(stdout);
    char** words = readLineOfWords();

    char** inputFiles = (char**) malloc( MAX_NUM_WORDS * sizeof(char*));

    char** outputFiles = (char**) malloc( MAX_NUM_WORDS * sizeof(char*));
    bool wait = true;
    bool pipe = false;
    char** execCommand = (char**) malloc( MAX_NUM_WORDS * sizeof(char*) );
    int counter = 0;
    int inputCounter = 0;
    int outputCounter = 0;
    bool endCommand = false;
    int inputLength = getArraySize(words);

    for (int i = 0; i < inputLength; i++){
      printf("%s\n", words[i]);
    }
    //printf("%i\n", sizeof(words));
    //fflush(stdout);
    while (!endCommand && counter<inputLength) {
      if (!strcmp(words[counter],"&") || !strcmp(words[counter],"|") || !strcmp(words[counter],">") || !strcmp(words[counter],"<")){
        execCommand[counter] = words[counter];
        counter++;
      }
      else{
        endCommand = true;
      }
    }
    // printf("1.2");
    // fflush(stdout);
    while (counter<inputLength){
      printf("%d",counter);
      fflush(stdout);
      if (strcmp(words[counter], "&")){
        wait = false;
        counter ++;
      }
      else if (strcmp(words[counter], "|")){
        pipe = true;
        counter ++;
      }
      else if (strcmp(words[counter], ">")){
        strcpy(outputFiles[outputCounter++], words[counter++]);
        counter++;
      }
      else if (strcmp(words[counter], "<")){
        strcpy(inputFiles[inputCounter++], words[counter++]);
        counter++;
      }
      else{
        counter++;
      }
      
    }
    // printf("2");
    // fflush(stdout);
    long i;

    // fork splits process into 2 identical processes that both continue
    // running from point where fork() returns. Only difference is return
    // value - 0 to the child process, pid of child to the parent process  
    int pid = fork();

    // if 0 is returned, execute code for child process
    if( pid == 0 ){
      execvp(words[0], words);
      fflush( stdout );
    }
    if (wait){
      waitpid(pid,NULL,0);
    }
  }
return 0;
}

/* 
 * reads a single line from terminal and parses it into an array of tokens/words by 
 * splitting the line on spaces.  Adds NULL as final token 
 */
char** readLineOfWords() {

  // A line may be at most 100 characters long, which means longest word is 100 chars, 
  // and max possible tokens is 51 as must be space between each
  size_t MAX_WORD_LENGTH = 100;
  size_t MAX_NUM_WORDS = 51;

  // allocate memory for array of array of characters (list of words)
  char** words = (char**) malloc( MAX_NUM_WORDS * sizeof(char*) );
  int i;
  for (i=0; i<MAX_NUM_WORDS; i++) {
    words[i] = (char*) malloc( MAX_WORD_LENGTH );
  }

  // read actual line of input from terminal
  int bytes_read;
  char *buf;
  buf = (char*) malloc( MAX_WORD_LENGTH+1 );
  bytes_read = getline(&buf, &MAX_WORD_LENGTH, stdin);
 
  // take each word from line and add it to next spot in list of words
  i=0;
  char* word = (char*) malloc( MAX_WORD_LENGTH );
  word = strtok(buf, " \n");
  while (word != NULL && i<MAX_NUM_WORDS) {
    strcpy(words[i++], word);
    word = strtok(NULL, " \n");
  }

  // check if we quit because of going over allowed word limit
  if (i == MAX_NUM_WORDS) {
    printf( "WARNING: line contains more than %d words!\n", (int)MAX_NUM_WORDS ); 
  } 
  else
    words[i] = NULL;
  
  // return the list of words
  return words;
}

int getArraySize(char** array) {
  int actualSize = 0;
  int i = 0;
  while(array[i] != NULL) {
    i++;
    actualSize++;
  }
  printf("Size is %d\n\n",actualSize);
  fflush(stdout);
  return actualSize;
}
