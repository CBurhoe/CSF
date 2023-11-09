#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int compare_i64(const void *left_, const void *right_) {
  int64_t left = *(int64_t *)left_;
  int64_t right = *(int64_t *)right_;
  if (left < right) return -1;
  if (left > right) return 1;
  return 0;
}

void seq_sort(int64_t *arr, size_t begin, size_t end) {
  size_t num_elements = end - begin;
  qsort(arr + begin, num_elements, sizeof(int64_t), compare_i64);
}

// Merge the elements in the sorted ranges [begin, mid) and [mid, end),
// copying the result into temparr.
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  int64_t *endl = arr + mid;
  int64_t *endr = arr + end;
  int64_t *left = arr + begin, *right = arr + mid, *dst = temparr;

  for (;;) {
    int at_end_l = left >= endl;
    int at_end_r = right >= endr;

    if (at_end_l && at_end_r) break;

    if (at_end_l)
      *dst++ = *right++;
    else if (at_end_r)
      *dst++ = *left++;
    else {
      int cmp = compare_i64(left, right);
      if (cmp <= 0)
        *dst++ = *left++;
      else
        *dst++ = *right++;
    }
  }
}

void fatal(const char *msg) __attribute__ ((noreturn));

void fatal(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  assert(end >= begin);
  size_t size = end - begin;

  if (size <= threshold) {
    seq_sort(arr, begin, end);
    return;
  }

  // recursively sort halves in parallel

  size_t mid = begin + size/2;

  // TODO: parallelize the recursive sorting
  
  // FIXME: fork() two times and have each child recursively sort their half of the array
  pid_t pidL = fork();
  pid_t pidR = fork();

  if (pidL == -1) {
    fprintf(stderr, "Error: failed to start a new process\n");
    exit(6);
  } else if (pidL == 0) {
    merge_sort(arr, begin, mid, threshold);
    exit(0);
  }
  
  if (pidR == -1) {
    fprintf(stderr, "Error: failed to start a new process\n");
    exit(6);
  } else if (pidR == 0) {
    merge_sort(arr, mid, end, threshold);
    exit(0);
  }
  
  int status1, status2;
  
  pid_t pid1 = waitpid(pidL, &status1, 0);
  pid_t pid2 = waitpid(pidR, &status2, 0);
  
  if (pid1 == -1) {
    fprintf(stderr, "Error: waitpid failed\n");
    exit(7);
  }
  if (pid2 == -1) {
    fprintf(stderr, "Error: waitpid failed\n");
    exit(7);
  }
  if (!WIFEXITED(status1) || !WIFEXITED(status2)) {
    fprintf(stderr, "Error: child process terminated abnormally\n");
    exit(8);
  } else if (WEXITSTATUS(status1) != 0 || WEXITSTATUS(status2) != 0) {
    fprintf(stderr, "Error: child process terminated abnormally\n");
    exit(9);
  }
  
  int64_t *temparr;
  merge(arr, begin, mid, end, temparr);
  for (int i = 0; i < end - begin; i++) {
    arr[i] = temparr[i];
  }
  
  /*
  merge_sort(arr, begin, mid, threshold);
  merge_sort(arr, mid, end, threshold);

  // allocate temp array now, so we can avoid unnecessary work
  // if the malloc fails
  int64_t *temp_arr = (int64_t *) malloc(size * sizeof(int64_t));
  if (temp_arr == NULL)
    fatal("malloc() failed");

  // child processes completed successfully, so in theory
  // we should be able to merge their results
  merge(arr, begin, mid, end, temp_arr);

  // copy data back to main array
  for (size_t i = 0; i < size; i++)
    arr[begin + i] = temp_arr[i];

  // now we can free the temp array
  free(temp_arr);

  // success!
  */
}

int main(int argc, char **argv) {
  // check for correct number of command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t) strtoul(argv[2], &end, 10);
  if (end != argv[2] + strlen(argv[2])) {
    // TODO: report an error (threshold value is invalid)
  }

  // TODO: open the file

  // TODO: use fstat to determine the size of the file

  // TODO: map the file into memory using mmap

  // TODO: sort the data!

  // TODO: unmap and close the file

  // TODO: exit with a 0 exit code if sort was successful
}
