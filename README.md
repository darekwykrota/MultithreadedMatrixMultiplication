# MultithreadedMatrixMultiplication

An attempt to speed up matrix multiplication by using C++11 threads.



For matrices of size 1000 i get the following results:

1) Single threaded version - 242 seconds.

2) Multithreaded version (where std::thread::hardware_concurrency() returns 8) - 57 seconds.
