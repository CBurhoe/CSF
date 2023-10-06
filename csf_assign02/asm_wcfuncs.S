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
#define WORDENTRY_SIZE          (WORDENTRY_NEXT_OFFSET+8)

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

 /*VERIFIED*/
	.globl wc_hash
wc_hash:
    subq $8, %rsp //adjust stack pointer
    pushq %rbx //save value from rbx in next stack qword
    movq $5381, %rbx // load base hashcode const into register for calculating hashcode

.LhashCodeLoop:
    cmpb $0, (%rdi) //check if input char pointer at null terminator
    jz .LhashCodeDone //if yes, jump out of loop
    movq %rbx, %rax //move hash value into return register
    movq $33, %r8 //insert value into register
    mulq %r8 //multiply previous hashcode val by 33
    movq %rax, %rbx //move hash value back into rbx
    movzbq (%rdi), %r8 //move character into register
    addq %r8, %rbx //add character code to hashcode
    inc %rdi //advance to next character
    jmp .LhashCodeLoop //continue loop

.LhashCodeDone:
    movq %rbx, %rax //load hashcode into return register
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

 /*VERIFIED*/
	.globl wc_str_compare
wc_str_compare:
    subq $8, %rsp //adjust stack pointer

.LcmpLoop:
    movb (%rdi), %al
    movb (%rsi), %bl
    cmpb %al, %bl //are current bytes equal?

    jl .LleftGreater //lhs is greater
    jg .LrightGreater //rhs is greater

.LequalSoFar:
    cmpb $0, %al //are pointers at a null terminator?
    jz .LequalStrings //strings are fully equal
    inc %rdi //advance to next char of first string
    inc %rsi //advance to next char of second string
    jmp .LcmpLoop //jump back to loop

.LequalStrings:
    movq $0, %rax //strings are equal
    jmp .Lfinished //jump to end

.LleftGreater:
    movq $1, %rax //positive value return
    jmp .Lfinished //jump to end

.LrightGreater:
    movq $-1, %rax //negative value return
    jmp .Lfinished //jump to end

.Lfinished:
    addq $8, %rsp //adjust stack pointer
	ret

/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */

 /*VERIFIED*/
	.globl wc_str_copy
wc_str_copy:
	/* TODO: implement */
	subq $8, %rsp //adjust stack pointer
	pushq %r12 //store callee saved register value
	pushq %r13 //store callee saved register value
	movq %rdi, %r12 //move destination buffer to r12
	movq %rsi, %r13 //move source string to r13

.LcopyLoop:
    cmpb $0, (%r13) //check if source char is null terminator
    jz .LcopyDone //if yes, jump out of loop
    movb (%r13), %r8b //move byte into lower byte register
    movb %r8b, (%r12) //move source bit into destination bit
    inc %r13 //advance source pointer
    inc %r12 //advance destination pointer
    movb $0, (%r12) //put a null terminator on the end of string
    jmp .LcopyLoop //continue loop

.LcopyDone:
    popq %r13 //restore callee stored register
    popq %r12 //restore callee stored register
    addq $8, %rsp //adjust stack pointer
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

 /*VERIFIED*/
	.globl wc_isspace
wc_isspace:
    subq $8, %rsp //adjust stack pointer

    cmpb $9, %dil //compare input with \t
    jz .LisWhitespace //is tab, jump to success label
    cmpb $10, %dil // compare input with \n
    jz .LisWhitespace //is newline, jump to success label
    cmpb $11, %dil // compare input with \v
    jz .LisWhitespace //is vertical tab, jump to success label
    cmpb $12, %dil // compare input with \f
    jz .LisWhitespace //is form feed, jump to success label
    cmpb $13, %dil // compare input with \r
    jz .LisWhitespace //is carriage return, jump to success label
    cmpb $32, %dil // compare input with ' '
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

 /*VERIFIED*/
	.globl wc_isalpha
wc_isalpha:
	subq $8, %rsp //adjust stack pointer

	/*
	*%rdi - char input c
	*/
	cmpb $65, %dil //compare c with 'A'
	jl .LnotAlphabetical //fail case
	cmpb $90, %dil //compare c with 'Z'
	jle .LisAlphabetical //success case

	cmpb $97, %dil //compare c with 'a'
	jl .LnotAlphabetical //fail case
	cmpb $122, %dil //compare c with 'z'
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

 /*VERIFIED*/
	.globl wc_readnext
wc_readnext: //TODO: implement correctly
    subq $8, %rsp //adjust stack pointer
    pushq %r12 //store value from caller saved register
    pushq %r13 //store value from caller saved register
    pushq %r14 //store value from caller saved register
    pushq %r15 //store value from caller saved register
    movq $0, %r12 //initialize to 0
    movq %rdi, %r13 //store input argument to callee saved register
    movq $0, %r14 //initialize counter to 0
    movq %rsi, %r15 //put buffer address into r15


