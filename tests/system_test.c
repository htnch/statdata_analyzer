#include "../include/statdata.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define TEST_INPUT_FILE1 "test_input1.bin"
#define TEST_INPUT_FILE2 "test_input2.bin"
#define TEST_OUTPUT_FILE "test_output.bin"

typedef struct {
  const char *name;
  const StatData *input1;
  size_t input1_size;
  const StatData *input2;
  size_t input2_size;
  const StatData *expected;
  size_t expected_size;
} TestCase;

const TestCase system_tests[] = {
    {"System test: basic merge and sort",
     (const StatData[]){
         {.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3},
         {.id = 90089, .count = 1, .cost = 88.90, .primary = 1, .mode = 0}},
     2,
     (const StatData[]){
         {.id = 90089, .count = 13, .cost = 0.011, .primary = 0, .mode = 2},
         {.id = 90189,
          .count = 1000,
          .cost = 1.00003,
          .primary = 1,
          .mode = 2}},
     2,
     (const StatData[]){
         {.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2},
         {.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3},
         {.id = 90089, .count = 14, .cost = 88.911, .primary = 0, .mode = 2}},
     3}};

const size_t num_system_tests = sizeof(system_tests) / sizeof(TestCase);

void create_test_files(const TestCase *test) {
  StoreDump(TEST_INPUT_FILE1, test->input1, test->input1_size);
  StoreDump(TEST_INPUT_FILE2, test->input2, test->input2_size);
}

int compare_results(const StatData *actual, size_t actual_size,
                    const StatData *expected, size_t expected_size) {
  if (actual_size != expected_size) {
    fprintf(stderr, "Size mismatch: expected %zu, got %zu\n", expected_size,
            actual_size);
    return 0;
  }

  for (size_t i = 0; i < actual_size; i++) {
    if (actual[i].id != expected[i].id ||
        actual[i].count != expected[i].count ||
        fabs(actual[i].cost - expected[i].cost) > 0.001 ||
        actual[i].primary != expected[i].primary ||
        actual[i].mode != expected[i].mode) {
      fprintf(stderr, "Mismatch at record %zu:\n", i);
      fprintf(stderr,
              "  Expected: id=%ld, count=%d, cost=%.3f, primary=%d, mode=%d\n",
              expected[i].id, expected[i].count, expected[i].cost,
              expected[i].primary, expected[i].mode);
      fprintf(stderr,
              "  Actual:   id=%ld, count=%d, cost=%.3f, primary=%d, mode=%d\n",
              actual[i].id, actual[i].count, actual[i].cost, actual[i].primary,
              actual[i].mode);
      return 0;
    }
  }
  return 1;
}

void run_system_test(const TestCase *test) {
  printf("Running %s\n", test->name);
  clock_t start = clock();

  create_test_files(test);

  pid_t pid = fork();
  if (pid == 0) {
    execl("./../statdata_analyzer", "./../statdata_analyzer", TEST_INPUT_FILE1,
          TEST_INPUT_FILE2, TEST_OUTPUT_FILE, NULL);
    perror("execl failed");
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
      size_t result_size;
      StatData *result = LoadDump(TEST_OUTPUT_FILE, &result_size);

      if (result) {
        if (compare_results(result, result_size, test->expected,
                            test->expected_size)) {
          clock_t end = clock();
          double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
          printf("  PASSED (%.3f seconds)\n", elapsed);
        }
        free(result);
      }
    }
  }

  unlink(TEST_INPUT_FILE1);
  unlink(TEST_INPUT_FILE2);
  unlink(TEST_OUTPUT_FILE);
}

void RunSystemTests() {
  printf("\nRunning system tests...\n");
  for (size_t i = 0; i < num_system_tests; i++) {
    run_system_test(&system_tests[i]);
  }
}

int main() {
  RunSystemTests();
  return 0;
}
