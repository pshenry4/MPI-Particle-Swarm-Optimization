### OpenMP Template in C++ using OSC ###
This is a basic template for implementing OpenMP projects in CS 4170/5170


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

## Using OSC ##
Move all the files to the Ohio Supercomputing Center (OSC) server of your choice. Make sure to build your code, then modify the `jobScript` accordingly. Submit from inside the `Default` directory using 
```
qsub jobScript
```

You may also do this using http://ondemand.osc.edu