.LreadInLoopOne:
    movq %r13, %rdi //put stream pointer into argument register
    call fgetc //get next char in stream
    movb %al, %r12b //move char into callee saved register
    cmpb $-1, %r12b //check if reached EOF
    jz .LreadInDone //stream ended
    movb %r12b, %dil //move char into arg1 register
    call wc_isspace //check if char is space
    cmpq $1, %rax //check result
    jz .LreadInLoopOne //still whitespace, continue loop 1
    movb %r12b, (%r15) //put first char into buffer
    inc %r14 //char read in
    inc %r15 //advance buffer pointer
    movb $0, (%r15) //set last char to null terminator

.LreadInLoopTwo:
    movq %r13, %rdi //put stream pointer into argument register
    call fgetc //get next char in stream
    movb %al, %r12b //move char into callee saved register
    cmpb $-1, %r12b //check if reached EOF
    jz .LreadInDone //stream ended
    movb %r12b, %dil //move char into arg1 register
    call wc_isspace //check if char is space
    cmpq $1, %rax //check result
    jz .LreadInDone //found whitespace, end of string
    movb %r12b, (%r15) //move char into buffer
    inc %r14 //char read in
    inc %r15 //advance buffer pointer
    movb $0, (%r15) //set last char to null terminator
    cmpq $MAX_WORDLEN, %r14 //check if number of chars read is at max length
    jge .LreadInDone
    jmp .LreadInLoopTwo //continue loop

.LreadInDone:
    cmpq $0, %r14 //check if no chars read in
    jg .Lsuccess //if greater than 0, chars were read in
    movq $0, %rax //load fail case into return register
    jmp .Lend //finished procedure

.Lsuccess:
    movq $1, %rax //load success case into return register

.Lend:
    popq %r15 //restore caller saved value to register
    popq %r14 //restore caller saved value to register
    popq %r13 //restore caller saved value to register
    popq %r12 //restore caller saved value to register
    addq $8, %rsp //restore stack pointer
	ret

/*
 * Convert the NUL-terminated character string in the array
 * pointed-to by w so that every letter is lower-case.
 *
 * C function prototype:
 *    void wc_tolower(unsigned char *w);
 */

 /*VERIFIED*/
	.globl wc_tolower
wc_tolower:
	subq $8, %rsp //adjust stack pointer
	movq %rdi, %r10 //move input into caller saved register

.LconvertCharsLoop:
    cmpb $0, (%r10) //check if pointer is at null terminator
    jz .LconvertCharsDone //if yes, jump out of loop
    cmpb $65, (%r10) //compare char with 'A'
    jb .LadvanceLoop //if below 'A', continue loop without change
    cmpb $90, (%r10) //compare char with 'Z'
    ja .LadvanceLoop //if above 'Z', continue loop without change
    addb $32, (%r10) //if char was between 'A', 'Z' inclusive, increment by 32 to lowercase

.LadvanceLoop:
    inc %r10 //advance to next char
    jmp .LconvertCharsLoop //next loop iteration

.LconvertCharsDone:
    addq $8, %rsp //restore stack pointer
	ret

/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */

 /*VERIFIED*/
	.globl wc_trim_non_alpha
wc_trim_non_alpha:
	subq $8, %rsp
	pushq %r12 //store callee saved register value
	pushq %r13 //store callee saved register value
	movq %rdi, %r12 //move input string to callee saved register
	movq $0, %r13 //set value to 0

.LcharLoop:
    cmpb $0, (%r12) //check if char is null terminator
    jz .LcharDone //if so, jump to end
    movb (%r12), %dil //move current byte into first argument byte
    call wc_isalpha //call function wc_isalpha on current byte
    cmpq $1, %rax //check if current byte is alphabetical
    jz .LlastAlpha //current last alphabetical char

    inc %r12 //advance to next byte/char in string
    jmp .LcharLoop //continue loop

.LlastAlpha:
    movq %r12, %r13 //store address of laast alphabetical char in r13
    inc %r12 //advance to next byte/char in string
    jmp .LcharLoop //continue loop

.LcharDone:
    movb $0, 1(%r13) //set byte after last alphabetical character to null terminator
    popq %r13 //restore callee saved register value
    popq %r12 //restore callee saved register value
    addq $8, %rsp
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

 /*VERIFIED*/
	.globl wc_find_or_insert
wc_find_or_insert:
	subq $8, %rsp //adjust stack pointer
	pushq %r12 //store value in callee saved register
	pushq %r13 //store value in callee saved register
	pushq %r14 //store value in callee saved register
	pushq %r15 //store value in callee saved register
	movq %rdi, %r15 //store linked list head pointer in r15
	movq %rsi, %r13 //store search string pointer in r13
	movq %rdx, %r14 //store insertion status int pointer in r14
	movq %rdi, %r12 //store current wordentry pointer in r12

    cmpq $0, %r15 //check if head is null
    jz .LwordMissing //no words in chain

