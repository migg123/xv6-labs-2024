#include "../kernel/types.h"
#include "user.h"

int main(int argc, char *argv[])
{
  int p1[2], p2[2];   // p1: parent -> child, p2: child -> parent
  char buf;

  if (pipe(p1) < 0) {
    printf("pipe p1 failed\n");
    exit(0);
  }
  if (pipe(p2) < 0) {
    printf("pipe p2 failed\n");
    exit(0);
  }

  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(0);
  }

  if (pid == 0) {
    // Child
    close(p1[1]);   // close write end of parent->child
    close(p2[0]);   // close read end of child->parent

    // Read ping from parent
    if (read(p1[0], &buf, 1) != 1) {
      // read error or EOF
      close(p1[0]);
      close(p2[1]);
      exit(0);
    }
    printf("%d: received ping\n", getpid());

    // Send pong back to parent
    if (write(p2[1], &buf, 1) != 1) {
      close(p1[0]);
      close(p2[1]);
      exit(0);
    }

    close(p1[0]);
    close(p2[1]);
    exit(0);
  } else {
    // Parent
    close(p1[0]);   // close read end of parent->child
    close(p2[1]);   // close write end of child->parent

    // Send ping to child
    buf = 'x';
    if (write(p1[1], &buf, 1) != 1) {
      close(p1[1]);
      close(p2[0]);
      wait(0);
      exit(0);
    }

    // Read pong from child
    if (read(p2[0], &buf, 1) != 1) {
      close(p1[1]);
      close(p2[0]);
      wait(0);
      exit(0);
    }
    printf("%d: received pong\n", getpid());

    close(p1[1]);
    close(p2[0]);
    wait(0); 
    exit(0);
  }
}
