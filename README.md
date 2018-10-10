### OpenMP Template in C++ ###

Basic OpenMP template

## Getting started ##
To compile and run from command line if you are not on windows:
```
cd src
g++ -fopenmp main.cpp CStopWatch.cpp
./a.out
```
or
```
cd Default && make all
```

To compile and run with Docker:
```
docker run --rm -v ${PWD}:/tmp -w /tmp/Default rgreen13/alpine-bash-gpp make all
docker run --rm -v ${PWD}:/tmp -w /tmp/Default rgreen13/alpine-bash-gpp ./OpenMP
```
