.PHONY:all clean
.DEFAULT_GOAL:= all
CXX = g++
RM = rm
MKDIR = mkdir
INCLUDEFLAGS = -I$(incdir)
CXXFLAGS = --std=c++11 -Wall -Wno-reorder -O3

incdir = include
srcdir = src
objdir = obj
bindir = bin

dirs := $(objdir) $(bindir)

src = $(wildcard $(srcdir)/*.cc)
obj = $(patsubst %.cc, $(objdir)/%.o, $(notdir ${src}))
dep = $(patsubst %.cc, $(srcdir)/%.d, $(notdir ${src}))
target = main
bin = $(bindir)/$(target)

all: $(dirs) $(bin) cleanup

$(dirs):
	$(MKDIR) $@
	
$(obj): $(objdir)/%.o: $(srcdir)/%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDEFLAGS) -o $@ -c $<

$(bin): $(obj)
	$(CXX) $(CXXFLAGS) $(INCLUDEFLAGS) -o $@ $^

$(dep): $(srcdir)/%.d: $(srcdir)/%.cc
	@set -e; \
	rm -f $@; \
	$(CXX) $(CXXFLAGS) -MM $(INCLUDEFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(objdir)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY:cleanup    
cleanup:
	rm -f $(srcdir)/*.d

-include $(dep)

.PHONY:clean
clean:
	$(RM) -rf $(dirs) $(srcdir)/*.d
