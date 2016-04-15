/* Simple Shell Program
 * Author: Quang Tran, Cody Bolhman
 * Last Modified: 4/15/2016
 */

#include    <stdlib.h>
#include    <stdio.h>
#include    <unistd.h>
#include    <string.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include    <stdbool.h>
#include    <fcntl.h>

char** readLineOfWords();
int getArraySize();

int main()
{
  size_t MAX_WORD_LENGTH = 100;
  size_t MAX_NUM_COMMANDS = 40;
  size_t MAX_NUM_WORDS = 51;

  while (1){
    printf("enter a shell command (e.g. ls): ");
    fflush(stdout);
    char** words = readLineOfWords();
    // counter to keep track of the current word in the command line
    int wordCounter = 0;

    char** inputFiles = (char**) malloc( MAX_NUM_WORDS * sizeof(char*));
    char** outputFiles = (char**) malloc( MAX_NUM_WORDS * sizeof(char*));
    bool wait = true;
    char*** execCommand = (char***) malloc(MAX_NUM_COMMANDS * MAX_NUM_WORDS * sizeof(char*));

    // counter to count total number of commands separated by pipe and given by user
    int totalCommands = 0;
    // counter to keep track of the word in current command
    int currentInCommand = 0;
    // counter to keep track of num of input and output files given by user.
    // If there is more than 1, it should still be a valid input, but the terminal will only consider the last one
    int inputCounter = 0;
    int outputCounter = 0;
    int inputLength = getArraySize(words);

    bool doAdd = true;
    execCommand[totalCommands] = (char**) malloc( MAX_NUM_WORDS * sizeof(char*));
    while (wordCounter<inputLength) {
        if (strcmp(words[wordCounter],"|") != 0 && strcmp(words[wordCounter],"<") != 0 && strcmp(words[wordCounter],">") != 0 && strcmp(words[wordCounter],"&") != 0){
          execCommand[totalCommands][currentInCommand] = words[wordCounter];
          currentInCommand++;
          wordCounter++;
        }
        else if (strcmp(words[wordCounter],"|") == 0){
          totalCommands++;
          execCommand[totalCommands] = (char**) malloc( MAX_NUM_WORDS * sizeof(char*));
          currentInCommand = 0;
          wordCounter++;
        }
        else if (strcmp(words[wordCounter],"<") == 0){
          wordCounter++;
          inputFiles[inputCounter] = words[wordCounter];
          inputCounter++;
          wordCounter++;
        }
        else if (strcmp(words[wordCounter],">") == 0){
          wordCounter++;
          outputFiles[outputCounter] = words[wordCounter];
          outputCounter++;
          wordCounter++;
        }
        else if (strcmp(words[wordCounter],"&") == 0){
          wordCounter++;
          wait = false;
        }
      }


    int currentCommand = 0;
    long i;
    // fork splits process into 2 identical processes that both continue
    // running from point where fork() returns. Only difference is return
    // value - 0 to the child process, pid of child to the parent process
    int pid = fork();

    // if 0 is returned, execute code for child process
    if( pid == 0){
      // take in input file. When the user types in several inputs, the only valid one is the last one
      if (getArraySize(inputFiles) > 0){
        int fileInput = open(inputFiles[getArraySize(inputFiles) - 1], O_CREAT|O_RDONLY, 6666);
        dup2(fileInput, 0);
      }
      free(inputFiles);
      // while there are still commands to run. Doesn't go here if there is only 1 command in user-given line
      while (currentCommand < totalCommands){
        // create a pipe
        int pfd[2];
        if (pipe(pfd) == 0){
          int pipepid = fork();
          if (pipepid == 0){
            close(pfd[0]);
            dup2(pfd[1], 1);
            waitpid(pipepid,NULL,0);
            execvp(execCommand[currentCommand][0], execCommand[currentCommand]);
          }
          else{
            close(pfd[1]);
            dup2(pfd[0], 0);
          }
        }
        currentCommand++;
      }

      // if there is output file, direct the result there
      if (getArraySize(outputFiles) > 0){
        int newfd = open(outputFiles[getArraySize(outputFiles) - 1], O_CREAT|O_WRONLY, 0644);
        dup2(newfd, 1);
      }
      free(outputFiles);
      //execute last command
      execvp(execCommand[currentCommand][0], execCommand[currentCommand]);
    }

    // shell process
    else{
      if (wait){
        waitpid(pid,NULL,0);
      }
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

/*
 * takes in a pointer to an array and returns it's size (or num of elements from the beginning)
 */
int getArraySize(char** array) {
  int actualSize = 0;
  int i = 0;
  while(array[i] != NULL) {
    i++;
    actualSize++;
  }
  return actualSize;
}
