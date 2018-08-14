COMPILER = g++
CCFLAGS = -O3 -std=c++11
METHOD = sigmod15
ALLOBJECTS = main.o trie.o util.o

all: $(METHOD)

$(METHOD) : $(ALLOBJECTS)
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -o $(METHOD) $(ALLOBJECTS)

main.o: main.cc util.h trie.h
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -c main.cc

util.o: util.cc util.h
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -c util.cc

trie.o: trie.cc trie.h
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -c trie.cc

clean:
	rm -f $(METHOD) *.o
