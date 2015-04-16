%{

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
/* The information related to the original ITIP program may be available
 from the following contacts. The original ITIP.y file was developed by
 Raymond Yeung and Ying-On Yan. */
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
 * Etienne Perron. Please do not remove this comment. For questions write to
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

    // Modification done Rethnakaran Pulikkoonattu
    // 1-Provision to allow small English letters as random variables
    //
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glpk.h>
#include "elemental_inequalities.h"

char argnumber, status, itype, macrodetect;
int numofinput, extrainput, multi, objcount;

char *input, buffer[255], ncount, fcount, vcount, condflag;
long int field[250], attribute, attrib[52], flag;  /* Ratna : 26*2 to have
small letter English alphabet as random variables*/

char rvnames[52][27], rvnames2[52][27], name[60]; // 26*2 x 2 dimension seems to be enough (Ratna)

long int rvtag[52];
int rvtotal;
long int tagmask;

double number, *nptr;
int c, offset;
struct EQN
{
    double coef;
    long int variable;
    struct EQN *next;
} *eqn, *current, *boundary;

struct EQNLIST
{
    struct EQN *eqn;
    int argtype;
    struct EQNLIST *next;
} *arghead, *arglist;

int delimiters[30], limcount;
%}
%start lines
%%
lines   : eq
    | macro1 {fcount=0;macrodetect=1;}
    | macro2 {fcount=0;macrodetect=2;}
    | macro3 {fcount=0;macrodetect=3;}
        ;
digit   : '0' {buffer[ncount++]='0';}
    | '1' {buffer[ncount++]='1';}
    | '2' {buffer[ncount++]='2';}
    | '3' {buffer[ncount++]='3';}
    | '4' {buffer[ncount++]='4';}
    | '5' {buffer[ncount++]='5';}
    | '6' {buffer[ncount++]='6';}
    | '7' {buffer[ncount++]='7';}
    | '8' {buffer[ncount++]='8';}
    | '9' {buffer[ncount++]='9';}
    ;
digitl  : digit
    | digitl digit
    ;
number  : digitl {itype=1;}
    | digitl '.' {buffer[ncount++]='.';} digitl {itype=1;}
    ;

var : entropy
    | minfo
    ;
term    : var {number=1;}
    | '-' var {number= -1;}
    | number var {if(itype)
              {buffer[ncount]='\0';number=atof(buffer);ncount=0;};}
    | '-' number var {if(itype)
              {buffer[ncount]='\0';number= -atof(buffer);ncount=0;}
              else
                  number= - number;}
    ;
expr    : term {advance();}
    | expr '+' term {advance();}
    | expr '-' term {number= -number;advance();}
    ;

eq  : expr '=' _TMP '0' {arglist->argtype=0;}
    | expr '<' '=' _TMP '0' {negate1();arglist->argtype=1;}
    | expr '>' '=' _TMP '0' {arglist->argtype=1;}
    | expr '=' _TMP expr {negate2();arglist->argtype=0;}
    | expr '<' '=' _TMP expr {negate1();arglist->argtype=1;}
    | expr '>' '=' _TMP expr {negate2();arglist->argtype=1;}
    ;
_TMP    : /* empty */ {boundary=current;}
    ;

