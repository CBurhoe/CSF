Project files created and implemented by Casey Burhoe. Data collection and report completed by Casey Burhoe.

Report:

My approach to testing this was to try all viable combinations of:
1. write-allocate vs no-write-allocate
2. write-through vs write-back
3. lru vs fifo
4. direct mapped vs associative vs set associative
5. small slots (8 bytes) vs large slots (64 bytes)

Since the only combination that is impossible is no-write-allocate + write-back, this leaves 36 total combinations
of parameters tested. For consistency, all data was collected using the gcc trace provided in gcc.trace. Additionally,
total cache size was held constant for all configurations (sets * slots * bytes = cache size = 4096 bytes).

For the sake of brevity, the number of hits and misses were combined between load and store commands in the data
included below, and the number of loads and stores were omitted.


Results
Best Direct Mapped: 512 1 8 write-allocate write-back fifo/lru
                    Total hits: 474587
                    Total misses: 41096
                    Total cycles: 16791187

Best Associative: 1 512 8 write-allocate write-back lru
                  Total hits: 489894
                  Total misses: 25789
                  Total cycles: 10682694

Best Set Associative: 16 32 8 write-allocate write-back lru
                      Total hits: 489885
                      Total misses: 25798
                      Total cycles: 10686285

The results from my tests show that overall, a fully associative cache with write-allocate, write back, and LRU
policies, as well as a small slot size was the most optimal cache configuration. However, the same policies and slot
size used with a set associative cache performed quite similarly, only differing by a few thousand cycles and a
handful of hits/misses.





[DATA]
------
Direct Mapped:

[cburhoe1@ugradx csf_assign03]$ ./csim 512 1 8 no-write-allocate write-through lru < gcc.trace
Total hits: 451896
Total misses: 63787
Total cycles: 24584694
[cburhoe1@ugradx csf_assign03]$ ./csim 512 1 8 write-allocate write-through lru < gcc.trace
Total hits: 474587
Total misses: 41096
Total cycles: 28266586
[cburhoe1@ugradx csf_assign03]$ ./csim 512 1 8 write-allocate write-back lru < gcc.trace
Total hits: 474587
Total misses: 41096
Total cycles: 16791187
[cburhoe1@ugradx csf_assign03]$ ./csim 512 1 8 no-write-allocate write-through fifo < gcc.trace
Total hits: 451896
Total misses: 63787
Total cycles: 24584694
[cburhoe1@ugradx csf_assign03]$ ./csim 512 1 8 write-allocate write-through fifo < gcc.trace
Total hits: 474587
Total misses: 41096
Total cycles: 28266586
[cburhoe1@ugradx csf_assign03]$ ./csim 512 1 8 write-allocate write-back fifo < gcc.trace
Total hits: 474587
Total misses: 41096
Total cycles: 16791187
[cburhoe1@ugradx csf_assign03]$ ./csim 64 1 64 no-write-allocate write-through lru < gcc.trace
Total hits: 460315
Total misses: 55368
Total cycles: 54761899
[cburhoe1@ugradx csf_assign03]$ ./csim 64 1 64 write-allocate write-through lru < gcc.trace
Total hits: 488882
Total misses: 26801
Total cycles: 62927645
[cburhoe1@ugradx csf_assign03]$ ./csim 64 1 64 write-allocate write-back lru < gcc.trace
Total hits: 488882
Total misses: 26801
Total cycles: 86100082
[cburhoe1@ugradx csf_assign03]$ ./csim 64 1 64 no-write-allocate write-through fifo < gcc.trace
Total hits: 460315
Total misses: 55368
Total cycles: 54761899
[cburhoe1@ugradx csf_assign03]$ ./csim 64 1 64 write-allocate write-through fifo < gcc.trace
Total hits: 488882
Total misses: 26801
Total cycles: 62927645
[cburhoe1@ugradx csf_assign03]$ ./csim 64 1 64 write-allocate write-back fifo < gcc.trace
Total hits: 488882
Total misses: 26801
Total cycles: 86100082

Associative:

