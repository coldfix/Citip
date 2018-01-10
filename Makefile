OBJS     = main.o parser.o scanner.o citip.o
CPPFLAGS = -MMD -MP
CXXFLAGS = -std=c++11

all: Citip

Citip: $(OBJS)
	g++ -o $@ $^ -lglpk

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CPPFLAGS) $(CXXFLAGS)

%.o: %.cxx
	$(CXX) -o $@ -c $< $(CPPFLAGS) $(CXXFLAGS)

parser.cxx: parser.y
	bison -o $@ --defines=parser.hxx $<

scanner.cxx: scanner.l
	flex -o $@ --header-file=scanner.hxx $<

$(OBJS): scanner.cxx parser.cxx

clean:
	rm -f *.o *.cxx *.hxx *.hh *.d

clobber: clean
	rm -f Citip

-include $(OBJS:%.o=%.d)
