CONTRIBUTIONS
This project was implemented solely by Casey Burhoe.

REPORT

The following logs are the experiments run to test the sorting time for varying
levels of parallelization, controlled by adjusting the threshold under which the
array would be sorted sequentially instead.

    ---DATA---
Test run with threshold 2097152
real    0m0.404s
user    0m0.384s
sys     0m0.019s

Test run with threshold 1048576
real    0m0.243s
user    0m0.414s
sys     0m0.021s

Test run with threshold 524288
real    0m0.190s
user    0m0.501s
sys     0m0.037s

Test run with threshold 262144
real    0m0.195s
user    0m0.386s
sys     0m0.052s

Test run with threshold 131072
real    0m0.236s
user    0m0.358s
sys     0m0.073s

Test run with threshold 65536
real    0m0.305s
user    0m0.390s
sys     0m0.108s

Test run with threshold 32768
real    0m0.416s
user    0m0.425s
sys     0m0.135s
    ---END-DATA---

As indicated in the "real" time value for each experiment,
the best performance was achieved using a threshold of 524288.
This means that an array passed into any given call of the merge_sort() function
with less than 524,288 elements would be sequentially sorted, rather than
make use of the parallelization of the function.

This threshold value makes sense for the peak performance. The experiments were
run on a machine using 4-core intel processors. This means that up to 4 separate
processes can run in parallel at the same time before they start competing for resources.

The experiments were done on a 16Mb (16,777,216 bytes) array of integers,
consisting of 2,097,157 8-byte elements, so the maximum level of parallelism would be
achieved when splitting the sequential sort into 2,097,157 / 4 = 524,288 element portions.
For thresholds less than this, more of the array is being sequentially sorted,
underutilizing parallel computing resources. For thresholds greater than this,
more processes are created than can run/scheduled simultaneously, so child processes
will be competing for resources and slow down the sorting function.