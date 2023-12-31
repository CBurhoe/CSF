#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result;
  for (int i = 0; i < 8; ++i) {
      result.data[i] = 0U;
  }
  result.data[0] = val;
  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 3 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result;
  for (int i = 0; i < 8; ++i) {
    result.data[i] = 0U;
  }
  for (int i = 0; i < 8; ++i) {
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result;
  char *endptr;
  char buff[9];
  size_t hexLength = strlen(hex);
  size_t numWordsToFill = (hexLength / 8) * 8;
  size_t remainingHexBits = hexLength % 8;
  
  for (int i = 0; i < 8; ++i) {
    result.data[i] = 0U;
  }
  
  if (numWordsToFill){
    for (size_t i = 0; i < numWordsToFill; i += 8) {
      strncpy(buff, hex + i, 8);
      buff[8] = '\0';
      
      uint32_t decimalWord = strtoul(buff, &endptr, 16);
      result.data[i / 8] = decimalWord;
      if (i == 64) {return result;}
    }
  }
  if (remainingHexBits) {
    strncpy(buff, hex + numWordsToFill, remainingHexBits);
    buff[remainingHexBits] = '\0';
    uint32_t decimalWord = strtoul(buff, &endptr, 16);
    if (numWordsToFill) {
      result.data[numWordsToFill / 8 + 1] = decimalWord;
    }else{
      result.data[0] = decimalWord;
    }
  }
  
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = calloc(65, sizeof(char)); //num_words * hex_chars_per_word + \0
  uint32_t leadingZeroes = 1;
  hex[0] = '\0'; // Initialize the hex string with a null terminator
  
  for (int i = 7; i >= 0; --i) {
    if (val.data[i] == 0U && leadingZeroes) { continue; }
    if (leadingZeroes) {
      char hexWord[9];
      sprintf(hexWord, "%x", val.data[i]);
      strcat(hex, hexWord);
      leadingZeroes = 0;
    } else {
      char hexWord[9];
      sprintf(hexWord, "%08x", val.data[i]);
      strcat(hex, hexWord);
    }
  }
  if (!strlen(hex)) {
    strcat(hex, "0");
  }
  
  return hex;
}


// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 3 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits;
  bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 sum;
  uint32_t carry = 0U;
  for(int i = 0; i < 8; ++i) {
      uint64_t wordSum = (uint64_t)left.data[i] +  right.data[i] + carry;
      sum.data[i] = (uint32_t)wordSum;
      carry = (wordSum >> 32);
  }
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  for (int i = 0; i < 8; ++i) {
    result.data[i] = 0;
  }
  result = uint256_add(left, uint256_negate(right));
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  for (int i = 0;i < 8; ++i) {
    result.data[i] = 0U;
  }
  uint32_t carry = 1;
  
  for(int i = 0; i < 8; ++i) {
    uint64_t wordResult = (uint64_t)(~(val.data[i])) + carry;
    result.data[i] = (uint32_t)wordResult;
    carry = (wordResult >> 32);
  }
  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;
  unsigned nWordsShift = nbits / 32;
  unsigned nBitsShift = nbits % 32;
  
  //Rotate the result according to the number of whole word shifts
  for (int i = 0; i < 8; ++i) {
    if ((i + nWordsShift) > 7) {
      result.data[(i + nWordsShift) - 8] = (val.data[i] << nBitsShift) | (val.data[i - 1] >> (32 - nBitsShift));
      continue;
    }
    if (i == 0) {
      result.data[i + nWordsShift] = (val.data[i] << nBitsShift) | (val.data[i + 7] >> (32 - nBitsShift));
      
    }
    result.data[i + nWordsShift] = (val.data[i] << nBitsShift) | (val.data[i - 1] >> (32 - nBitsShift));
  }
  
  return result;
}

// Helper function to right rotate 32-bit words by nbits
uint32_t uint32_rotate_right(uint32_t val, unsigned nbits) {
  return (val >> nbits) | (val << (32 - nbits));
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;
  unsigned nWordsShift = nbits / 32U;
  unsigned nBitsShift = nbits % 32U;
  
  for (int i = 0; i < 8; ++i) {
    result.data[i] = 0U;
  }
  
  for (int i = 0; i < 8; ++i) {
    unsigned valWordIndex = (i - nWordsShift + 8) % 8;
    
    result.data[i] = uint32_rotate_right(val.data[valWordIndex], nBitsShift);
    result.data[i] |= val.data[(valWordIndex + 1) % 8] << (32 - nBitsShift);
  }
  
  return result;
}
