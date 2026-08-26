#ifndef __FINWO_CNFPARSE_H__
#define __FINWO_CNFPARSE_H__

#include <stddef.h>
#include <stdio.h>

struct cnf_directive {
  char   *name;
  size_t argc;
  char   **argv;
};

void cnf_directive_free(struct cnf_directive *subject);
struct cnf_directive * cnf_directive_read(FILE *fd);

#endif // __FINWO_CNFPARSE_H__