rv  : 'A' {if (attrib[0])    attribute=attrib[0];
                else        {attribute=flag; attrib[0]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='A'; }}
    | 'B' {if (attrib[1])    attribute=attrib[1];
                else        {attribute=flag; attrib[1]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='B'; }}
    | 'C' {if (attrib[2])    attribute=attrib[2];
                else        {attribute=flag; attrib[2]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='C'; }}
    | 'D' {if (attrib[3])    attribute=attrib[3];
                else        {attribute=flag; attrib[3]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='D'; }}
    | 'E' {if (attrib[4])    attribute=attrib[4];
                else        {attribute=flag; attrib[4]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='E'; }}
    | 'F' {if (attrib[5])    attribute=attrib[5];
                else        {attribute=flag; attrib[5]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='F'; }}
    | 'G' {if (attrib[6])    attribute=attrib[6];
                else        {attribute=flag; attrib[6]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='G'; }}
    | 'H' {if (attrib[7])    attribute=attrib[7];
                else        {attribute=flag; attrib[7]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='H'; }}
    | 'I' {if (attrib[8])    attribute=attrib[8];
                else        {attribute=flag; attrib[8]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='I'; }}
    | 'J' {if (attrib[9])    attribute=attrib[9];
                else        {attribute=flag; attrib[9]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='J'; }}
    | 'K' {if (attrib[10])    attribute=attrib[10];
                else        {attribute=flag; attrib[10]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='K'; }}
    | 'L' {if (attrib[11])    attribute=attrib[11];
                else        {attribute=flag; attrib[11]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='L'; }}
    | 'M' {if (attrib[12])    attribute=attrib[12];
                else        {attribute=flag; attrib[12]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='M'; }}
    | 'N' {if (attrib[13])    attribute=attrib[13];
                else        {attribute=flag; attrib[13]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='N'; }}
    | 'O' {if (attrib[14])    attribute=attrib[14];
                else        {attribute=flag; attrib[14]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='O'; }}
    | 'P' {if (attrib[15])    attribute=attrib[15];
                else        {attribute=flag; attrib[15]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='P'; }}
    | 'Q' {if (attrib[16])    attribute=attrib[16];
                else        {attribute=flag; attrib[16]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='Q'; }}
    | 'R' {if (attrib[17])    attribute=attrib[17];
                else        {attribute=flag; attrib[17]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='R'; }}
    | 'S' {if (attrib[18])    attribute=attrib[18];
                else        {attribute=flag; attrib[18]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='S'; }}
    | 'T' {if (attrib[19])    attribute=attrib[19];
                else        {attribute=flag; attrib[19]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='T'; }}
    | 'U' {if (attrib[20])    attribute=attrib[20];
                else        {attribute=flag; attrib[20]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='U'; }}
    | 'V' {if (attrib[21])    attribute=attrib[21];
                else        {attribute=flag; attrib[21]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='V'; }}
    | 'W' {if (attrib[22])    attribute=attrib[22];
                else        {attribute=flag; attrib[22]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='W'; }}
    | 'X' {if (attrib[23])    attribute=attrib[23];
                else        {attribute=flag; attrib[23]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='X'; }}
    | 'Y' {if (attrib[24])    attribute=attrib[24];
                else        {attribute=flag; attrib[24]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='Y'; }}
    | 'Z' {if (attrib[25])    attribute=attrib[25];
                else        {attribute=flag; attrib[25]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='Z'; }}
    | 'a' {if (attrib[26])    attribute=attrib[26];
                else        {attribute=flag; attrib[26]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='a'; }}
    | 'b' {if (attrib[27])    attribute=attrib[27];
                else        {attribute=flag; attrib[27]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='b'; }}
    | 'c' {if (attrib[28])    attribute=attrib[28];
                else        {attribute=flag; attrib[28]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='c'; }}
    | 'd' {if (attrib[29])    attribute=attrib[29];
                else        {attribute=flag; attrib[29]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='d'; }}
    | 'e' {if (attrib[30])    attribute=attrib[30];
                else        {attribute=flag; attrib[30]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='e'; }}
    | 'f' {if (attrib[31])    attribute=attrib[31];
                else        {attribute=flag; attrib[31]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='f'; }}
    | 'g' {if (attrib[32])    attribute=attrib[32];
                else        {attribute=flag; attrib[32]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='g'; }}
    | 'h' {if (attrib[33])    attribute=attrib[33];
                else        {attribute=flag; attrib[33]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='h'; }}
    | 'i' {if (attrib[34])    attribute=attrib[34];
                else        {attribute=flag; attrib[34]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='i'; }}
    | 'j' {if (attrib[35])    attribute=attrib[35];
                else        {attribute=flag; attrib[35]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='j'; }}
    | 'k' {if (attrib[36])    attribute=attrib[36];
                else        {attribute=flag; attrib[36]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='k'; }}
    | 'l' {if (attrib[37])    attribute=attrib[37];
                else        {attribute=flag; attrib[37]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='l'; }}
    | 'm' {if (attrib[38])    attribute=attrib[38];
                else        {attribute=flag; attrib[38]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='m'; }}
    | 'n' {if (attrib[39])    attribute=attrib[39];
                else        {attribute=flag; attrib[39]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='n'; }}
    | 'o' {if (attrib[40])    attribute=attrib[40];
                else        {attribute=flag; attrib[40]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='o'; }}
    | 'p' {if (attrib[41])    attribute=attrib[41];
                else        {attribute=flag; attrib[41]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='p'; }}
    | 'q' {if (attrib[42])    attribute=attrib[42];
                else        {attribute=flag; attrib[42]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='q'; }}
    | 'r' {if (attrib[43])    attribute=attrib[43];
                else        {attribute=flag; attrib[43]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='r'; }}
    | 's' {if (attrib[44])    attribute=attrib[44];
                else        {attribute=flag; attrib[44]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='s'; }}
    | 't' {if (attrib[45])    attribute=attrib[45];
                else        {attribute=flag; attrib[45]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='t'; }}
    | 'u' {if (attrib[46])    attribute=attrib[46];
                else        {attribute=flag; attrib[46]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='u'; }}
    | 'v' {if (attrib[47])    attribute=attrib[47];
                else        {attribute=flag; attrib[47]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='v'; }}
    | 'w' {if (attrib[48])    attribute=attrib[48];
                else        {attribute=flag; attrib[48]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='w'; }}
    | 'x' {if (attrib[49])    attribute=attrib[49];
                else        {attribute=flag; attrib[49]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='x'; }}
    | 'y' {if (attrib[50])    attribute=attrib[50];
                else        {attribute=flag; attrib[50]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='y'; }}
    | 'z' {if (attrib[51])    attribute=attrib[51];
                else        {attribute=flag; attrib[51]=flag; flag=flag<<1;
                 rvnames[vcount++][0]='z'; }}
        ;