[cburhoe1@ugradx csf_assign03]$ ./csim 1 512 8 no-write-allocate write-through lru < gcc.trace
Total hits: 467385
Total misses: 48298
Total cycles: 22242464
[cburhoe1@ugradx csf_assign03]$ ./csim 1 512 8 write-allocate write-through lru < gcc.trace
Total hits: 489894
Total misses: 25789
Total cycles: 25217547
[cburhoe1@ugradx csf_assign03]$ ./csim 1 512 8 write-allocate write-back lru < gcc.trace
Total hits: 489894
Total misses: 25789
Total cycles: 10682694
[cburhoe1@ugradx csf_assign03]$ ./csim 1 512 8 no-write-allocate write-through fifo < gcc.trace
Total hits: 463651
Total misses: 52032
Total cycles: 22589122
[cburhoe1@ugradx csf_assign03]$ ./csim 1 512 8 write-allocate write-through fifo < gcc.trace
Total hits: 486860
Total misses: 28823
Total cycles: 25822241
[cburhoe1@ugradx csf_assign03]$ ./csim 1 512 8 write-allocate write-back fifo < gcc.trace
Total hits: 486860
Total misses: 28823
Total cycles: 11893860
[cburhoe1@ugradx csf_assign03]$ ./csim 1 64 64 no-write-allocate write-through lru < gcc.trace
Total hits: 481983
Total misses: 33700
Total cycles: 30041359
[cburhoe1@ugradx csf_assign03]$ ./csim 1 64 64 write-allocate write-through lru < gcc.trace
Total hits: 507131
Total misses: 8552
Total cycles: 33744566
[cburhoe1@ugradx csf_assign03]$ ./csim 1 64 64 write-allocate write-back lru < gcc.trace
Total hits: 507131
Total misses: 8552
Total cycles: 27713531
[cburhoe1@ugradx csf_assign03]$ ./csim 1 64 64 no-write-allocate write-through fifo < gcc.trace
Total hits: 478664
Total misses: 37019
Total cycles: 33381670
[cburhoe1@ugradx csf_assign03]$ ./csim 1 64 64 write-allocate write-through fifo < gcc.trace
Total hits: 504473
Total misses: 11210
Total cycles: 37995205
[cburhoe1@ugradx csf_assign03]$ ./csim 1 64 64 write-allocate write-back fifo < gcc.trace
Total hits: 504473
Total misses: 11210
Total cycles: 36186073

Set Associative:

[cburhoe1@ugradx csf_assign03]$ ./csim 16 32 8 no-write-allocate write-through lru < gcc.trace
Total hits: 467266
Total misses: 48417
Total cycles: 22254006
[cburhoe1@ugradx csf_assign03]$ ./csim 16 32 8 write-allocate write-through lru < gcc.trace
Total hits: 489885
Total misses: 25798
Total cycles: 25219350
[cburhoe1@ugradx csf_assign03]$ ./csim 16 32 8 write-allocate write-back lru < gcc.trace
Total hits: 489885
Total misses: 25798
Total cycles: 10686285
[cburhoe1@ugradx csf_assign03]$ ./csim 16 32 8 no-write-allocate write-through fifo < gcc.trace
Total hits: 464449
Total misses: 51234
Total cycles: 22576187
[cburhoe1@ugradx csf_assign03]$ ./csim 16 32 8 write-allocate write-through fifo < gcc.trace
Total hits: 486779
Total misses: 28904
Total cycles: 25838408
[cburhoe1@ugradx csf_assign03]$ ./csim 16 32 8 write-allocate write-back fifo < gcc.trace
Total hits: 486779
Total misses: 28904
Total cycles: 11926179
[cburhoe1@ugradx csf_assign03]$ ./csim 16 4 64 no-write-allocate write-through lru < gcc.trace
Total hits: 476070
Total misses: 39613
Total cycles: 37510288
[cburhoe1@ugradx csf_assign03]$ ./csim 16 4 64 write-allocate write-through lru < gcc.trace
Total hits: 501926
Total misses: 13757
Total cycles: 42067902
[cburhoe1@ugradx csf_assign03]$ ./csim 16 4 64 write-allocate write-back lru < gcc.trace
Total hits: 501926
Total misses: 13757
Total cycles: 44341926
[cburhoe1@ugradx csf_assign03]$ ./csim 16 4 64 no-write-allocate write-through fifo < gcc.trace
Total hits: 471495
Total misses: 44188
Total cycles: 42264115
[cburhoe1@ugradx csf_assign03]$ ./csim 16 4 64 write-allocate write-through fifo < gcc.trace
Total hits: 498506
Total misses: 17177
Total cycles: 47536964
[cburhoe1@ugradx csf_assign03]$ ./csim 16 4 64 write-allocate write-back fifo < gcc.trace
Total hits: 498506
Total misses: 17177
Total cycles: 55316106