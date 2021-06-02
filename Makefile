CC:=g++
CFLAGS:= -std=c++11 -O3 -Wall -Wextra -Wfatal-errors -fopenmp -DUSE_32BIT 

EXE1:= test_heap
EXE2:= test_graph

all: ${EXE1} ${EXE2}

heap.o: heap.cc heap.h def.h
	${CC} ${CFLAGS} -c $< -o $@

test_heap.o: test_heap.cc heap.h def.h
	${CC} ${CFLAGS} -c $< -o $@

graph.o: graph.cc graph.h def.h
	${CC} ${CFLAGS} -c $< -o $@

test_graph.o: test_graph.cc graph.h def.h
	${CC} ${CFLAGS} -c $< -o $@

${EXE1}: test_heap.o heap.o
	${CC} ${CFLAGS} $^ -o $@ -lm

${EXE2}: graph.o heap.o test_graph.o
	${CC} ${CFLAGS} $^ -o $@ -lm

clean:
	rm -f *.o ${EXE1} ${EXE2}
