#include "statdata.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int StoreDump(const char *filename, const StatData *data, size_t count) {
  FILE *file = fopen(filename, "wb");
  if (!file)
    return -1;

  size_t written = fwrite(data, sizeof(StatData), count, file);
  fclose(file);

  return written == count ? 0 : -1;
}

StatData *LoadDump(const char *filename, size_t *count) {
  FILE *file = fopen(filename, "rb");
  if (!file)
    return NULL;

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  *count = file_size / sizeof(StatData);
  StatData *data = malloc(file_size);
  if (!data) {
    fclose(file);
    return NULL;
  }

  size_t read = fread(data, sizeof(StatData), *count, file);
  fclose(file);

  if (read != *count) {
    free(data);
    return NULL;
  }

  return data;
}

static int compare_id(const void *a, const void *b) {
  const StatData *da = (const StatData *)a;
  const StatData *db = (const StatData *)b;
  return (da->id > db->id) - (da->id < db->id);
}

static int compare_cost(const void *a, const void *b) {
  const StatData *da = (const StatData *)a;
  const StatData *db = (const StatData *)b;

  if (da->cost < db->cost)
    return -1;
  if (da->cost > db->cost)
    return 1;
  return 0;
}

StatData *JoinDump(const StatData *data1, size_t count1, const StatData *data2,
                   size_t count2, size_t *result_count) {
  StatData *combined = malloc((count1 + count2) * sizeof(StatData));
  if (!combined)
    return NULL;

  memcpy(combined, data1, count1 * sizeof(StatData));
  memcpy(combined + count1, data2, count2 * sizeof(StatData));

  qsort(combined, count1 + count2, sizeof(StatData), compare_id);

  size_t unique_count = 0;
  for (size_t i = 0; i < count1 + count2;) {
    size_t j = i + 1;
    while (j < count1 + count2 && combined[j].id == combined[i].id)
      j++;

    StatData merged = combined[i];
    for (size_t k = i + 1; k < j; k++) {
      merged.count += combined[k].count;
      merged.cost += combined[k].cost;
      merged.primary &= combined[k].primary;
      if (combined[k].mode > merged.mode) {
        merged.mode = combined[k].mode;
      }
    }

    combined[unique_count++] = merged;
    i = j;
  }

  StatData *result = realloc(combined, unique_count * sizeof(StatData));
  if (!result) {
    free(combined);
    return NULL;
  }

  *result_count = unique_count;
  return result;
}

void SortDump(StatData *data, size_t count) {
  qsort(data, count, sizeof(StatData), compare_cost);
}

void PrintStatData(const StatData *data, size_t count, size_t limit) {
  if (limit == 0 || limit > count)
    limit = count;

  printf("%-10s %-10s %-15s %-8s %-5s\n", "ID", "Count", "Cost", "Primary",
         "Mode");
  printf("--------------------------------------------\n");

  for (size_t i = 0; i < limit; i++) {
    printf("%-10lx %-10d %-15.3e %-8c %d%d%d\n", data[i].id, data[i].count,
           data[i].cost, data[i].primary ? 'y' : 'n',
           (data[i].mode & 4) ? 1 : 0, (data[i].mode & 2) ? 1 : 0,
           (data[i].mode & 1) ? 1 : 0);
  }
}