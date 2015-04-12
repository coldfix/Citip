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
 *  itip1.l is the preparser lex file. lex/flex is required 
 *  to compile this
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
 *  The program Xitip uses two other softwares
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


%option noyywrap
%option prefix="itipyacc"
%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ITIP.h"
#define WordLength 300

//int itip(char **expressions, int num_expressions);
struct doc_words
{
char words[WordLength];
int index;
struct doc_words *next;
};

struct doc_words *head=NULL;;
int i;
int word_error = 0;

struct doc_words *getmem()
       {
        struct doc_words *p;
        p = malloc(sizeof(struct doc_words));
        //p = (struct doc_words*)malloc(sizeof( doc_words));
        if(!p)
              {
            /* printf("out of memory");*/
               return(0);
              }
        else
        return(p);
       }

int FindWords(char *text){
/* return the index of the text in the list if text is already present 
   in the dictionary. Otherwise, return -1.
*/
    struct doc_words *Dictionary;
    int found=0;
    int iIndex=0;
    Dictionary=head; //Take the updated dictionary and call it "Dictionary "
while ((Dictionary != NULL) && !found) { //If non empty and if no match found yet
		if (!strcmp(Dictionary->words, text)) { //If a match is found 
			found = 1;       // Flag that a match has been found in the dictionary
                        iIndex=Dictionary->index;// Index of the matching word in dictionary
		} 
                else {                                 // If no match found yet, try next element
			Dictionary = Dictionary->next; /*If no match found, get ready 
                                                         to point to the next element 
                                                         in the dictionary list */
                        //found=0;
		}

	}
if (found==1){ //When a word is found matching in the dictionary,return the index 
    return iIndex;
}
else {
 return -1;
}
}

int AddWords( char *text) 
{
struct doc_words* newNode;

    newNode=getmem();
    strncpy(newNode->words,text,strlen(text)+1);
    newNode->next=head;
    if(head == NULL){
      newNode->index = 0;
    }
    else{
      newNode->index = (head->index) + 1;
    }
    head=newNode;
return newNode->index;
};

int destroy_doc_words(){
  struct doc_words* tmp;
  while(head != NULL){
    tmp = head->next;
    free(head);
    head = tmp;
  }
  head=NULL;

  return 0;
}


%}
%%
[I][\(] {fprintf(yyout,"I(");}
[H][\(] {fprintf(yyout,"H(");}
[a-zA-Z][a-zA-Z0-9_']* {

  /*check whether yytext is not too lont:*/
  if(strlen(yytext) >= WordLength){ /*there must be space for the \0 character*/
    word_error = 4; /* this will cause itip() to quit without calling ITIP */
  }
  else{
    /*check whether the yytext is in the table, if yes, find position 
      in table, output corresponding letter (A,B,...); if not
      insert into table, output corr. letter.
     */
    if(head == NULL) {
       i=-1;            /*First word; Very first word is read 
                           and add to the dictionary*/
    }
    else{
      i = FindWords(yytext);  /*Pass the index of the matched word in 
                          the dictionary*/
    }
    if(i == -1){
     i = AddWords(yytext);   /* Add the words which are not yet present 
                           in Dictionary (indicated by i=-1) to 
                           Dictionary */
    }

    //  If the number of distinct words (random variables) are more than
    //  26, then use small letter (English alphabet) to map the random 
    //  variables, while parsing. For the first 26 distinct random variables
    //  the parser maps out them to distinct letters of the English 
    //  (large) alphabet. In all, a maximum of 52 different random variables
    //  can thus be used. This number is significantly higher than, the
    //  useful information expressions (atleast demanded so far). Moreover, the
    //  computing speed limits, restrict the usage of random variables, more than
    // these (even 20 or more random variables,demand higher computing resources.
    if(i>51){ /* If there are too many words: */
      word_error = 3; /* this will cause itip() to quit without calling ITIP */
    }
    
    if(i>25){
      fprintf(yyout,"%c",'a'+i-26);
    }
    else
    {
      fprintf(yyout,"%c",'A'+i);
    }
  }

}
%%

int itip1(char **expressions, int num_expressions)
{
int temp_result;
FILE *f2;
int ii;
int jj;
int kindex;
char **expressions1;
FILE *f1;
int inumber;
int result;

word_error = 0;

expressions1 = (char **)malloc( (num_expressions) *sizeof(char *));
for(kindex=0;kindex<num_expressions;kindex++)
    {
    expressions1[kindex] = (char *)malloc( (strlen(expressions[kindex]) + 1) * sizeof(char )); //new +1
    
    }
    
    
f1 = fopen("expout.txt","w");
for (inumber=0;inumber <num_expressions;inumber++)
{
    fprintf(f1,"%s\n",expressions[inumber]);
}
fclose(f1);


yyin = fopen("expout.txt","r");
yyout = fopen("Output.txt","w");

yylex();

fclose(yyin);
fclose(yyout);

/* if an error occurred, we do not call ITIP: */
if(word_error != 0){
  destroy_doc_words();
  return word_error;
}

f2 = fopen("Output.txt","r");
    ii=0;

while (!feof(f2)) 
{
    fscanf(f2,"%s\n",expressions1[ii]);
    ii++;
}
fclose(f2);

unlink("Output.txt");
unlink("expout.txt");

result = ITIP(expressions1,num_expressions);

/*free the dictionary:*/
destroy_doc_words();



return result;
}
/* error codes:
3: too many words
4: some word is too long
*/
