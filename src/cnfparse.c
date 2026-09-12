#ifdef __cplusplus
extern "C" {
#endif

// includes/system {{{
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
// }}}
// includes/deps {{{
#include "finwo/buf.h"
// }}}
// includes/local {{{
#include "cnfparse.h"
// }}}

#define STATE_BLANK   0
#define STATE_COMMENT 1
#define STATE_QUOTE   2
#define STATE_WORD    3

// cnf_directive_append(subject,value,len) {{{
// caller frees
void cnf_directive_append(struct cnf_directive *subject, char *value, size_t len) {
  if (!value) return;
  char *val = calloc(1, len+1);
  memcpy(val, value, len);
  if (!subject->name) {
    subject->name = val;
    return;
  }
  subject->argv = realloc(subject->argv, (subject->argc + 1) * sizeof(char*));
  subject->argv[subject->argc++] = val;
}
// }}}
// cnf_directive_free(subject) {{{
void cnf_directive_free(struct cnf_directive *subject) {
  if (!subject) return;
  if (subject->name) {
    free(subject->name);
  }
  for(size_t i = 0 ; i < subject->argc ; i++) {
    free(subject->argv[i]);
  }
  if (subject->argv) {
    free(subject->argv);
  }
  free(subject);
}
// }}}
// cnf_directive_read(fd) {{{
struct cnf_directive * cnf_directive_read(FILE *fd) {
  if (feof(fd)) return NULL;

  struct buf *acc = calloc(1, sizeof(struct buf));
  struct cnf_directive *output = calloc(1, sizeof(struct cnf_directive));

  int state = STATE_BLANK;
  char c;
  char quote;
  bool escape;

  while(!feof(fd)) {
    if (!fread(&c, sizeof(char), 1, fd)) break;

    switch(state) {
      case STATE_BLANK:
        switch(c) {
          case ' ':
          case '\t':
          case '\r':
            // Skip leading whitespace
            // Consume without further action
            continue;
          case '\n':
            // Output directive if we have something
            if (output->name) goto cnf_directive_read_finalize;
            continue;
          case '#':
            state = STATE_COMMENT;
            continue;
          case '"':
          case '\'':
            state = STATE_QUOTE;
            quote = c;
            escape = false;
            continue;
          default:
            buf_append_byte(acc, c);
            state = STATE_WORD;
            continue;
        }
        break;
      case STATE_COMMENT:
        switch(c) {
          case '\n': // newline breaks comment
            if (output->name) goto cnf_directive_read_finalize; // Operator = directive complete
            state = STATE_BLANK;                                // No operator = read one more line
            // acc->len = 0;
            continue;
          default:
            // Consume without further action
            continue;
        }
        break;
      case STATE_QUOTE:
        if (escape) {
          buf_append_byte(acc, c);
          escape = false;
          continue;
        }
        if (c == '\\') {
          escape = true;
          continue;
        }
        if (c == quote) {
          cnf_directive_append(output, acc->dat, acc->len);
          state = STATE_BLANK;
          memset(acc->dat, 0, acc->cap);
          acc->len = 0;
          continue;
        }
        buf_append_byte(acc, c);
        continue;;
      case STATE_WORD:
        switch(c) {
          case ' ':
          case '\t':
          case '\r':
          case '\n':
            // whitespace breaks word
            cnf_directive_append(output, acc->dat, acc->len);
            state = STATE_BLANK;
            memset(acc->dat, 0, acc->cap);
            acc->len = 0;
            if ('\n' == c) {
              goto cnf_directive_read_finalize;
            }
            continue;
          default:
            // append char to word
            buf_append_byte(acc, c);
            continue;
        }
        break;
      default:
        fprintf(stderr, "Invalid state: %d\n", state);
        exit(1);
    }

  }

cnf_directive_read_finalize:
  if (acc->len) {
    cnf_directive_append(output, acc->dat, acc->len);
  }
  buf_clear(acc);
  free(acc);
  if (!output->name) {
    free(output);
    return NULL;
  }
  return output;
}
// }}}

#ifdef __cplusplus
} // extern "C"
#endif

// vim:fdm=marker:fdl=0
