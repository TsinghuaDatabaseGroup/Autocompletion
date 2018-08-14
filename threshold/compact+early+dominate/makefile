COMPILER = g++
CCFLAGS = -O3 -std=c++11
METHOD = sigmod15
ALLOBJECTS = main.o trie.o util.o mtrie.o

all: $(METHOD)

$(METHOD) : $(ALLOBJECTS)
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -o $(METHOD) $(ALLOBJECTS)

main.o: main.cc util.o trie.o mtrie.o
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -c main.cc

util.o: util.cc util.h
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -c util.cc

mtrie.o: mtrie.cc trie.o
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -c mtrie.cc

trie.o: trie.cc trie.h
	${COMPILER} ${CCFLAGS} $(ADDFLAGS) -c trie.cc

clean:
	rm -f $(METHOD) *.o
