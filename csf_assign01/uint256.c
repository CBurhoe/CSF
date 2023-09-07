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
  // TODO: implement
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = NULL;
  // TODO: implement
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
  uint32_t overflow;
  uint32_t space_check;
  for(int i = 0; i < 8; ++i) {
      space_check = ~(0U) - left.data[i];
      if(space_check < right.data[i]) {
          sum.data[i] = (right.data[i] - space_check) + overflow;
          overflow = 1U;
          continue;
      }
      sum.data[i] = left.data[i] + right.data[i] + overflow;
      overflow = 0U;
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
  uint32_t carry = 1;
  for(int i = 0; i < 8; ++i) {
      val.data[i] = ~(val.data[i]) + carry;
      carry = val.data[i] == 0;
  }
  return result;
}

// Helper function to left rotate 32-bit words by nbits
uint32_t uint32_rotate_left(uint32_t val, unsigned nbits) {
  return (val << nbits) | (val >> (32 - nbits)); // Left shift by nbits and wrap most
                                                 // significant bits into the least significant bits
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;
  unsigned nWordsShift = nbits / 32U;
  unsigned nBitsShift = nbits % 32U;
  
  // Initialize all bits to zero
  for (int i = 0; i < 8; ++i) {
    result.data[i] = 0U;
  }
  
  for (int i = 0; i < 8; ++i) {
    // Find the number of words that the source index (val) is offset from the result index
    unsigned valWordIndex = (i - nWordsShift + 8) % 8;
    
    // Rotate the bits within each word
    result.data[i] = uint32_rotate_left(val.data[valWordIndex], nBitsShift);
    
    // Wrap the bits shifted out of each word into the next word
    result.data[i] |= val.data[(valWordIndex + 1) % 8] >> (32 - nBitsShift);
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
