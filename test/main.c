#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cnfparse.h"

void usage() {
  printf("Usage:\n");
  printf("  cnfparse_test --config <path>\n");
  printf("  cnfparse_test --config=<path>\n");
  exit(0);
}

int main(int argc, char *argv[]) {
  char *path = NULL;

  // Handle params
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--config") == 0) {
      if (i + 1 >= argc) { usage(); }
      path = argv[++i];
    } else if (strncmp(argv[i], "--config=", 9) == 0) {
      path = argv[i] + 9;
    } else {
      usage();
    }
  }
  if (!path) {
    usage();
  }

  FILE *fd = fopen(path, "r");
  if (!fd) {
    fprintf(stderr, "%s: cannot open\n", path);
    exit(1);
  }

  while(1) {
    struct cnf_directive *dir = cnf_directive_read(fd);
    if (!dir) break;
    printf("dir:");
    if (dir->name) printf(" name=%s", dir->name);
    printf(" argc=%ld", dir->argc);
    for(int i = 0; i < dir->argc ; i++ ) {
      printf(" argv[%d]=[%s]", i, dir->argv[i]);
    }
    printf("\n");
  }

  return 42;
}
