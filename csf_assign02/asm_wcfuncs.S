/*
 * Assembly language function implementations
 */

#define MAX_WORDLEN 63

/*
 * Offsets for the fields of the struct WordEntry data type
 * (Note that there is 4 bytes of padding between the count and next
 * fields.)
 */
#define WORDENTRY_WORD_OFFSET   (0)
#define WORDENTRY_COUNT_OFFSET  (MAX_WORDLEN+1)
#define WORDENTRY_NEXT_OFFSET   (MAX_WORDLEN+1+4+4)

	.section .rodata
/* Define any string constants or read-only data here */

	.section .text

/*
 * Compute a hash code for the given NUL-terminated
 * character string.
 *
 * The hash algorithm should be implemented as follows:
 *
 * uint32_t hash_code = 5381
 * for each character c of w in order {
 *   hash_code = hash_code * 33 + c
 * }
 *
 * Note that the character values should be treated as
 * being unsigned (in the range 0..255)
 *
 * C function prototype:
 *    uint32_t wc_hash(const unsigned char *w);
 */
	.globl wc_hash
wc_hash:

.equ HASH_CODE, 5381 //define constant for base hash code

    subq $8, %rsp //adjust stack pointer
    pushq %rbx //save value from rbx in next stack qword

.LhashCodeLoop:
    cmbp $0, (%rdi) //check if input char pointer at null terminator
    jz .LhashCodeDone //if yes, jump out of loop

.LhashCodeDone:

    movq $HASH_CODE, %rbx // load base hashcode const into register for calculating hashcode



	/* TODO: implement */
	popq %rbx //restore value saved in stack to rbx
	addq $8, %rsp //restore stack pointer
	ret

/*
 * Compare two strings lexicographically. Return
 *
 * - a negative value if lhs string is less than rhs string
 * - 0 if lhs string is identical to rhs string
 * - a positive value if lhs string is greater than rhs string
 *
 * Lexicographical comparison is a generalization of alphabetical
 * order, but using character codes. If one string is a prefix
 * of the other, it is considered as "less than". E.g.,
 * "hi" would compare as less than "high".
 *
 * C function prototype:
 *    int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs);
 */
	.globl wc_str_compare
wc_str_compare:
    subq $8, %rsp //adjust stack pointer
    movq $0, %rax //assume equal strings
    movq $0, %rbx //initialize secondary return register TODO: %rbx is callee-saved reg, need to save and restore value to use

.LcmpLoop:
    movb (%rdi), %al
    movb (%rsi), %bl
    cmpb %al, %bl //are current bytes equal?

    jne .LnotEqualStrings //if not, jump out of loop
    cmpb $0, %al //are pointers at a null terminator?
    jz .LequalStrings //if yes, strings are equal!

    inc %rdi //advance to next char of first string
    inc %rsi //advance to next char of second string
    jmp .LcmpLoop //jump to top of loop

.LequalStrings:
    movq $0, %rax //strings are equal
    addq $8, %rsp //adjust stack pointer
    ret

.LnotEqualStrings:
    movzbq %al, %rax //move byte with zero extension to return register
    movzbq %bl, %rbx //move byte with zero extension to secondary return register
    sub %rbx, %rax //compare return registers
    addq $8, %rsp //adjust stack pointer
	ret

/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
	.globl wc_str_copy
wc_str_copy:
	/* TODO: implement */
	ret

/*
 * Return 1 if the character code in c is a whitespace character,
 * false otherwise.
 *
 * For the purposes of this function, a whitespace character is one of
 *
 *   ' '
 *   '\t'
 *   '\r'
 *   '\n'
 *   '\f'
 *   '\v'
 *
 * C function prototype:
 *    int wc_isspace(unsigned char c);
 */
	.globl wc_isspace
