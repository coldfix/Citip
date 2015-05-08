#  Citip -- Information Theoretic Inequality Prover (C++/CLI version)
#
#  Copyright (C) 2015 Thomas Gläßle <t_glaessle@gmx.de>
#                     http://github.com/coldfix/Citip
#
#  This file is copied from the Xitip program and possibly modified. For a
#  detailed list of changes from the original file, see the git version
#  history.
#
#  Copyright (C) 2007 Rethnakaran Pulikkoonattu,
#                     Etienne Perron,
#                     Suhas Diggavi.
#                     Information Processing Group,
#                     Ecole Polytechnique Federale de Lausanne,
#                     EPFL, Switzerland, CH-1005
#                     Email: rethnakaran.pulikkoonattu@epfl.ch
#                            etienne.perron@epfl.ch
#                            suhas.diggavi@epfl.ch
#                     http://ipg.epfl.ch
#                     http://xitip.epfl.ch
#  Dependent utilities:
#  The program uses two other softwares
#  1) The ITIP software developed by Raymond Yeung
#  2) The GLPK (GNU Linear Programming Kit) for linear programming
#  The details of the licensing terms of the above mentioned software shall
#  be obtained from the respective websites and owners.
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

CC=gcc
CFLAGS=
LDFLAGS=


OBJS= Citip.o ITIP.o itip1.o main.o parser.o scanner.o

all: Citip

Citip: $(OBJS)
	g++ -o $@ $^ -lglpk

%.o: %.cpp
	g++ $(CFLAGS) -o $@ -c $< -std=c++11

parser.cpp: parser.y
	bison $<

scanner.cpp: scanner.l
	flex $<

parser.hpp:  parser.cpp
scanner.hpp: scanner.cpp
parser.o:    ast.hpp scanner.hpp
scanner.o:   ast.hpp parser.hpp
Citip.o:     ast.hpp parser.hpp scanner.hpp


temp = parser.cpp parser.hpp scanner.cpp scanner.hpp \
	   location.hh position.hh stack.hh

clean:
	rm -f ITIP.c itip.c $(OBJS) $(temp)

clobber: clean
	rm -f Citip