.LsearchLoop:
    leaq WORDENTRY_WORD_OFFSET(%r12), %rdi //put word pointer into arg1 register
    movq %r13, %rsi // move search string into second arg register
    call wc_str_compare //call compare function on current str and search str
    cmpq $0, %rax //check if words are the same or not
    jz .LwordFound //if equal, jump to success case
    movq WORDENTRY_NEXT_OFFSET(%r12), %r8 //load address of wordentry->next into r8
    cmpq $0, %r8 //check if current wordentry is the end of the chain
    jz .LwordMissing //if end of chain, jump to fail case
    movq WORDENTRY_NEXT_OFFSET(%r12), %r12 //move to next word in chain
    jmp .LsearchLoop //continue loop

.LwordFound:
    movq $0, (%r14) //set int at pointer to 0
    movq %r12, %rax //put address of wordentry in return register
    jmp .Ldone //jump to end of procedure

.LwordMissing:
    movq $WORDENTRY_SIZE, %rdi //move wordentry size into arg1 register
    call malloc //allocate new wordentry struct
    movq %rax, %r12 //load new wordentry into r12
    movq %r13, %rsi //move string into arg2 register
    leaq WORDENTRY_WORD_OFFSET(%r12), %rdi //put wordentry->word address into  arg1 register
    call wc_str_copy //copy input string into wordentry->word
    movq $0, WORDENTRY_COUNT_OFFSET(%r12) //set word count to 0
    movq (%r15), %r8 //move head address into r8
    movq %r8, WORDENTRY_NEXT_OFFSET(%r12) //set wordentry->next to head of chain
    movq $1, (%r14) //set int at pointer to 1
    movq %r12, %rax //move pointer to new wordentry into return register

.Ldone:
    popq %r15 //restore value in callee saved register
    popq %r14 //restore value in callee saved register
    popq %r13 //restore value in callee saved register
    popq %r12 //restore value in callee saved register
    addq $8, %rsp //restore stack pointer
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
	subq $8, %rsp //adjust stack pointer
    pushq %r12 //store callee saved register value
    pushq %r13 //store callee saved register value
    pushq %r14 //store callee saved register value
    pushq %r15 //store callee saved register value
    movq %rdi, %r12 //load buckets array pointer int0 r12
    movq %rsi, %r13 //load num_buckets value into r13
    movq %rdx, %r14 //load input string pointer into r14

.LdictionarySearch:
    movq %r14, %rdi //load input string pointer into arg1 register
    call wc_hash //calculate hash code for string
    idivq %r13 //divide rax (hash_code) by r13 (num_buckets)
    movq %rdx, %r15 //load hash mod num_buckets into r15

    movq $4, %rdi //load size of int into arg1 register
    call malloc //allocate memory for integer
    movq %rax, %r13 //put address of allocated int into r13
    movq $0, (%r13) //initialize heap allocated int to 0
    leaq (%r15,%r12), %rdi //put pointer to bucket into arg1 register
    movq %r14, %rsi //put input string into arg2 register
    movq %r13, %rdx //put address of insert status int into arg3 register
    call wc_find_or_insert //find or insert wordentry into bucket/chain
    movq (%r13), %r10
    cmpq $1, %r10 //check if word was inserted
    jz .LwordInserted //word was inserted
    jmp .LwordInChain //word already in chain

.LwordInserted:
    movq (%r15, %r12), %r15 //find address of appropriate bucket
    movq %rax, %r15 //put address of wordentry object into index of array

.LwordInChain:
    movq %r13, %rdi //move allocated int pointer into arg1
    call free //free allocated variable
    popq %r15 //restore callee saved register
    popq %r14 //restore callee saved register
    popq %r13 //restore callee saved register
    popq %r12 //restore callee saved register
    addq $8, %rsp //restore stack pointer
	ret

/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */

 /*VERIFIED*/
	.globl wc_free_chain
wc_free_chain:
	subq $8, %rsp //adjust stack pointer
	pushq %r12 //store value in callee saved register
	movq %rdi, %r12 //load wordentry pointer into %r12

    cmpq $0, WORDENTRY_NEXT_OFFSET(%r12) //check if wordentry->next is null (end of chain)
    jz .LendOfChain //if end of chain, jump past recursive call
	movq WORDENTRY_NEXT_OFFSET(%r12), %rdi //put wordentry->next into arg1 register
	call wc_free_chain //recursively call function on next wordentry

.LendOfChain:
    movq %r12, %rdi //move current wordentry into arg1 register
    call free //call free function on current wordentry

	popq %r12 //restore value in callee saved register
	ret

/*
vim:ft=gas:
*/
