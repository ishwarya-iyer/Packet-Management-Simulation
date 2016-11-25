# Packet-Management-Simulation

The process life cycle of data packets is implemented in a multithreaded environment. 
The  job is to emulate the packet and token arrivals, the operation of the token bucket filter, the first-come-first-served queues Q1 and Q2, and servers S1 and S2. A trace of the emulation for every important event occurred in the emulation is also produced.

The said token bucket has a capacity (bucket depth) of B tokens. The inter-arrival time between two consecutive tokens is 1/r. We can call "r" the token arrival rate (although, it's not exactly that the token arrival rate; the tokens may not arrive at a constant rate of r). Extra tokens (overflow) would simply disappear if the token bucket is full. A token bucket, together with its control mechanism, is referred to as a token bucket filter.

Packets:
Packets arrive at the token bucket filter according to an arrival process where the inter-arrival time between two consecutive packets is 1/lambda. We can call "lambda" the packet arrival rate (although, it's not exactly that the packet arrival rate; the packets may not arrive at a constant rate of lambda). Each packet requires P tokens in order for it to be eligible for transmission. When a packet arrives, if Q1 is not empty, it will just get queued onto the Q1 facility. Otherwise, it will check if the token bucket has P or more tokens in it. If the token bucket has P or more tokens in it, P tokens will be removed from the token bucket and the packet will join the Q2 facility (So first, packets are added to Q1 and then time is noted,  P tokens are removed from the token bucket before transferring the packet from Q1 to Q2.Time is once again noted before the packet leaves Q1). Finally, if the number of tokens required by a packet is larger than the bucket depth, the packet is dropped (otherwise, it will block all other packets that follow it).


Servers:
The servers S1 and S2 serve packets in Q2 in the first-come-first-served order and at a service rate of "mu" per second. When a server becomes available, it will dequeue the first packet from Q2 and start transmitting the packet. When a packet has received 1/mu seconds of service, it leaves the system. The servers are kept as busy as possible.


Tokens:
When a token arrives at the token bucket, it will add a token into the token bucket. If the bucket is already full, the token will be lost. It will then check to see if Q1 is empty. If Q1 is not empty, it will see if there are enough tokens to make the packet at the head of Q1 be eligible for transmission. If it does, it will remove the corresponding number of tokens from the token bucket, remove that packet from Q1 and move it into Q2, and wake up the servers in case they are sleeping. It then checks the packet that is now at the head of Q1 to see if it's also eligible for transmission, and so on.


The system can run in only one of two modes.
1) Deterministic: 
In this mode, all inter-arrival times are equal to 1/lambda seconds, all packets require exactly P tokens, and all service times are equal to 1/mu seconds. If1/lambda is greater than 10 seconds, inter-arrival time is 10 seconds. If 1/mu is greater than 10 seconds,service time is 10 seconds.
 
2) Trace-driven: 
In this mode, the emulation will be driven using a trace file. Each line in the trace file specifies the inter-arrival time of a packet, the number of tokens it needs in order for it to be eligible for transmission, and its service time. (Please note that in this mode, it's perfectly fine if an inter-arrival time or a service time is greater than 10 seconds.)

Commandline format:
The command line syntax (also known as "usage information") for warmup2 is as follows:Square bracketed items are optional. The command line options can come in any order. Unless otherwise specified, the output of program goes to stdout and error messages goes to stderr.
The lambda, mu, r, B, and P parameters all have obvious meanings (according to the description above). The -n option specifies the total number of packets to arrive. If the -t option is specified, tsfile is a trace specification file that you should use to drive the emulation. In this case, you should ignore the -lambda, -mu, -P, and -n command line options and run your emulation in the trace-driven mode. tsfile should conform to the trace file format specification. (This means that if an error in encountered in this file, it simply prints an error message and calls exit()) If the -t option is not used, then it is the deterministic mode.
The default value (i.e., if it's not specified in a commandline option) for lambda is 1 (packets per second), the default value for mu is 0.35 (packets per second), the default value for r is 1.5 (tokens per second), the default value for B is 10 (tokens), the default value for P is 3 (tokens), and the default value for num is 20 (packets). B, P, and num must be positive integers with a maximum value of 2147483647 (0x7fffffff). lambda, mu, and r must be positive real numbers.

 Trace File Format:
    3
    2716   2    9253
    7721   1   15149
    972    3    2614

In the above example, packet 1 is to arrive 2716ms after emulation starts, it needs 2 tokens to be eligible for transmission, and its service time should be 9253ms; the inter-arrival time between packet 2 and 1 is to be 7721ms, it needs 1 token to be eligible for transmission, and its service time should be 15149ms; the inter-arrival time between packet 3 and 2 is to be 972ms, it needs 3 token to be eligible for transmission, and its service time should be 2614ms.
In the trace output, what is measured  is printed(i.e., by reading the clock) not the values in the file.
