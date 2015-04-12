/*
 *  Citip -- Information Theoretic Inequality Prover (C++/CLI version)
 *
 *  Copyright (C) 2015 Thomas Gläßle <t_glaessle@gmx.de>
 *                     http://github.com/coldfix/Citip
 *
 *  This file is copied from the Xitip program and possibly modified. For a
 *  detailed list of changes from the original file, see the git version
 *  history.
 *
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
 *  2) The GLPK (GNU Linear Programming Kit) for linear programming
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


/***********************************************************************
 *                                                                     *
 *             Information Theoretic Inequality Prover (ITIP)          *
 *                                                                     *
 *                   Raymond W. Yeung and Ying-On Yan                  *
 *                                                                     *
 *                              Version 3.0                            *
 *                    Last updated on August 10, 2001                  *
 *                          All rights reserved                        *
 *                                                                     *
 *         Please send comments and suggestions to the authors at:     *
 *               whyeung@ie.cuhk.edu.hk and yy26@cornell.edu           *
 *                                                                     *
 ***********************************************************************/
/* This c-only version of ITIP is based on the original code by
 * Raymond W. Yeung and Ying-On Yan. It has been modified by
 * Etienne Perron and can be used and changed freely. Please
 * do not remove this comment. For questions write to
 * etienne.perron@epfl.ch.
 * For solving the linear programs, we use the GLPK callable library,
 * available at https://www.gnu.org/software/glpk/
 */

/*
Note that in this ITIP software, the dimension of the optimization space
is not reduced using the constraints. The constraints will not be set as
D \tilde{Q} v'  >= 0 as in Yeung's "Framework" paper, but they will simply be
D >= 0 AND Q >= 0
*/



/* this function stores D into the provided LPX object*/

#include <math.h>
#include <stdlib.h>
#include <glpk.h>

void make_D(glp_prob* lp, int number_vars) /*number_constraints indicates the number of rows already present in the linear program*/
{
    long int index,i,j,s1,s2,mask,temp ;
    int number_rows;
    int number_cols;
    int *indices;
    double *values;
    int row;

    int ind_tmp;
    double val_tmp;

    if (number_vars == 1) {
        ind_tmp = 1;
        val_tmp = 1;
        row = glp_add_rows(lp, 1);
        glp_set_row_bnds(lp, row, GLP_LO, 0.0, NAN);
        glp_set_mat_row(lp, row, 1, (&ind_tmp)-1, (&val_tmp)-1);
        return;
    }

    number_rows = glp_get_num_rows(lp);
    number_cols = glp_get_num_cols(lp);


    /*indices and values are buffers used to feed the current row into the LPX object:*/
    indices = (int *)malloc(number_cols * sizeof(int));
    values = (double *)malloc(number_cols * sizeof(double));

    /*row k will contain the quantity H(X1,X2,....,XN) - H(X1,X2,...,X(k-1),X(k+1),...,XN), where k=index*/
    for(index=1;index<=number_vars;index++)
    {
        s1 = (1L << (index - 1)) ; /*s1 is 2^(index-1)*/

        indices[0] = number_cols; /* H(X1,...,XN)*/ /*in GLPK, matrix-indices start at 1*/
        values[0] = -1;
        indices[1] = number_cols - s1; /* H( {X1,...,XN} minus {k} ) */
        values[1] = 1;

        number_rows++;

        /*add new row to the LP:*/
        row = glp_add_rows(lp, 1);
        glp_set_row_bnds(lp, row, GLP_UP, NAN, 0.0);
        glp_set_mat_row(lp, row, 2, indices-1, values-1);
    }




    /* this loop is for I(X_{i+1}; X_{j+1} | X_K), where K is a subset of { {1,...,number_vars} minus {i+1, j+1}} */
    for(i=0;i<(number_vars-1);i++){
        for(j=i+1;j<number_vars;j++){
            s1 = 1L << i ; /*the single variable X_k has coordinate (1L << k-1) in the joint entropy space (k = 1,...,number_vars)*/
            s2 = 1L << j ;
            mask = s1 | s2 ; /*this is the index of (X_(i+1),X_(j+1))*/

            /*first row ( for K = {} ):*/
            indices[0] = s1; /*H(X_(i+1))*/
            values[0] = -1;

            indices[1] = s2; /*H(X_(j+1))*/
            values[1] = -1;

            indices[2] = mask; /* H(X_(i+1),X_(j+1)) */
            values[2] = 1;

            number_rows++;

            /*add new row to the LP*/
            row = glp_add_rows(lp, 1);
            glp_set_row_bnds(lp, row, GLP_UP, NAN, 0.0);
            glp_set_mat_row(lp, row, 3, indices-1, values-1);



            /*additional rows for all non-empty K:*/
            for(temp=1;temp<=number_cols;temp++) { /* temp runs through all possible subsets of (X_1, ..., X_N)*/
                if(!(temp & mask)) { /*if K != (i+1,j+1)*/
                    indices[0] = (s1 | temp); /* H(X_(i+1), X_K) */
                    values[0] = -1;

                    indices[1] = (s2 | temp); /* H(X_(j+1), X_K) */
                    values[1] = -1;

                    indices[2] = temp; /* H(X_K) */
                    values[2] = 1;

                    indices[3] = (s1 | s2 | temp); /* H(X_(i+1), X_(j+1), X_K) */
                    values[3] = 1;

                    number_rows++;

                    /*add new row to the LP*/
                    row = glp_add_rows(lp, 1);
                    glp_set_row_bnds(lp, row, GLP_UP, NAN, 0.0);
                    glp_set_mat_row(lp, row, 4, indices-1, values-1);
                }
            }
        }
    }

    free(indices);
    free(values);
}
