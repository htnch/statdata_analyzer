#include "statdata.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <file1> <file2> <output_file>\n", argv[0]);
    return 1;
  }

  clock_t start = clock();

  size_t count1, count2;
  StatData *data1 = LoadDump(argv[1], &count1);
  StatData *data2 = LoadDump(argv[2], &count2);

  if (!data1 || !data2) {
    fprintf(stderr, "Error loading input files\n");
    free(data1);
    free(data2);
    return 1;
  }

  size_t result_count;
  StatData *result = JoinDump(data1, count1, data2, count2, &result_count);
  free(data1);
  free(data2);

  if (!result) {
    fprintf(stderr, "Error joining data\n");
    return 1;
  }

  SortDump(result, result_count);
  PrintStatData(result, result_count, 10);

  if (StoreDump(argv[3], result, result_count) != 0) {
    fprintf(stderr, "Error saving output file\n");
    free(result);
    return 1;
  }

  free(result);

  clock_t end = clock();
  double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
  printf("\nProcessing completed in %.3f seconds\n", elapsed);

  return 0;
}
