# Multi-threaded-prime-finder
Find a certain number of primes entered as one of the command line arguments using the PThreads API. The collector thread inserts the primes as it recieves them into a min-heap. The generator threads are searching for the prime numbers. A bounded buffer implemented as a monitor was used to store the primes and then remove them at the same time. The program then prints out each prime in sorted order to the stdout then terminates.

# How to Use
run the commands 
```bash
make
./mtprimes -b <block> -l <limit> -t <num_threads>
```
block - the block each thread is searching through (e.g., one thread does 0..49, another 50..99, etc.)<br />
limit - number of primes to be found <br />
num_threads - number of threads fiding primes at the same time<br />
