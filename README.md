SA Queries in O(rlogn) space

libdivsufsort is used for suffix array construction so make sure to install that first

Run `make` in this directory to create two executables:

bin/sa_rlbwt
- This program can build our data structure and save it to a file. It can also read the data structure from a file and do things with it. Check the -h option.

<!-- end of first bullet -->

bin/sa
- This program uses libdivsufsort to create the suffix array for a text and saves it to a file. It can also read the suffix array from a file and do things with it. Check the -h option.

You can also run `make lib` to create a shared library containing functions for using our data structure. This must be done before building the modified FM-Index code under the FM-Index directory. The original implementation can be found here: https://github.com/mpetri/FM-Index. Just run `make` to compile, and check -h to see the options we added to fmlocate.

Random patterns to search for are generated using a program that you can compile by typing `make mygenpatterns`

For multiple experiments we used a 400 MB DNA file generated from a 100MB DNA file. We concatenated the 100MB DNA file 4 times and then made random mutations in 0.1% of the DNA. Type `make genstring` to compile the program used to do this.