rvlist  : rv {field[fcount]=attribute;}
        | rvlist rv {field[fcount]|=attribute;}
        | rvlist ',' rv {field[fcount]|=attribute;}
        ;

rvcore  : rvlist ';' _ADDF rvlist
        | rvcore ';' _ADDF rvlist
        ;

entropy : 'H' '(' rvlist ')' {condflag=0;}
    | 'H' '(' rvlist _ADDF '|' rvlist ')' {condflag=1;}
    ;

minfo   : 'I' '(' rvcore ')' {condflag=0;}
    | 'I' '(' rvcore '|' _ADDF rvlist ')' {condflag=1;}
    ;

_ADDF   : /* empty */ {fcount++;}
        ;

macro1  : rvlist ':' _MAC1 rvlist
    ;
macro2  : rvlist '.' _MAC1 rvlist
    | macro2 '.' _MAC2 rvlist
    ;
macro3  : rvlist '/' _MAC1 rvlist '/' _MAC2 rvlist
    | macro3 '/' _MAC2 rvlist {extrainput++;}
    ;
_MAC1   : /* empty */ {delimiters[0]=c;limcount=1;}
    ;
_MAC2   : /* empty */ {delimiters[limcount++]=c;}
    ;
%%
void expandmacro1()
{
    char temp[2048];
    input[delimiters[0]-1] = '|';
    temp[0] = 'H';
    temp[1] = '(';
    temp[2] = '\0';
    strcat(temp, input);
    strcat(temp, ")=0");
    strcpy(input, temp);
    c = 0;
    yyparse();
}

