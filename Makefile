CC=gcc
CFLAGS=
LDFLAGS=


OBJS= main.o parser.o scanner.o citip.o

all: Citip

Citip: $(OBJS)
	g++ -o $@ $^ -lglpk

%.o: %.cpp
	g++ $(CFLAGS) -o $@ -c $< -std=c++11

%.o: %.cxx
	g++ $(CFLAGS) -o $@ -c $< -std=c++11

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
