/* 
 *  Xitip -- Information theoretic inequality Prover. 
 *  Xitip.c is the main C program to produce the GTK 2+ based
 *  graphical front end.
 *  Copyright (C) 2007 Rethnakaran Pulikkoonattu,
 *                     Etienne Perron, 
 *                     Suhas Diggavi. 
 *                     Information Processing Group, 
 *                     Ecole Polytechnique Federale de Lausanne,
 *                     EPFL, Switzerland, CH-1005
 *                     Email: rethnakaran.pulikkoonattu@epfl.ch
 *                            etienne.perron@epfl.ch
 *                            suhas.diggavi@epfl.ch
 *                     http://ipg.epfl.ch
 *                     http://xitip.epfl.ch
 *  Dependent utilities:
 *  The program uses two other softwares
 *  1) The ITIP software developed by Raymond Yeung 
 *  2) qsopt, a linear programming solver developed by David Applegate et al.
 *  The details of the licensing terms of the above mentioned software shall 
 *  be obtained from the respective websites and owners. 
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

int itip1(char **expressions, int number_expressions);
