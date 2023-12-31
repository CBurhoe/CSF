#include <stdio.h>
#include <stdint.h>
#include "wcfuncs.h"
#include <stdlib.h>

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249

// TODO: prototypes for helper functions

int main(int argc, char **argv) {
  // stats (to be printed at end)
  uint32_t total_words = 0;
  uint32_t unique_words = 0;
  const unsigned char *best_word = (const unsigned char *) "";
  uint32_t best_word_count = 0;

  FILE *input_stream;
  
  // TODO: implement
  if (argc == 2) {
    input_stream = fopen(argv[1], "r");
    if (input_stream == NULL) {
      fprintf(stderr, "Could not open file.\n");
      return 1;
    }
  } else {
    input_stream = stdin;
  }
  struct WordEntry **hash_table = (struct WordEntry **)malloc(HASHTABLE_SIZE * sizeof(struct WordEntry));
  unsigned char word[MAX_WORDLEN + 1];
  while (wc_readnext(input_stream, word)) {
    ++total_words;
    
    /*
  use wc_tolower to convert word to lower case

  use wc_trim_non_alpha to remove non-alphabetic characters at end of word

  use wc_find_or_insert to find or insert the word in the hash table

  increment the WordEntry's count
     */
    wc_tolower(word);
    wc_trim_non_alpha(word);
    struct WordEntry *new_entry = wc_dict_find_or_insert(hash_table, HASHTABLE_SIZE, word);
    if (new_entry->count == 0) { ++unique_words; }
    new_entry->count++;
  }
  
  struct WordEntry *most_frequent_word = *hash_table;
  for (int i = 0; i < HASHTABLE_SIZE; ++i) {
    if (hash_table[i]->count > most_frequent_word->count) {
      most_frequent_word = hash_table[i];
      continue;
    }
    if (hash_table[i]->count < most_frequent_word->count) {
      continue;
    }
    if (wc_str_compare(hash_table[i]->word, most_frequent_word->word) == -1) {
      most_frequent_word = hash_table[i];
    }
  }
  best_word = most_frequent_word->word;
  best_word_count = most_frequent_word->count;
  

  printf("Total words read: %u\n", (unsigned int) total_words);
  printf("Unique words read: %u\n", (unsigned int) unique_words);
  printf("Most frequent word: %s (%u)\n", (const char *) best_word, best_word_count);

  // TODO: make sure file is closed (if one was opened)
  fclose(input_stream);
  // TODO: make sure memory is freed
  for (int i = 0; i < HASHTABLE_SIZE; ++i) {
    wc_free_chain(hash_table[i]);
  }
  free(hash_table);

  return 0;
}

// TODO: definitions of helper functions
