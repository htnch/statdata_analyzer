#include "../include/statdata.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ASSERT(condition, message)                                             \
  do {                                                                         \
    if (!(condition)) {                                                        \
      fprintf(stderr, "FAIL: %s\n", message);                                  \
      return 0;                                                                \
    }                                                                          \
  } while (0)

#define RUN_TEST(test)                                                         \
  do {                                                                         \
    printf("Running %-60s", #test);                                            \
    clock_t start = clock();                                                   \
    int result = test();                                                       \
    clock_t end = clock();                                                     \
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;                   \
    if (result) {                                                              \
      printf("PASS (%.3f sec)\n", elapsed);                                    \
      passed++;                                                                \
    } else {                                                                   \
      failed++;                                                                \
    }                                                                          \
    total++;                                                                   \
  } while (0)

// Test cases
int test_store_load_empty() {
  const char *filename = "empty_test.bin";
  StatData empty = {0};

  // Store empty data
  ASSERT(StoreDump(filename, &empty, 1) == 0, "Store empty failed");

  // Load empty data
  size_t count = 0;
  StatData *loaded = LoadDump(filename, &count);
  ASSERT(loaded != NULL, "Load empty failed");
  ASSERT(count == 1, "Empty count mismatch");
  ASSERT(memcmp(&empty, loaded, sizeof(StatData)) == 0, "Empty data mismatch");

  free(loaded);
  remove(filename);
  return 1;
}

int test_store_load_multiple() {
  const char *filename = "multi_test.bin";
  const StatData data[] = {
      {.id = 1, .count = 10, .cost = 1.5, .primary = 1, .mode = 3},
      {.id = 2, .count = 20, .cost = 2.5, .primary = 0, .mode = 5},
      {.id = 3, .count = 30, .cost = 3.5, .primary = 1, .mode = 7}};
  const size_t data_count = sizeof(data) / sizeof(StatData);

  ASSERT(StoreDump(filename, data, data_count) == 0, "Store multiple failed");

  size_t loaded_count = 0;
  StatData *loaded = LoadDump(filename, &loaded_count);
  ASSERT(loaded != NULL, "Load multiple failed");
  ASSERT(loaded_count == data_count, "Multiple count mismatch");

  for (size_t i = 0; i < data_count; i++) {
    ASSERT(memcmp(&data[i], &loaded[i], sizeof(StatData)) == 0,
           "Multiple data mismatch");
  }

  free(loaded);
  remove(filename);
  return 1;
}

int test_join_basic() {
  const StatData data1[] = {
      {.id = 1, .count = 10, .cost = 1.0, .primary = 1, .mode = 2}};
  const StatData data2[] = {
      {.id = 2, .count = 20, .cost = 2.0, .primary = 0, .mode = 3}};

  size_t result_count = 0;
  StatData *result = JoinDump(data1, 1, data2, 1, &result_count);

  ASSERT(result != NULL, "Join basic failed");
  ASSERT(result_count == 2, "Join basic count mismatch");

  free(result);
  return 1;
}

int test_join_duplicates() {
  const StatData data1[] = {
      {.id = 1, .count = 10, .cost = 1.0, .primary = 1, .mode = 2},
      {.id = 2, .count = 20, .cost = 2.0, .primary = 1, .mode = 3}};
  const StatData data2[] = {
      {.id = 1, .count = 5, .cost = 0.5, .primary = 0, .mode = 4},
      {.id = 3, .count = 30, .cost = 3.0, .primary = 0, .mode = 1}};

  size_t result_count = 0;
  StatData *result = JoinDump(data1, 2, data2, 2, &result_count);

  ASSERT(result != NULL, "Join duplicates failed");
  ASSERT(result_count == 3, "Join duplicates count mismatch");

  // Verify merged record (id=1)
  StatData merged = {0};
  int found = 0;
  for (size_t i = 0; i < result_count; i++) {
    if (result[i].id == 1) {
      merged = result[i];
      found = 1;
      break;
    }
  }

  ASSERT(found, "Merged record not found");
  ASSERT(merged.count == 15, "Count merge failed");
  ASSERT(fabs(merged.cost - 1.5) < 0.001, "Cost merge failed");
  ASSERT(merged.primary == 0, "Primary merge failed");
  ASSERT(merged.mode == 4, "Mode merge failed");

  free(result);
  return 1;
}

int test_sort_basic() {
  StatData data[] = {
      {.id = 1, .cost = 3.0}, {.id = 2, .cost = 1.0}, {.id = 3, .cost = 2.0}};
  const size_t count = sizeof(data) / sizeof(StatData);

  SortDump(data, count);

  for (size_t i = 1; i < count; i++) {
    ASSERT(data[i - 1].cost <= data[i].cost, "Sort order incorrect");
  }
  return 1;
}

int test_bitfield_operations() {
  StatData data = {0};

  // Test primary bit (1 bit)
  data.primary = 1;
  ASSERT(data.primary == 1, "Primary bit set failed");
  data.primary = 0;
  ASSERT(data.primary == 0, "Primary bit clear failed");

  // Test mode bits (3 bits)
  for (unsigned int i = 0; i < 8; i++) {
    data.mode = i;
    ASSERT(data.mode == i, "Mode bits operation failed");
  }

  return 1;
}

void RunUnitTests() {
  int total = 0, passed = 0, failed = 0;

  printf("\nStarting unit tests...\n");

  RUN_TEST(test_store_load_empty);
  RUN_TEST(test_store_load_multiple);
  RUN_TEST(test_join_basic);
  RUN_TEST(test_join_duplicates);
  RUN_TEST(test_sort_basic);
  RUN_TEST(test_bitfield_operations);

  printf("\nUnit tests summary:\n");
  printf("  Total:  %d\n", total);
  printf("  Passed: %d\n", passed);
  printf("  Failed: %d\n", failed);

  if (failed == 0) {
    printf("\nALL UNIT TESTS PASSED SUCCESSFULLY!\n");
  } else {
    printf("\nSOME UNIT TESTS FAILED!\n");
  }
}

int main() {
  RunUnitTests();
  return 0;
}
