# Parallel Gauss-Jordan

This project is a parallel impementation of the _Gauss-Jordan_ method for solving linear systems using the __MPI with C++__. 

It was developed during my university class _Parallel Algorithms_

## Notes
* The array is distributed equaly to all the processes __by columns__ and computations are made in parallel.
* In order to avoid swapping all elements of a row on every iteration, a pivot array is used as an index and is kept persistant with the required communication.
* The code have been re-written to be compatible with older C++ compilers and no longer need the -std=c++11 compile flag.
* The B array is always handled by the master process (rank 0).
* There are two modes for distributing columns, _by groups_ and _round-robin_.
* [limitation]The dimention of the array A must be an exact multiple of the number of processes.
* The array A is always a square array _NxN_ with random values.

## Usage
To compile the files use the make command.

To run:
```shell
mpiexec -n <number of processes> ./parallel-gauss-jordan <dimention> <distribution mode (0/1)>
```
