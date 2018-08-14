COMPILER = g++
CCFLAGS = -O3 -std=c++11
METHOD = ipcan
ALLOBJECTS = main.o Trie.o ActiveNode.o

all: $(METHOD)

$(METHOD) : $(ALLOBJECTS)
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -o $(METHOD) $(ALLOBJECTS)

main.o: main.cc Trie.o ActiveNode.o
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -c main.cc

ActiveNode.o: ActiveNode.h ActiveNode.cc Trie.o
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -c ActiveNode.cc

Trie.o: Trie.cc Trie.h
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -c Trie.cc

clean:
	rm -f $(METHOD) *.o
