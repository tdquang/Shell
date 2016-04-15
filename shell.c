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
    int wordCounter = 0;

    char** inputFiles = (char**) malloc( MAX_NUM_WORDS * sizeof(char*));
    char** outputFiles = (char**) malloc( MAX_NUM_WORDS * sizeof(char*));
    bool wait = true;
    char*** execCommand = (char***) malloc(MAX_NUM_COMMANDS * MAX_NUM_WORDS * sizeof(char*) );

    // char** commands = malloc( sizeof(execCommand));
    int totalCommands = 0;
    int currentInCommand = 0;
    int inputCounter = 0;
    int outputCounter = 0;
    bool endCommand = false;
    int inputLength = getArraySize(words);
    // printf("%d",inputLength);
    // fflush(stdout);
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
    // printf("Here1");
    // fflush(stdout);

        // printf("HERE!");
        // fflush(stdout);
    long i;
        // printf("Here2");
        // fflush(stdout);
        // fork splits process into 2 identical processes that both continue
        // running from point where fork() returns. Only difference is return
        // value - 0 to the child process, pid of child to the parent process
    int pid = fork();
        // if 0 is returned, execute code for child process
    if( pid == 0){
      if (getArraySize(inputFiles) > 0){
        int fileInput = open(inputFiles[getArraySize(inputFiles) - 1], O_CREAT|O_RDONLY, 6666);
        dup2(fileInput, 0);
      }

      if (getArraySize(outputFiles) > 0){
        //printf("\n\n%s\n\n",outputFiles[0]);
        //fflush(stdout);
        int newfd = open(outputFiles[getArraySize(outputFiles) - 1], O_CREAT|O_WRONLY, 0644);
        dup2(newfd, 1);
      }

      while (currentCommand < totalCommands){
        // printf("\nsomething\n");
        // fflush(stdout);
        int pfd[2];
        if (pipe(pfd) == 0){
          int pipepid = fork();
          if (pipepid == 0){
            close(pfd[0]);
            dup2(pfd[1], 1);
            printf("\nStdin %d\n", pfd[1]);
            fflush(stdout);
            execvp(execCommand[currentCommand][0], execCommand[currentCommand]);
            waitpid(pipepid,NULL,0);
          }
          else{
            close(pfd[1]);
            dup2(pfd[0], 0);
            printf("\nStdout %d\n", pfd[0]);
            fflush(stdout);
          }
        }
        currentCommand++;
      }
      execvp(execCommand[currentCommand][0], execCommand[currentCommand]);

    }
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

int getArraySize(char** array) {
  int actualSize = 0;
  int i = 0;
  while(array[i] != NULL) {
    i++;
    actualSize++;
  }
  return actualSize;
}
