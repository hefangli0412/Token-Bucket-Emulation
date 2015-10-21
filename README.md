Multithreading practice
====
In this assignment, you will emulate/simulate a traffic shaper who transmits packets controlled by a token bucket filter depicted below using multi-threading within a single process. If you are not familiar with pthreads, you should read Chapter 2 of our required textbook.

Below is an example what your program output must look like (please note that the values used here are just a bunch of unrelated random numbers for illustration purposes):
    Emulation Parameters:
        number to arrive = 20
        lambda = 2            (if -t is not specified)
        mu = 0.35             (if -t is not specified)
        r = 4
        B = 10
        P = 3                 (if -t is not specified)
        tsfile = FILENAME         (if -t is specified)

    00000000.000ms: emulation begins
    00000251.726ms: token t1 arrives, token bucket now has 1 token
    00000502.031ms: token t2 arrives, token bucket now has 2 tokens
    00000503.112ms: p1 arrives, needs 3 tokens, inter-arrival time = 503.112ms
    00000503.376ms: p1 enters Q1
    00000751.148ms: token t3 arrives, token bucket now has 3 tokens
    00000751.186ms: p1 leaves Q1, time in Q1 = 247.810ms, token bucket now has 0 token
    00000752.716ms: p1 enters Q2
    00000752.932ms: p1 leaves Q2, time in Q2 = 0.216ms
    00000752.982ms: p1 begins service at S1, requesting 2850ms of service
    00001004.271ms: p2 arrives, needs 3 tokens, inter-arrival time = 501.159ms
    00001004.526ms: p2 enters Q1
    00001007.615ms: token t4 arrives, token bucket now has 1 token
    00001251.259ms: token t5 arrives, token bucket now has 2 tokens
    00001505.986ms: p3 arrives, needs 3 tokens, inter-arrival time = 501.715ms
    00001506.713ms: p3 enters Q1
    00001507.552ms: token t6 arrives, token bucket now has 3 tokens
    00001508.281ms: p2 leaves Q1, time in Q1 = 503.755ms, token bucket now has 0 token
    00001508.761ms: p2 enters Q2
    00001508.874ms: p2 leaves Q2, time in Q2 = 0.113ms
    00001508.895ms: p2 begins service at S2, requesting 1900ms of service
    ...
    00003427.557ms: p2 departs from S2, service time = 1918.662ms, time in system = 2423.286ms
    00003612.843ms: p1 departs from S1, service time = 2859.861ms, time in system = 3109.731ms
    ...
    ????????.???ms: p20 departs from S?, service time = ???.???ms, time in system = ???.???ms
    ????????.???ms: emulation ends

    Statistics:

        average packet inter-arrival time = <real-value>
        average packet service time = <real-value>
    
        average number of packets in Q1 = <real-value>
        average number of packets in Q2 = <real-value>
        average number of packets at S1 = <real-value>
        average number of packets at S2 = <real-value>
    
        average time a packet spent in system = <real-value>
        standard deviation for time spent in system = <real-value>

        token drop probability = <real-value>
        packet drop probability = <real-value>
