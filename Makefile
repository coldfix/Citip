OBJS     = main.o parser.o scanner.o citip.o
CXXFLAGS = -std=c++11

all: Citip

Citip: $(OBJS)
	$(CXX) -o $@ $^ -lglpk

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CPPFLAGS) $(CXXFLAGS)

%.o: %.cxx
	$(CXX) -o $@ -c $< $(CPPFLAGS) $(CXXFLAGS)

parser.cxx: parser.y
	bison -o $@ --defines=parser.hxx $<

scanner.cxx: scanner.l
	flex -o $@ --header-file=scanner.hxx $<

parser.hxx:  parser.cxx
scanner.hxx: scanner.cxx
parser.o:    ast.hpp scanner.hxx
scanner.o:   ast.hpp parser.hxx
citip.o:     ast.hpp parser.hxx scanner.hxx
main.o:      parser.hxx citip.hpp

clean:
	rm -f *.o *.cxx *.hxx *.hh

clobber: clean
	rm -f Citip