wc_isspace:
    subq $8, %rsp //adjust stack pointer
    movq %rdi, %r10

    /*
    *%rdi - input char c
    */
    cmpb $9, (%r10) //compare input with \t
    jz .LisWhitespace //is tab, jump to success label
    cmpb $10, (%r10) // compare input with \n
    jz .LisWhitespace //is newline, jump to success label
    cmpb $11, (%r10) // compare input with \v
    jz .LisWhitespace //is vertical tab, jump to success label
    cmpb $12, (%r10) // compare input with \f
    jz .LisWhitespace //is form feed, jump to success label
    cmpb $13, (%r10) // compare input with \r
    jz .LisWhitespace //is carriage return, jump to success label
    cmpb $32, (%r10) // compare input with ' '
    jz .LisWhitespace //is space, jump to success label
    movq $0, %rax //no conditions met, load 0 for return
    addq $8, %rsp //adjust stack pointer
    ret //return result

.LisWhitespace:
    movq $1, %rax //condition was met, load 1 for return

    addq $8, %rsp //adjust stack pointer
    ret //return result
/*
 * Return 1 if the character code in c is an alphabetic character
 * ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
 *
 * C function prototype:
 *    int wc_isalpha(unsigned char c);
 */
	.globl wc_isalpha
wc_isalpha:
	subq $8, %rsp //adjust stack pointer

	/*
	*%rdi - char input c
	*/
	movb %dil, %al
	cmpb $65, %al //compare c with 'A'
	jl .LnotAlphabetical //fail case
	cmpb $90, %al //compare c with 'Z'
	jle .LisAlphabetical //success case

	cmpb $97, %al //compare c with 'a'
	jl .LnotAlphabetical //fail case
	cmpb $122, %al //compare c with 'z'
	jle .LisAlphabetical //success case

.LnotAlphabetical:
    movq $0, %rax //failed cases
    addq $8, %rsp //adjust stack pointer
	ret

.LisAlphabetical:
    movq $1, %rax //succeeded in a case
    addq $8, %rsp //adjust stack pointer
	ret


/*
 * Read the next word from given input stream, storing
 * the word in the array pointed-to by w. (This array should be
 * assumed to be MAX_WORDLEN+1 elements in size.) Return 1
 * if a word is read successfully, 0 otherwise.
 *
 * For the purposes of this function, a word is a sequence of
 * 1 or more non-whitespace characters.
 *
 * If a sequence of non-whitespace characters has more than
 * MAX_WORDLEN characters, then only the first MAX_WORDLEN
 * characters in the sequence should be stored in the array.
 *
 * C function prototype:
 *    int wc_readnext(FILE *in, unsigned char *w);
 */
	.globl wc_readnext
wc_readnext:
	/* TODO: implement */
	ret

/*
 * Convert the NUL-terminated character string in the array
 * pointed-to by w so that every letter is lower-case.
 *
 * C function prototype:
 *    void wc_tolower(unsigned char *w);
 */
	.globl wc_tolower
wc_tolower:
	/* TODO: implement */
	ret

/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */
	.globl wc_trim_non_alpha
wc_trim_non_alpha:
	/* TODO: implement */
	ret

/*
 * Search the specified linked list of WordEntry objects for an object
 * containing the specified string.
 *
 * If a matching object is found, set the int variable pointed-to by
 * inserted to 0 and return a pointer to the matching object.
 *
 * If a matching object is not found, allocate a new WordEntry object,
 * set its next pointer to point to what head points to (i.e., so the
 * new object is at the head of the list), set the variable pointed-to
 * by inserted to 1, and return a pointer to the new node. Note that
 * the new node should have its count value set to 0. (It is the caller's
 * job to update the count.)
 *
 * C function prototype:
 *    struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted);
 */
	.globl wc_find_or_insert
wc_find_or_insert:
	/* TODO: implement */
	ret

/*
 * Find or insert the WordEntry object for the given string (s), returning
 * a pointer to it. The head of the linked list which contains (or should
 * contain) the entry for s is the element of buckets whose index is the
 * hash code of s mod num_buckets.
 *
 * Returns a pointer to the WordEntry object in the appropriate linked list
 * which represents s.
 *
 * C function prototype:
 *    struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s);
 */
	.globl wc_dict_find_or_insert
wc_dict_find_or_insert:
	/* TODO: implement */
	ret

/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
	.globl wc_free_chain
wc_free_chain:
	/* TODO: implement */
	ret

/*
vim:ft=gas:
*/
