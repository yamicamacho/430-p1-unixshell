#include "shell.h"
/*************************************
 * By: Yamileth Camacho
 * Class: CSS430 Operating systems
 *
 * NOTES:
 * - everything works and gives expected output
 * - The osh> promt can get behind which makes the program lag
 * - once you type into the osh> promt again it works as expected
 ***************************************/

// Main method to either use interactively
// or run the basic tests
int main(int argc, char **argv) {
  if (argc == 2 && equal(argv[1], "--interactive")) {
    return interactiveShell();
  } else {
    return runTests();
  }
}

// interactive shell to process commands
int interactiveShell() {
  bool should_run = true;
  char *line = calloc(1, MAXLINE);
  char previousCommand[MAXLINE + 1];
  strcpy(previousCommand, "");

  while (should_run) {
    printf(PROMPT);
    fflush(stdout);
    int n = fetchline(&line);
    printf("read: %s (length = %d)\n", line, n);
    // ^D results in n == -1
    if (n == -1 || equal(line, "exit")) {
      should_run = false;
      continue;
    }
    if (equal(line, "")) {
      continue;
    }
    if (equal(line, "!!")) {
      if (equal(previousCommand, "")) {
        printf("No commands in history\n");
        continue;
      }
      free(line);
      line = strdup(previousCommand);
    }

    strcpy(previousCommand, line); // updating
    processLine(line);
    sleep(2);
  }
  free(line);
  return 0;
}

