BUILDDIR = build
OBJS     = $(addprefix $(BUILDDIR)/,main.o parser.o scanner.o citip.o)
CPPFLAGS = -MMD -MP
CXXFLAGS = -std=c++11 -I. -I$(BUILDDIR)

all: prepare Citip

Citip: $(OBJS)
	g++ -o $@ $^ -lglpk

$(BUILDDIR)/%.o: %.cpp
	$(CXX) -o $@ -c $< $(CPPFLAGS) $(CXXFLAGS)

$(BUILDDIR)/%.o: $(BUILDDIR)/%.cxx
	$(CXX) -o $@ -c $< $(CPPFLAGS) $(CXXFLAGS)

$(BUILDDIR)/parser.cxx: parser.y
	bison -o $@ --defines=$(BUILDDIR)/parser.hxx $<

$(BUILDDIR)/scanner.cxx: scanner.l
	flex  -o $@ --header-file=$(BUILDDIR)/scanner.hxx $<

$(OBJS): $(BUILDDIR)/scanner.cxx $(BUILDDIR)/parser.cxx

.PHONY: prepare all
prepare:
	@mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

clobber: clean
	rm -f Citip

-include $(OBJS:%.o=%.d)