void expandmacro2()
{
    char temp[2048];
    int l, j;
    for (l = 0; l < limcount; l++)
        input[delimiters[l]-1] = ' ';
    temp[0] = 'H';
    temp[1] = '(';
    temp[2] = '\0';
    strcat(temp, input);
    strcat(temp, ")=H(");
    for (l = 0, j = 0; l < limcount; l++)
    {
        input[delimiters[l]-1] = '\0';
        strcat(temp, input+j);
        strcat(temp, ")+H(");
        j = delimiters[l];
    }
    strcat(temp, input+j);
    strcat(temp, ")");
    strcpy(input, temp);
    c = 0;
    yyparse();
}

void expandmacro3()
{
    char temp[2048], backup[2048], *p;
    int l, j;
    if (argnumber == 0)
        multi = limcount - 1;

    strcpy(backup, input);
    for (l = 0; l < limcount; l++)
        backup[delimiters[l]-1] = '\0';
    temp[0] = 'I';
    temp[1] = '(';
    p = temp + 2;
    for (l = 1; l < limcount; l++)
    {
        *p = '\0';
        strcat(p, backup);
        strcat(p, "; ");
        strcat(p, backup+delimiters[l]);
        strcat(p, "|");
        strcat(p, backup+delimiters[l-1]);
        strcat(p, ")=0");

        strcpy(input, temp);
        c = 0;
        yyparse();

        arglist->next = (struct EQNLIST*) malloc(sizeof(struct EQNLIST));
        arglist = arglist->next;
        arglist->next = NULL;

        if (l != (limcount-1))
        {
            arglist->eqn = (struct EQN*) malloc(sizeof(struct EQN));
            eqn = arglist->eqn;
            eqn->next = NULL;
            current = eqn;

            backup[delimiters[l-1]-1] = ' ';
        }
        else
            arglist->eqn = NULL;
    }
}

void allocate()
{
    current->next = (struct EQN*) malloc(sizeof(struct EQN));
    current = current->next;
    current->next = NULL;
}

int advance()
{
    char tail, i, j, p, notdone;
    long int mask, temp;
    int bptr, bcount, btemp, bfinal;
    tail = fcount;
    if (condflag)
        tail--;
    notdone = 1;
    p = 0;
    for (j = 1; j <= tail; j++)
    {
        for (i = 0; i <= p; i++)
        {
            mask = field[i] & field[j];
            if (mask == field[i] || mask == field[j])
            {
                field[i] = mask;
                break;
            }
        }
        if (i > p)
            field[++p] = field[j];
    }

    if (condflag)
    {
        mask = ~field[fcount];
        for (i = 0; i <= p; i++)
        {
            temp = field[i] & mask;
            if (temp)
                field[i] = temp;
            else
            {
                notdone = 0;
                break;
            }
        }
    }

    if (notdone)
    {
        if (!p)
        {
            if (condflag)
            {
                current->coef = number;
                current->variable = field[0] | field[fcount];
                allocate();
                current->coef = - number;
                current->variable = field[fcount];
            }
            else
            {
                current->coef = number;
                current->variable = field[0];
            }
            allocate();
        }
        else
        {
            bfinal = (1L << (p+1)) - 1;
            if (condflag)
                mask = field[fcount];
            else
                mask = 0;

            for (bcount = 1; bcount <= bfinal; bcount++)
            {
                btemp = (bcount & 0xFFFF) ^ (bcount >> 16);
                btemp = (btemp & 0xFF) ^ (btemp >> 8);
                btemp = (btemp & 0xF) ^ (btemp >> 4);
                btemp = (btemp & 0x3) ^ (btemp >> 2);
                btemp = (btemp & 0x1) ^ (btemp >> 1);

                temp = mask;
                for (bptr = 0; bptr <= p; bptr++)
                    if (bcount & (1L << bptr))
                        temp |= field[bptr];

                if (btemp)
                    current->coef = number;
                else
                    current->coef = - number;
                current->variable = temp;

                allocate();
            }

            if (condflag)
            {
                current->coef = - number;
                current->variable = mask;
                allocate();
            }
        }
    }

    fcount = 0;
    return 1;
}

