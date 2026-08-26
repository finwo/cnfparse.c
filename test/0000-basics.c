#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "cnfparse.h"

static FILE *open_string(const char *text) {
  return fmemopen((void *)text, strlen(text), "r");
}

void test_empty_file() {
  FILE *fd = open_string("");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("empty file returns NULL", dir == NULL);
  fclose(fd);
}

void test_blank_lines_only() {
  FILE *fd = open_string("\n\n   \t\r\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("blank lines return NULL", dir == NULL);
  fclose(fd);
}

void test_comment_only_line() {
  FILE *fd = open_string("# this is a comment line\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("comment-only line returns NULL", dir == NULL);
  fclose(fd);
}

void test_single_word_no_args() {
  FILE *fd = open_string("directive\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("non-empty directive returned", dir != NULL);
  ASSERT_STRING_EQUALS("directive", dir->name);
  ASSERT_EQUALS(0, dir->argc);
  cnf_directive_free(dir);
  fclose(fd);
}

void test_leading_whitespace() {
  FILE *fd = open_string("   \t leading_spaces\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("directive with leading whitespace returned", dir != NULL);
  ASSERT_STRING_EQUALS("leading_spaces", dir->name);
  ASSERT_EQUALS(0, dir->argc);
  cnf_directive_free(dir);
  fclose(fd);
}

void test_trailing_whitespace() {
  FILE *fd = open_string("trailing   \t\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("directive with trailing whitespace returned", dir != NULL);
  ASSERT_STRING_EQUALS("trailing", dir->name);
  ASSERT_EQUALS(0, dir->argc);
  cnf_directive_free(dir);
  fclose(fd);
}

void test_multiple_space_separated_args() {
  FILE *fd = open_string("add 10.0.0.0/8 via 192.168.1.1\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("directive with args returned", dir != NULL);
  ASSERT_STRING_EQUALS("add", dir->name);
  ASSERT_EQUALS(3, dir->argc);
  ASSERT_STRING_EQUALS("10.0.0.0/8", dir->argv[0]);
  ASSERT_STRING_EQUALS("via", dir->argv[1]);
  ASSERT_STRING_EQUALS("192.168.1.1", dir->argv[2]);
  cnf_directive_free(dir);
  fclose(fd);
}

void test_multiple_spaces_between_args() {
  FILE *fd = open_string("   del    10.1.0.0/16\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("directive with extra spaces returned", dir != NULL);
  ASSERT_STRING_EQUALS("del", dir->name);
  ASSERT_EQUALS(1, dir->argc);
  ASSERT_STRING_EQUALS("10.1.0.0/16", dir->argv[0]);
  cnf_directive_free(dir);
  fclose(fd);
}

void test_quoted_double_args() {
  FILE *fd = open_string("set note \"hello world\" and \"foo bar\"\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("directive with quoted args returned", dir != NULL);
  ASSERT_STRING_EQUALS("set", dir->name);
  ASSERT_EQUALS(4, dir->argc);
  ASSERT_STRING_EQUALS("note", dir->argv[0]);
  ASSERT_STRING_EQUALS("hello world", dir->argv[1]);
  ASSERT_STRING_EQUALS("and", dir->argv[2]);
  ASSERT_STRING_EQUALS("foo bar", dir->argv[3]);
  fclose(fd);
}

void test_quoted_single_args() {
  FILE *fd = open_string("op 'single quoted'\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("directive with single-quoted arg returned", dir != NULL);
  ASSERT_STRING_EQUALS("op", dir->name);
  ASSERT_EQUALS(1, dir->argc);
  ASSERT_STRING_EQUALS("single quoted", dir->argv[0]);
  cnf_directive_free(dir);
  fclose(fd);
}

void test_escaped_quote_in_double() {
  FILE *fd = open_string("op arg \"a \\\" b\"\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("directive with escaped quote returned", dir != NULL);
  ASSERT_STRING_EQUALS("op", dir->name);
  ASSERT_EQUALS(2, dir->argc);
  ASSERT_STRING_EQUALS("arg", dir->argv[0]);
  ASSERT_STRING_EQUALS("a \" b", dir->argv[1]);
  cnf_directive_free(dir);
  fclose(fd);
}

void test_hash_in_quoted_string() {
  FILE *fd = open_string("set note \"a # b\"\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("directive with hash in quotes returned", dir != NULL);
  ASSERT_STRING_EQUALS("set", dir->name);
  ASSERT_EQUALS(2, dir->argc);
  ASSERT_STRING_EQUALS("note", dir->argv[0]);
  ASSERT_STRING_EQUALS("a # b", dir->argv[1]);
  cnf_directive_free(dir);
  fclose(fd);
}

void test_trailing_comment() {
  FILE *fd = open_string("op arg # trailing comment\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("directive before trailing comment returned", dir != NULL);
  ASSERT_STRING_EQUALS("op", dir->name);
  ASSERT_EQUALS(1, dir->argc);
  ASSERT_STRING_EQUALS("arg", dir->argv[0]);
  cnf_directive_free(dir);
  fclose(fd);
}

void test_comment_no_space() {
  FILE *fd = open_string("route 172.16.0.0/12 #comment with no space\n");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("directive before tight comment returned", dir != NULL);
  ASSERT_STRING_EQUALS("route", dir->name);
  ASSERT_EQUALS(1, dir->argc);
  ASSERT_STRING_EQUALS("172.16.0.0/12", dir->argv[0]);
  cnf_directive_free(dir);
  fclose(fd);
}

void test_multiple_directives_sequence() {
  FILE *fd = open_string("first one\nsecond two three\n");
  ASSERT("fmemopen succeeds", fd != NULL);

  struct cnf_directive *d1 = cnf_directive_read(fd);
  ASSERT("first directive returned", d1 != NULL);
  ASSERT_STRING_EQUALS("first", d1->name);
  ASSERT_EQUALS(1, d1->argc);
  ASSERT_STRING_EQUALS("one", d1->argv[0]);
  cnf_directive_free(d1);

  struct cnf_directive *d2 = cnf_directive_read(fd);
  ASSERT("second directive returned", d2 != NULL);
  ASSERT_STRING_EQUALS("second", d2->name);
  ASSERT_EQUALS(2, d2->argc);
  ASSERT_STRING_EQUALS("two", d2->argv[0]);
  ASSERT_STRING_EQUALS("three", d2->argv[1]);
  cnf_directive_free(d2);

  struct cnf_directive *d3 = cnf_directive_read(fd);
  ASSERT("exhausted file returns NULL", d3 == NULL);
  fclose(fd);
}

void test_blank_lines_between_directives() {
  FILE *fd = open_string("\n\nfirst arg\n\n\nsecond\n");
  ASSERT("fmemopen succeeds", fd != NULL);

  struct cnf_directive *d1 = cnf_directive_read(fd);
  ASSERT("directive after blank lines returned", d1 != NULL);
  ASSERT_STRING_EQUALS("first", d1->name);
  ASSERT_EQUALS(1, d1->argc);
  cnf_directive_free(d1);

  struct cnf_directive *d2 = cnf_directive_read(fd);
  ASSERT("directive after more blank lines returned", d2 != NULL);
  ASSERT_STRING_EQUALS("second", d2->name);
  ASSERT_EQUALS(0, d2->argc);
  cnf_directive_free(d2);

  fclose(fd);
}

void test_no_trailing_newline() {
  FILE *fd = open_string("final one two three");
  ASSERT("fmemopen succeeds", fd != NULL);
  struct cnf_directive *dir = cnf_directive_read(fd);
  ASSERT("directive without trailing newline returned", dir != NULL);
  ASSERT_STRING_EQUALS("final", dir->name);
  ASSERT_EQUALS(3, dir->argc);
  ASSERT_STRING_EQUALS("one", dir->argv[0]);
  ASSERT_STRING_EQUALS("two", dir->argv[1]);
  ASSERT_STRING_EQUALS("three", dir->argv[2]);
  cnf_directive_free(dir);
  fclose(fd);
}

void test_directive_free_null() {
  cnf_directive_free(NULL);
  ASSERT("free NULL does not crash", 1);
}

int main(int argc, const char *argv[]) {
  RUN(test_empty_file);
  RUN(test_blank_lines_only);
  RUN(test_comment_only_line);
  RUN(test_single_word_no_args);
  RUN(test_leading_whitespace);
  RUN(test_trailing_whitespace);
  RUN(test_multiple_space_separated_args);
  RUN(test_multiple_spaces_between_args);
  RUN(test_quoted_double_args);
  RUN(test_quoted_single_args);
  RUN(test_escaped_quote_in_double);
  RUN(test_hash_in_quoted_string);
  RUN(test_trailing_comment);
  RUN(test_comment_no_space);
  RUN(test_multiple_directives_sequence);
  RUN(test_blank_lines_between_directives);
  RUN(test_no_trailing_newline);
  RUN(test_directive_free_null);
  return TEST_REPORT();
}