// Processes each line case by case
// parse it then finds the operator then executes depending on the case
int processLine(char *line) {

  printf("processing line: %s\n", line);

  if (equal(line, "ascii")) {
    printf("  |\\_/|\n");
    printf(" / @ @ \\     ****************************    (\\_/)\n");
    printf("( > º < )    *  \"Purrrfectly pleasant\"  *   (='.'=)\n");
    printf(" `>>x<<´     *       Poppy Prinz        *   (\")_(\")\n");
    printf(" /  O  \\     *   (pprinz@example.com)   *\n");
    printf("             ****************************\n");
    return 0;
  }
  char *args[MAXLINE / 2 + 1];

  for (int i = 0; i < MAXLINE / 2 + 1; i++) {
    args[i] = '\0'; // adding \0 to the end to indicate end of line
  }

  int tokLength = 1;
  args[0] = strtok(line, " ");

  while ((args[tokLength] = strtok(NULL, " "))) {
    tokLength++; // number of tokens
  }

  int operator= 0;       // counts how many oprators
  int operatorIndex = 0; // cheack index of opeartor in list

  for (int i = 0; i < tokLength; i++) {
    if (equal(args[i], ";")) {
      operator= 1;
      operatorIndex = i;
      break;
    }
    if (equal(args[i], "&")) {
      operator= 2;
      operatorIndex = i;
      break;
    }
    if (equal(args[i], "|")) {
      operator= 3;
      operatorIndex = i;
      break;
    }
    if (equal(args[i], "<")) {
      operator= 4;
      operatorIndex = i;
      break;
    }
    if (equal(args[i], ">")) {
      operator= 5;
      operatorIndex = i;
      break;
    }
  }

  // If else statements to deal with each operator type
  if (operator== 1) { // for ; symbol (parent must wait for child to finish)
    // if there is two commands
    char *firstCommand[MAXLINE / 2 + 1];
    char *secondCommand[MAXLINE / 2 + 1];

    for (int i = 0; i < MAXLINE / 2 + 1; i++) {
      firstCommand[i] = '\0';
      secondCommand[i] = '\0';
    }

    for (int i = 0; i < tokLength; i++) {
      // putting first command and second command
      if (i < operatorIndex) {
        firstCommand[i] = args[i];
      } else if (i > operatorIndex) {
        secondCommand[i - (operatorIndex + 1)] = args[i];
      }
    }
    char *secConcantination;
    if (secondCommand[0] != NULL) {
      secConcantination = malloc(MAXLINE); // to be safe
      strcpy(secConcantination, secondCommand[0]);
      if (secondCommand[1] != NULL) {
        strcat(secConcantination, " ");
        strcat(secConcantination, secondCommand[1]);
      }
    }

    // firstCommand character
    int pid = fork();
    if (pid == 0) { // child process
      execvp(firstCommand[0], firstCommand);
      return 0;
    } else {
      int status;
      wait(&status); // Parent waits
    }
    printf("\n");

    if (secondCommand[0] != NULL) {
      processLine(secConcantination);
    }
    return 0;
  } else if (operator== 2) // for & symbol (do not wait for child to finish)
  {
    char *firstCommand[MAXLINE / 2 + 1];
    char *secondCommand[MAXLINE / 2 + 1];

    for (int i = 0; i < MAXLINE / 2 + 1; i++) {
      firstCommand[i] = '\0';
      secondCommand[i] = '\0';
    }

    for (int i = 0; i < tokLength; i++) {
      if (i < operatorIndex) {
        firstCommand[i] = args[i];
      } else if (i > operatorIndex) {
        secondCommand[i - (operatorIndex + 1)] = args[i];
      }
    }

    char *secondConcantination;
    if (secondCommand[0] != NULL) {
      secondConcantination = malloc(MAXLINE); // to be safe
      strcpy(secondConcantination, secondCommand[0]);
      if (secondCommand[1] != NULL) {
        strcat(secondConcantination, " ");
        strcat(secondConcantination, secondCommand[1]);
      }
    }

    // firstCommand
    int pid = fork();
    if (pid == 0) { // child process
      execvp(firstCommand[0], firstCommand);
      return 0;
    } else {             // actual parent does not wait
      int pid2 = fork(); // second fork if there is another command
      if (pid2 == 0) {   // Second child process
        if (secondCommand[0] != NULL) {
          processLine(secondConcantination);
        }
        return 0;
      } else {
        // no waiting
      }
    }
    return 0;
  } else if (operator== 3) // for | symbol (separate multiple commands?)
  {
    char *firstCommand[MAXLINE / 2 + 1];
    char *secondCommand[MAXLINE / 2 + 1];

    for (int i = 0; i < MAXLINE / 2 + 1; i++) {
      firstCommand[i] = '\0';
      secondCommand[i] = '\0';
    }

    for (int i = 0; i < tokLength; i++) {
      if (i < operatorIndex) {
        firstCommand[i] = args[i];
      } else if (i > operatorIndex) {
        secondCommand[i - (operatorIndex + 1)] = args[i];
      }
    }

    int thePipe[2];
    pipe(thePipe);

    int pid = fork();
    if (pid == 0) { // first child of parent
      dup2(thePipe[WR], STDOUT_FILENO);
      close(thePipe[RD]);
      close(thePipe[WR]);
      execvp(firstCommand[0], firstCommand);
      return 0;
    } else { // parent
      int pid2 = fork();
      if (pid2 == 0) { // second child of parents
        dup2(thePipe[RD], STDIN_FILENO);
        close(thePipe[RD]);
        close(thePipe[WR]);
        execvp(secondCommand[0], secondCommand);
        return 0;
      } else {
        close(thePipe[RD]);
        close(thePipe[WR]);
        wait(NULL);
      }
      wait(NULL);
    }
    return 0;
  } else if (operator== 4) // for < symbol (taking out of file)
  {
    char *commandForFile[MAXLINE / 2 + 1];
    char *theFile[MAXLINE / 2 + 1];

    for (int i = 0; i < MAXLINE / 2 + 1; i++) {
      commandForFile[i] = '\0';
      theFile[i] = '\0';
    }

    for (int i = 0; i < tokLength; i++) {
      if (i < operatorIndex) {
        commandForFile[i] = args[i];
      } else if (i > operatorIndex) {
        theFile[i - (operatorIndex + 1)] = args[i];
        commandForFile[i - 1] = args[i];
      }
    }

    int pid = fork();
    if (pid == 0) {
      int fd = open(theFile[0], O_RDONLY);
      dup2(0, fd); // Redirects input to a file
      close(fd);
      execvp(commandForFile[0], commandForFile);
      return 0;
    } else {
      int status;
      wait(&status);
      printf("\n");
    }
    return 0;
  } else if (operator== 5) // for > symbol (puts it into the file)
  {
    char *firstCommand[MAXLINE / 2 + 1];
    char *theFile[MAXLINE / 2 + 1];

    for (int i = 0; i < MAXLINE / 2 + 1; i++) {
      firstCommand[i] = '\0';
      theFile[i] = '\0';
    }

    for (int i = 0; i < tokLength; i++) {
      if (i < operatorIndex) {
        firstCommand[i] = args[i];
      } else if (i > operatorIndex) {
        theFile[i - (operatorIndex + 1)] = args[i];
      }
    }

    int pid = fork();
    if (pid == 0) { // child
      int fd = open(theFile[0], O_WRONLY | O_CREAT | O_TRUNC,
                    S_IRUSR | S_IWUSR);
      dup2(fd, 1);
      dup2(fd, 2);
      close(fd);
      execvp(firstCommand[0], firstCommand);
      return 0;
    } else { // parent
      int status;
      wait(&status);
    }
    return 0;
  } else { // No special symbol
    int pid = fork();
    if (pid == 0) {          // child process
      execvp(args[0], args); // executes command
      return 0;
    } else {
      int status;
      wait(&status);
    }
    return 0;
  }
  return 0;
}

// Runs all the basic tests in the program
int runTests() {
  printf("*** Running basic tests ***\n");
  char lines[7][MAXLINE] = {
      "ls",      "ls -al", "ls & whoami ;", "ls > junk.txt", "cat < junk.txt",
      "ls | wc", "ascii"};
  for (int i = 0; i < 7; i++) {
    printf("* %d. Testing %s *\n", i + 1, lines[i]);
    processLine(lines[i]);
    sleep(2);
  }

  return 0;
}

// return true if C-strings are equal
bool equal(char *a, char *b) { return (strcmp(a, b) == 0); }

// read a line from console
// return length of line read or -1 if failed to read
// removes the \n on the line read
int fetchline(char **line) {
  size_t len = 0;
  size_t n = getline(line, &len, stdin);
  if (n > 0) {
    (*line)[n - 1] = '\0';
  }
  return n;
}