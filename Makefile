GCC_FLAGS=-g -std=gnu++14 -Wall -Wno-deprecated -Wdeprecated-declarations -Wno-error=deprecated-declarations -Wno-sign-compare -Wno-unused -Wunused-label -Wunused-result -Wnon-virtual-dtor -fopenmp
CPP_LIBS=-lfolly -lcurl -lboost_context -lboost_chrono -lboost_date_time -lboost_filesystem -lboost_program_options -lboost_regex -lboost_system -lboost_thread -lboost_atomic -lpthread -ldouble-conversion -lglog -levent -lssl -lcrypto -ldouble-conversion -lglog -lgflags -lpthread -levent -lssl -lcrypto -lz -llzma -llz4 -lzstd -lsnappy -liberty -ldl -lpthread -lgmock -lgtest 

2018-05.bin: 2018-05.cpp
	g++ $^ -O3 $(GCC_FLAGS) $(CPP_LIBS) -o $@

2018-05: 2018-05.bin
	export OMP_NUM_THREADS=5 && ./$^ --logtostderr
