CC:=g++

CFLAGS:= -std=c++11

ifeq ($(debug), 1)
	CFLAGS+= -g -Wall -Wextra -Wfatal-errors -fopenmp -DUSE_32BIT 
else
	CFLAGS+= -O3 -Wall -Wextra -Wfatal-errors -fopenmp -DUSE_32BIT
endif

EXE1:= test_heap
EXE2:= test_graph
EXE3:= test_heap_list
EXE4:= test_partition
EXE5:= test_louvain

all: ${EXE1} ${EXE2} ${EXE3} ${EXE4} ${EXE5}

heap.o: heap.cc heap.h def.h
	${CC} ${CFLAGS} -c $< -o $@

test_heap.o: test_heap.cc heap.h def.h
	${CC} ${CFLAGS} -c $< -o $@

graph.o: graph.cc graph.h def.h
	${CC} ${CFLAGS} -c $< -o $@

test_graph.o: test_graph.cc graph.cc graph.h def.h
	${CC} ${CFLAGS} -c $< -o $@

heap_list.o: heap_list.cc heap_list.h def.h
	${CC} ${CFLAGS} -c $< -o $@

test_heap_list.o: test_heap_list.cc heap_list.h def.h
	${CC} ${CFLAGS} -c $< -o $@

partition.o: partition.cc partition.h graph.cc def.h
	${CC} ${CFLAGS} -c $< -o $@

test_partition.o: test_partition.cc partition.cc partition.h graph.cc heap.cc def.h
	${CC} ${CFLAGS} -c $< -o $@

louvain.o: louvain.cc partition.cc graph.cc louvain.h heap.cc
	${CC} ${CFLAGS} -c $< -o $@

test_louvain.o: test_louvain.cc louvain.cc partition.cc graph.cc def.h heap.cc
	${CC} ${CFLAGS} -c $< -o $@

${EXE1}: test_heap.o heap.o
	${CC} ${CFLAGS} $^ -o $@ -lm 

${EXE2}: test_graph.o graph.o heap.o
	${CC} ${CFLAGS} $^ -o $@ -lm

${EXE3}: test_heap_list.o heap_list.o
	${CC} ${CFLAGS} $^ -o $@ -lm

${EXE4}: test_partition.o partition.o graph.o heap.o 
	${CC} ${CFLAGS} $^ -o $@ -lm

${EXE5}: test_louvain.o louvain.o partition.o graph.o heap.o 
	${CC} ${CFLAGS} $^ -o $@ -lm

clean:
	rm -f *.o ${EXE1} ${EXE2} ${EXE3} ${EXE4} ${EXE5}
