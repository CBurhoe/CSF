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
    
    
    size_t mid = (begin + end) / 2;
    
    // fork() two times and have each child recursively sort their half of the array
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
    
    int wstatus1, wstatus2;
    
    pid_t actualPidL = waitpid(pidL, &wstatus1, 0);
    pid_t actualPidR = waitpid(pidR, &wstatus2, 0);
    
    if (actualPidL == -1 || actualPidR == -1) {
      fprintf(stderr, "Error: waitpid failed\n");
      exit(7);
    }
    if (!WIFEXITED(wstatus1) || !WIFEXITED(wstatus2)) {
      fprintf(stderr, "Error: child process terminated abnormally\n");
      exit(8);
    }
    if (WEXITSTATUS(wstatus1) != 0 || WEXITSTATUS(wstatus2) != 0) {
      fprintf(stderr, "Error: child process terminated abnormally\n");
      exit(9);
    }
    int64_t *temparr = (int64_t *)malloc(size * sizeof(int64_t));
    if (temparr == NULL) {
      fatal("malloc() failed");
    }
    merge(arr, begin, mid, end, temparr);
    
    // Copy merged data back to the main array
    for (size_t i = 0; i < size; i++) {
      arr[begin + i] = temparr[i];
    }
    free(temparr);
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
    fprintf(stderr, "Error: threshold value is invalid\n");
    exit(1);
  }

  // open the file
  int fd = open(filename, O_RDWR);
  if (fd < 1) {
    fprintf(stderr, "Error: failed to open file\n");
    exit(2);
  }
  // use fstat to determine the size of the file
  struct stat statbuf;
  int rc = fstat(fd, &statbuf);
  if (rc != 0) {
    fprintf(stderr, "Error: fstat error\n");
    close(fd);
    exit(3);
  }
  size_t file_size_in_bytes = statbuf.st_size;
  // map the file into memory using mmap
  int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    fprintf(stderr, "Error: mmap failure\n");
    exit(4);
  }
  // sort the data!
  merge_sort(data, 0, file_size_in_bytes / 8, threshold);
  // unmap and close the file
  if (munmap(data, file_size_in_bytes)) {
    fprintf(stderr, "Error: munmap failure\n");
    exit(5);
  }
  close(fd);
  // exit with a 0 exit code if sort was successful
  exit(0);
}
