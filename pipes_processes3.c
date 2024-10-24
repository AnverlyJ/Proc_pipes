#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "Usage: %s <search_term>\n", argv[0]);
    //exit(1);
  }

  int pipefd1[2];
  int pipefd2[2];
  pid_t pid1, pid2;

  if (pipe(pipefd1) == -1){
    perror("pipe1 failed");
    //exit(1);
  }

  if((pid1 = fork()) == -1){
    perror("fork1 failed");
    //exit(1);
  }

  if (pid1 == 0){

    if(pipe(pipefd2) == -1){
      perror("pipe2 failed");
      //exit(1);
    }

    if ((pid2 = fork() == -1)){
      perror("fork2 failed");
      //exit(1);
    }

    if (pid2 == 0){

      close(pipefd1[0]);
      close(pipefd1[1]);
      close(pipefd2[1]);

      dup2(pipefd2[0], STDIN_FILENO);
      close(pipefd2[0]);

      execlp("sort", "sort", NULL);
      perror("exec sort failed");
      //exit(1);

    }else{
      close(pipefd2[0]);
      dup2(pipefd1[0], STDIN_FILENO);
      dup2(pipefd2[1], STDOUT_FILENO);
      close(pipefd1[0]);
      close(pipefd2[1]);

      char *grep_args[] = {"grep", argv[1], NULL};

      execvp("grep", grep_args);
      perror("exec grep failed");
      //exit(1);
    }

  }else{
    close(pipefd1[0]);
    dup2(pipefd1[1], STDOUT_FILENO);
    close(pipefd1[1]);

    execlp("cat", "cat", "scores", NULL);
    perror("exec cat failed");
    //xit(1);
  }

  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
  return 0;

  // char *cat_args[] = {"cat", "scores", NULL};
  // 

  // // make a pipe (fds go in pipefd[0] and pipefd[1])

  // pipe(pipefd);

  // pid = fork();

  // if (pid == 0)
  //   {
  //     // child gets here and handles "grep Villanova"

  //     // replace standard input with input part of pipe

  //     dup2(pipefd[0], 0);

  //     // close unused hald of pipe

  //     close(pipefd[1]);

  //     // execute grep

  //     execvp("grep", grep_args);
  //   }
  // else
  //   {
  //     // parent gets here and handles "cat scores"

  //     // replace standard output with output part of pipe

  //     dup2(pipefd[1], 1);

  //     // close unused unput half of pipe

  //     close(pipefd[0]);

  //     // execute cat

  //     execvp("cat", cat_args);
  //   }
}