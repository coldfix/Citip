#  Xitip -- Information theoretic inequality Prover. 
#  Xitip.c is the main C program to produce the GTK 2+ based
#  graphical front end.
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
#  2) qsopt, a linear programming solver developed by David Applegate et al.
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
# 
#-----------------------Makefile-----------------------
# Just type make to produce the Xitip executable. The following packages 
# are required for successfully compilation.
# Gtk2+, (and all libs)
# lex/flex
# yacc/bison
# gcc (or cc)
# pango, glib, xpm and related libs
# qsopt (if not included, obtain the library from qsopt webpage)
#


CC=gcc
CFLAGS= `pkg-config --cflags gtk+-2.0`
LDFLAGS=-lm `pkg-config --libs gtk+-2.0`


OBJS= make_D.o ITIP.o itip1.o Xitip.o

Xitip:	$(OBJS) qsopt.a

clean:
	rm -f ITIP.c itip.c $(OBJS)

clobber: clean
	rm -f Xitip
