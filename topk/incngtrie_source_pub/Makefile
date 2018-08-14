# Copyright 2012 UNSW CSE. All Rights Reserved.
# Author: jqin@cse.unsw.edu.au (Jianbin Qin)

PROD	:= PROD
OPT     := -O3
VERSION := \"0.0.1.0_${PROD}\"
TARGETS := searcher
DEFINES := 
SRCS    := vector_trie.cc brother_trie.cc map_trie.cc trie_base.cc dataset.cc util.cc variants.cc fastssindex.cc active_node.cc fastss_searcher.cc searcher_base.cc easy_timer.cc searcher.cc
HEADERS := 
OBJS    := ${SRCS:.cc=.o}


CCFLAGS = ${OPT} -Wall -std=c++0x -Wno-deprecated -ggdb -D${PROD} ${DEFINES} -DVERSION=${VERSION} 
LDFLAGS = ${OPT} -ggdb -std=c++0x
LIBS    = -lcrypto
CC	= g++


.PHONY: all clean distclean 
all:: ${TARGETS} 

searcher: searcher.o fastssindex.o variants.o dataset.o fastss_searcher.o active_node.o searcher_base.o easy_timer.o util.o map_trie.o trie_base.o vector_trie.o brother_trie.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}


${OBJS}: %.o: %.cc
	${CC} ${CCFLAGS} -o $@ -c $< 

clean:: 
	-rm -f *~ *.o ${TARGETS}

install:
	cp ${TARGETS} ../bin/

distclean:: clean
