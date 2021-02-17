### OpenMP Assignment #2 for CS 4170/5170 ###
This is a basic template for OpenMP Assignment in CS 4170/5170

## Starting your own Repo ##

Follow these steps:

1. Create your appropriately named repo on Gitlab.

2. On your local computer: 
    - if using SSH, run  `git clone -b Assignment2 --single-branch git@gitlab.com:rgreen13/openmp.git <DIRECTORY_NAME>`. 
    - If using HTTPS, run `git clone -b Assignment2 --single-branch https://gitlab.com/rgreen13/openmp.git <DIRECTORY_NAME>`

3. `cd <DIRECTORY_NAME>`

4. `git remote remove origin`. This removes the current remote named `origin`.

5. `git remote add origin <URL_OF_YOUR_REPO>`. This adds a new remote that has the address of your repo.

6. `git branch -m master`. This changes the name of the current branch (Assignment2) to master.

6. `git push -u origin master`. This pushes your changes.

## Running ##
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