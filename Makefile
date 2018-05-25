GCC_FLAGS=-g -std=gnu++14 -Wall -Wno-deprecated -Wdeprecated-declarations -Wno-error=deprecated-declarations -Wno-sign-compare -Wno-unused -Wunused-label -Wunused-result -Wnon-virtual-dtor -fopenmp
CPP_LIBS=-lfolly -lglog -levent -lgflags -lpthread

2018-05.bin: 2018-05.cpp
	g++ $^ -O3 $(GCC_FLAGS) $(CPP_LIBS) -o $@

2018-05: 2018-05.bin
	export OMP_NUM_THREADS=5 && ./$^ --logtostderr