int negate1()
{
    current = eqn;
    while (current != boundary)
    {
        current->coef = - current->coef;
        current = current->next;
    }
    return 1;
}

int negate2()
{
    current = boundary;
    while (current->next != NULL)
    {
        current->coef= -current->coef;
        current = current->next;
    }
    return 1;
}

void free_eqn()
{
    struct EQN *temp;
    int count;

    arglist = arghead;
    while (arglist != NULL)
    {
        current = arglist->eqn;
        while (current != NULL)
        {
            temp = current->next;
            free(current);
            current = temp;
        }
        arghead = arglist;
        arglist = arglist->next;
        free(arghead);
    }
}

void new_rv(long tag)
{
    int index, oldtotal;
    long part0, part1, part2;
    if (tag & tagmask)
    {
        tagmask = tagmask | tag;
        oldtotal = rvtotal;
        index = 0;
        while (tag)
        {
            if (index == oldtotal)
            {
                rvtag[rvtotal++] = tag;
                tag = 0;
            }
            else
            {
                part0 = rvtag[index] & tag;
                if (part0)
                {
                    part1 = rvtag[index] & (~tag);
                    part2 = (~rvtag[index]) & tag;

                    rvtag[index++] = part0;
                    if (part1)
                        rvtag[rvtotal++] = part1;
                    tag = part2;
                }
                else
                    index++;
            }
        }
    }
    else
    {
        tagmask = tagmask | tag;
        rvtag[rvtotal++] = tag;
    }
}

void reduce_rv()
{
    current = eqn;
    while (current->next != NULL)
    {
        new_rv(current->variable);
        current = current->next;
    }
}

void split_rv(long int *p)
{
    int index;
    long int current_bit, temp, output;

    temp = *p;
    output = 0;

    for (index = 0, current_bit = 1; index < rvtotal; index++, current_bit <<= 1)
        if (temp & rvtag[index])
        {
            output = output | current_bit;
            temp = temp & (~rvtag[index]);
        }

    *p = output;
}

void map_rv()
{
    current = eqn;
    while (current->next != NULL)
    {
        split_rv(&(current->variable));
        current = current->next;
    }
}

void expand(double *coef)
{
    current = eqn;
    while (current->next != NULL)
    {
        coef[current->variable - 1] += current->coef;
        current = current->next;
    }
}

int yylex()
{
    while (input[c] == ' ')
        c++;
    return(input[c++]);
}

int yyerror(char *s)
{
    struct EQN *temp;
    free_eqn();
    status = 1;
    return(-1);
}


