all: process sequencer clean

process: process.cpp
	gcc process.cpp -o process.out -lpthread

sequencer: sequencer.cpp
	gcc sequencer.cpp -o sequencer.out -lpthread

clean: 
	rm -rf *o
