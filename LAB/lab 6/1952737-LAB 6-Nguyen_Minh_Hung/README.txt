COMMANDS: 
    make all: create all object and executable files. (mem, main.o, mem.o)
    ./mem 1 : testing with First-fit allocator
    ./mem 2 : testing with Best-fit allocator
    ./mem 3 : testing with Worst-fit allocator
    make clean: clean all  
    sh test.sh : testing and displaying with all 3 algorithms
    sh test.sh > output.txt : save results to output.txt