int ITIP(char **expressions, int number_expressions) {
    int temp;
    long b;
    int *intype;
    double **inmatrix;
    int i, j;
    glp_prob* lp;
    int row_length;
    int *row_indices;
    double *row_values, *x; /*every constraint has a 0 on the right-hand-side*/
    int row_sense;
    int result = 1, outcome, status;
    int row, col;
    glp_smcp parm;

    for (temp = 0; temp < 52; temp++) {
        attrib[temp] = 0;
        rvnames[temp][1] = '\0';
    }

    flag = 1;
    vcount = 0;
    status = 0;
    extrainput = 0;
    multi = 1;
    /*
      multi tells us how many objective functions there are.
      The objective function is always the first input string.
      However, if the first input string is a Markov chain (type 3 macro) involving 4 or
      more groups of variables, e.g. AB/C/U/XY, then this can not
      be expressed using a single objective function.
    */

    arghead = (struct EQNLIST*) malloc(sizeof(struct EQNLIST));
    arglist = arghead;
    arglist->eqn = NULL;
    arglist->next = NULL;

    input = (char*) malloc(1000*sizeof(char));

    /* parse the expressions one by one: */
    for (argnumber = 0; argnumber < number_expressions; argnumber++) {
        ncount = 0;
        fcount = 0;
        c = 0;

        if (strlen(expressions[argnumber]) >= 900) {
            input = realloc(input, 2*strlen(expressions[argnumber])*sizeof(char));
        }
        strcpy(input, expressions[argnumber]);

        arglist->eqn = (struct EQN*) malloc(sizeof(struct EQN));
        eqn = arglist->eqn;
        eqn->next = NULL;
        current = eqn;
        macrodetect = 0;
        if (yyparse())
        {
            return (-2-argnumber);  // syntax error in expression
        }


        if (status) {
            free_eqn();
            free(input);
            return 20; /* any value larger than 6 will lead to an "unexpected error" message */
        }
        switch(macrodetect)
        {
            case 1: expandmacro1(); break;
            case 2: expandmacro2(); break;
            case 3: expandmacro3();
        }


        if (macrodetect != 3)
        {
            arglist->next = (struct EQNLIST*) malloc(sizeof(struct EQNLIST));
            arglist = arglist->next;
            arglist->eqn = NULL;
            arglist->next = NULL;
        }
    }
    numofinput = argnumber;
    /*end of file-reading and parsing*/

    /*always do optimization (collapses variables that always appear together)*/
    rvtotal = 0;
    tagmask = 0;
    arglist = arghead;
    while (arglist != NULL) {
        if (arglist->eqn != NULL)
        {
            eqn = arglist->eqn;
            reduce_rv();
        }
        arglist = arglist->next;
    }

    if (flag > 1L<<rvtotal) {
        arglist = arghead;
        while (arglist != NULL) {
            if (arglist->eqn != NULL)
            {
                eqn = arglist->eqn;
                map_rv();
            }
            arglist = arglist->next;
        }
        flag = 1L<<rvtotal;
    }
    /*end of optimization*/

    /*flag-1 is the number of dimensions in the vector representation space*/
    /*numofinput+extrainput is the number of equations*/
    inmatrix = (double **) malloc((numofinput+extrainput)*sizeof(double*));
    for (i = 0; i < numofinput+extrainput; i++) {
        /*inmatrix contains the vector representation of one equation in each row*/
        inmatrix[i] = (double*) malloc((flag-1)*sizeof(double));
        /* first set all the entries to zero:*/
        for (j = 0; j < flag-1; j++) {
            inmatrix[i][j] = 0;
        }
    }

    /*write the equations into inmatrix (one per row):*/
    argnumber = 0;
    arglist = arghead;
    while (arglist != NULL)
    {
        if (arglist->eqn != NULL)
        {
            eqn = arglist->eqn;
            expand(inmatrix[argnumber]);
        }
        argnumber++;
        arglist = arglist->next;
    }

    /*in intype, we store whether the equation is an equality (==0) or an inequality (==1)*/
    intype = (int*) malloc((numofinput+extrainput)*sizeof(int));
    argnumber = 0;
    arglist = arghead;
    while (arglist != NULL)
    {
        if (arglist->eqn != NULL)
            intype[argnumber] = arglist->argtype;
        argnumber++;
        arglist = arglist->next;
    }

    free_eqn();

    for (b = multi; b <= numofinput+extrainput-1; b++)
        if (intype[b])
        {
            b = -1;
            break;
        }

    if (b == -1)
    {
        // Constraints cannot be inequalities.
        for (i = 0; i < numofinput+extrainput; i++) {
            free(inmatrix[i]);
        }
        free(inmatrix);
        free(intype);
        free(input);
        return 5;
    }


    /*LINEAR PROGRAMMING PART:*/

    /*x is here to hold the optimal solutions:*/
    x = (double*) malloc((flag-1) * sizeof(double));


    /*construct the linear program object:*/
    /*and set the optimization-direction:*/
    lp = glp_create_prob();
    glp_set_prob_name(lp, "ITIP_problem");
    glp_set_obj_dir(lp, GLP_MIN);


    /*add all primal variables; we do not yet set their coefficients in the objective function:*/
    for (j = 0; j < flag-1; j++) {
        col = glp_add_cols(lp, 1);
        glp_set_col_bnds(lp, col, GLP_LO, 0, NAN);
    }

    row_indices = (int*) malloc((flag-1)*sizeof(int));
    row_values = (double*) malloc((flag-1)*sizeof(double));
    /*add all the user-provided constraints to the constraint matrix:*/
    for (i = multi; i < numofinput+extrainput; i++) { /*numofinput+extrainput is the number of rows in inmatrix */
    /*starts at multi because the first multi rows of inmatrix are the objective functions*/
        row_length = 0;
        for (j = 0; j < flag-1; j++) { /*for every element in the row*/
            if (inmatrix[i][j] != 0) {
                /*append elements to lp constraint row:*/
                row_values[row_length] = inmatrix[i][j];
                row_indices[row_length] = j+1;
                row_length++;
            }
            /* else (if inmatrix[i][j] is zero), do nothing */
        }
        /*decide whether the current row is an equality or an inequality:*/
        if (intype[i] == 1) { /*if the row is an inequality*/
            row_sense = GLP_UP;
        }
        else{ /*if the row is an equality*/
            row_sense = GLP_FX; /*row must be less than 0*/
        }
        row = glp_add_rows(lp, 1);
        glp_set_row_bnds(lp, row, row_sense, 0.0, 0.0);
        glp_set_mat_row(lp, row, row_length, row_indices-1, row_values-1);
    }

    /*add all the Shannon-type constraints:*/
    add_elemental_inequalities(lp, rvtotal);


    /*for every objective function, we check the following:*/
    for (objcount = 0; objcount < multi; objcount++) {

        /*set the objective function for the columns:*/
        for (j = 1; j < flag; j++) {
            glp_set_obj_coef(lp, j, inmatrix[objcount][j-1]);
        }

        /*solve linear program:*/
        glp_init_smcp(&parm);
        parm.msg_lev = GLP_MSG_ERR;
        outcome = glp_simplex(lp, &parm);
        if (outcome != 0) {
            // TODO: return error
        }
        status = glp_get_status(lp);

        /*test whether an optimal solution has been found:*/
        if (status != GLP_OPT) {
            result = 0;
        }
        else{ /*if the solution is optimal, check whether it is all-zero*/
            // the original check was for the solution (primal variable values)
            // rather than objective value, but let's do it simpler for now:
            // (if an optimum is found, it should be zero anyway)
            if (glp_get_obj_val(lp) != 0.0) {
                result = 0;
            }
        }

        /*if this first test was positive:*/
        if (result == 1) {
            /*check whether a second test is necessary (this is the case if we have to check for equality)*/
            if (intype[0] == 0) {
                /*change the objective function in the LPX object:*/
                for (j = 1; j < flag; j++) {
                    glp_set_obj_coef(lp, j, - inmatrix[objcount][j-1]);
                }

                /*solve linear program:*/
                outcome = glp_simplex(lp, &parm);
                if (outcome != 0) {
                /*    puts("function QSopt_primal returned an error!"); */
                }
                status = glp_get_status(lp);

                /*test whether an optimal solution has been found:*/
                if (status != GLP_OPT) {
                    result = 0;
                }
                else{  /*if the solution is optimal, check whether it is all-zero*/
                    if (glp_get_obj_val(lp) != 0.0) {
                        result = 0;
                    }
                }
            }
        }
    }/*end objcount loop*/

    /*free memory*/
    free(row_indices);
    free(row_values);
    free(x);
    glp_delete_prob(lp);
    for (i = 0; i < numofinput+extrainput; i++) {
        free(inmatrix[i]);
    }
    free(inmatrix);
    free(intype);

    free(input);

    return result;
}
