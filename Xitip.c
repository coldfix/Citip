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




/*
gcc -c make_D.c
yacc ITIP.y
cp y.tab.c ITIP.c
gcc -c ITIP.c
gcc -c -Wall  `pkg-config --cflags gtk+-2.0`  guiITIP.c
gcc -lm `pkg-config --libs gtk+-2.0` guiITIP.o ITIP.o make_D.o qsopt.a -o gui_ITIP
#include "epfl_orig.xpm"
#include "shannon.xpm"
 */

#include <gtk/gtk.h>
#include "xlogo.xpm"
#include "ipg_logo.xpm"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include "itip1.h"

typedef struct
{
GtkWidget  *entry1, *textview;
} Widgets;
static void callITIP (GtkButton*, Widgets*);
static void callABOUT ();
static void destroy (GtkWidget*, gpointer);
static void callHELP ();


/*This function removes all tabs, spaces and newlines from a string:*/
gchar * remove_white_space(char *string){
  gchar *temp_string;
  int k, length,ktemp;
  
  length = strlen(string)+1;
  temp_string = (char *)malloc(length*sizeof(gchar));
  
  ktemp = 0;
  for(k=0;k<length;k++){
    if(string[k] == ' ' || string[k] == '\t' || string[k] == '\n'){
      /* do nothing */
    }
    else{
      temp_string[ktemp] = string[k];
      ktemp++;
    }
  }

  return temp_string;
}

int main (int argc,char *argv[])
{
  GtkWidget *window, *scrolled_win, *vbox, *hbox, *find, *page, *about, *help;
Widgets *w = g_slice_new (Widgets);
GtkTextBuffer *buffer;
gtk_init (&argc, &argv);
window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
gtk_window_set_title (GTK_WINDOW (window), "Xitip");

GtkWidget *menubar,*menuitem,*menuitem_menu,*about1,*vbox_main;

menubar = gtk_menu_bar_new ();
menuitem = gtk_menu_item_new_with_mnemonic ("_Help");
  menuitem_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menuitem_menu);
  about1 = gtk_menu_item_new_with_mnemonic ("_About");

  vbox_main = gtk_vbox_new (FALSE, 0);

gtk_container_set_border_width (GTK_CONTAINER (window), 10);
page = gtk_label_new ("Welcome to the ITIP Solver");

g_signal_connect (G_OBJECT (window), "destroy",G_CALLBACK (destroy), NULL);
GtkWidget *image;
GdkPixmap *icon;
GdkPixmap *icon_mask;
GdkColormap *colormap;
GtkStyle *style;
w->entry1 = gtk_entry_new ();
w->textview = gtk_text_view_new ();


buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (w->textview));
gtk_entry_set_text (GTK_ENTRY (w->entry1), "I(X;Y)+1.23 H(Eric_Clapton|Lausanne_Snows)+0.123 I(X;Z|Q)>=0");
find = gtk_button_new_from_stock ("_Check");
about = gtk_button_new_from_stock (GTK_STOCK_ABOUT);
g_signal_connect (G_OBJECT (find), "clicked",G_CALLBACK (callITIP),(gpointer) w); // When clicked ths function search() is involked 
g_signal_connect (G_OBJECT (about), "clicked",G_CALLBACK (callABOUT),NULL); // When clicked ths function search() is involked 
scrolled_win = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_size_request (scrolled_win, 800, 200); 
gtk_container_add (GTK_CONTAINER (scrolled_win), w->textview);

hbox = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox), w->entry1, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (hbox), find, FALSE, TRUE, 0);

vbox = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
gtk_container_add (GTK_CONTAINER (window), vbox);
gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 6);

style=gtk_widget_get_style(window);
colormap=gtk_widget_get_colormap(window);
icon=gdk_pixmap_colormap_create_from_xpm_d(GTK_WIDGET(window)->window,colormap,&icon_mask,&style->bg[GTK_STATE_NORMAL],(gchar **)xlogo);// The xpm file name is new.xpm

image=gtk_pixmap_new(icon,icon_mask);
//image1=gtk_pixmap_new(icon1,icon_mask1);
gtk_widget_show(image);
gtk_box_pack_start (GTK_BOX (vbox), image, FALSE, TRUE, 6);

GtkWidget *hbox1;
help = gtk_button_new_from_stock ("_Help");
hbox1 = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox1), help, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), about, FALSE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, TRUE, 6);

/* gtk_container_add (GTK_CONTAINER (vbox), hbox1); */

g_signal_connect (G_OBJECT (help), "clicked",G_CALLBACK (callHELP),NULL); // When clicked ths function search() is involked 
gtk_widget_show_all (window);
g_object_unref(icon);
g_object_unref(icon_mask);
gtk_main();
return 0;
}


// ITIP Function 
static void
callITIP (GtkButton *button,Widgets *w)
{
  int result;
  gint number_lines,linecount,number_expressions,exprcount;
  const gchar *find;
  gchar **expressions, **expressions_clean, *temp_string;
  gchar *output;
  gchar *output_synerr;
  const gchar *test_string;  // for debug only
  GtkWidget *dialog;
  GtkTextBuffer *buffer;
  GtkTextIter start, end;
  find = gtk_entry_get_text (GTK_ENTRY (w->entry1));
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (w->textview));
  
  number_lines = gtk_text_buffer_get_line_count(buffer);
  expressions = (gchar **)malloc( (number_lines+1) *sizeof(gchar *));// allocate space for all the constrains AND the expression
  expressions_clean = (gchar **)malloc( (number_lines+1) *sizeof(gchar *));
  
  expressions[0] = (gchar *)gtk_entry_get_text (GTK_ENTRY (w->entry1));

  for(linecount = 1; linecount<number_lines; linecount++){
    gtk_text_buffer_get_iter_at_line(buffer,&start,linecount-1);
    gtk_text_buffer_get_iter_at_line(buffer,&end,linecount);
    expressions[linecount] = gtk_text_buffer_get_text(buffer,&start,&end,FALSE);
  }
  gtk_text_buffer_get_iter_at_line(buffer,&start,number_lines-1);
  gtk_text_buffer_get_end_iter(buffer,&end);
  expressions[number_lines] = gtk_text_buffer_get_text(buffer,&start,&end,FALSE);

  /*remove all \n at the end of the strings, also remove empty strings:*/
  exprcount = 0;
  for(linecount = 0; linecount <= number_lines; linecount++){
    temp_string = (gchar *)remove_white_space(expressions[linecount]);
    expressions[linecount] = temp_string;
    /*skip the expression if it is empty:*/
    if(strlen(expressions[linecount])>0){
      expressions_clean[exprcount] = expressions[linecount];
      exprcount++;
    }
  }
  number_expressions = exprcount;

  /* if the expression is empty give an exit warning to enter a valid
   * expression. When an expression not entered, but some constraints
   * are provided, that as well to be flaged out. When the expression
   * field is empty,do not call itip, but just flag this to the user.
   * */
  if ((strlen(expressions[0]))){
    result = itip1(expressions_clean,number_expressions);
  }
  else {
    result=2; //No expression enteredin the field.
  }

  if(result ==2)
    test_string="You must enter a valid expression";
  else if(result==1)
    test_string = "TRUE";
  else
    test_string = "Not solvable by Xitip: This implies either of the following situations\n 1.\t The inequality is NOT true\n or\n 2.\t This expression is a non-Shannon type inequality which is true.\n \t Currently Xitip is equipped enough to verify only the Shannon type inequalities";
  
  if (result==2){
    output = g_strdup_printf ("The information expression is EMPTY!\n You must enter a valid information expression in the first field");
  }
  else if (number_expressions>1){
    output = g_strdup_printf ("The information expression (with the given constraints)\n %s is\n %s", expressions[0], test_string);
  }
  else{
    output = g_strdup_printf ("The information expression (without any further constraint)\n %s is\n %s", expressions[0], test_string);
  }

  if(result ==-2){
    output_synerr = g_strdup_printf ("Syntaxt ERROR: Re-enter the information expression \n %s", expressions[0]);
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, output_synerr, NULL);
  }
  else if(result == 5){
    output_synerr = "ERROR: The constraints cannot be inequalities.\n";
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, output_synerr, NULL);
  }
  else if(result == 3){
    output_synerr = "ERROR: The number of distinct random variables cannot be more than 52.\n";
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, output_synerr, NULL);
  }
  else if(result == 4){
    output_synerr = "ERROR: Some of the random variables are too long. The maximal length allowed for a single random variable name is 300.\n";
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, output_synerr, NULL);
  }
  else if(result <=-3){
    output_synerr = g_strdup_printf ("Syntax ERROR:Constraint %d has wrong syntax.\n Please re-enter\n %s\n with the correct syntax\n",-1*result-2, expressions[-1*result-2]);
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, output_synerr, NULL);
  }
  else if(result==-1)
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,GTK_BUTTONS_OK, "You have entered a basic Shannon measure. These measures, without sign are always non negative! Basic Information measures are in the form of entropies (single, joint or conditional) and mutual information(unconditional or conditional). For example the following are basic shannon measures (for 3 random variables X,Y and Z) which are non-nagative by definition\nH(X)>=0, \nH(X,Y)>=0, \nI(X;Y)>=0, \nI(X;Y|Z)>=0,\nH(X|Y)>=0. \nIf you have entered an expression as a single negative constant times these basic Shannon measure, then the inequality is always Non true, beacuase of the non negativity of Shannon measures", NULL);
  else if(result==2){
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING,GTK_BUTTONS_OK, output, NULL);
  }
  else if(result==1){
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,GTK_BUTTONS_OK, output, output);
  }
  else if(result==0){
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING,GTK_BUTTONS_OK, output, NULL); 
  }
  else{
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, "An unexpected error has occurred.", NULL);
  }
   
  

  gtk_window_set_title (GTK_WINDOW (dialog), "Information Inequality Result");
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  g_free (output);
}

/* Stop the GTK+ main loop function when the window is destroyed. */
static void
destroy (GtkWidget *window,gpointer data)
{
gtk_main_quit ();
}
// callABOUT Function 
static void
callABOUT (void)
{
GtkWidget *dialog;

GdkPixbuf *logo;
GError *error = NULL;

//const gchar *comments = "Xitip";
//const gchar *name = "Xitip";
const gchar *version = "1.0";
const gchar *copyright = "(C) 2007-2008 Rethnakaran Pulikkoonattu, Etienne Perron, Suhas Diggavi";
const gchar *authors[] = {
"Rethnakaran Pulikkoonattu",
"Etienne Perron",
"Suhas Diggavi",
"\nThe algorithm used in this software\nwas proposed by Raymond W. Yeung\nand Ying-On Yan.\nXitip is an adaptation of their software,\ncalled ITIP, which originally uses the\nlinear programming solver in MATLAB's\nOptimization Toolbox. Xitip uses a\nC-based linear programming solver instead.\nThe core code for parsing information\ninequalities and setting up the\ncorresponding linear program stems from ITIP.\nhttp://user-www.ie.cuhk.edu.hk/~ITIP/\n\nThe rest of the software was developed by\nRethnakaran Pulikkoonattu\nEtienne Perron\nSuhas Diggavi.\nIn particular, the original code was made\nindependent\nof Matlab, a pre-parser and a\ngraphical user-interface\nwas added.\nhttp://xitip.epfl.ch\n\nThe linear programming library used (QSopt)\nwas developed by David Applegate,\nWilliam Cook, Sanjeeb Dash, and\nMonika Mevenkamp.\nhttp://www2.isye.gatech.edu/~wcook/qsopt/\n",
NULL
};
/* const gchar *documenters[] = { */
/* "Rethnakaran Pulikkoonattu", */
/* "Etienne Perron", */
/* "Suhas Diggavi", */
/* NULL */
/* }; */
dialog = gtk_about_dialog_new ();
logo = gdk_pixbuf_new_from_xpm_data ((const char **) ipg_logo); // Embed the image onto the binary

GdkColormap *colormap;
GdkPixmap *icon;
GtkWidget *image;
GdkPixmap *icon_mask;
GtkStyle *style;
style=gtk_widget_get_style(dialog);
colormap=gtk_widget_get_colormap(dialog);
icon=gdk_pixmap_colormap_create_from_xpm_d(NULL,colormap,&icon_mask,&style->bg[GTK_STATE_NORMAL],(gchar **)ipg_logo);
image=gtk_pixmap_new(icon,icon_mask);


/* Set the application logo or handle the error. */
if (error == NULL)
gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), logo);
else
{
if (error->domain == GDK_PIXBUF_ERROR)
g_print ("GdkPixbufError: %s\n", error->message);
else if (error->domain == G_FILE_ERROR)
g_print ("GFileError: %s\n", error->message);
else
g_print ("An error in the domain: %d has occurred!\n", error->domain);
g_error_free (error);
}
/* Set application data that will be displayed in the main dialog. */
gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), version);
gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog),copyright);
/* Set the license text, which is usually loaded from a file. Also, set the * web site address and label. */
gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), "\t\t\t\t\t\tLICENSE\n\t\t\t\t\t\t======\n\nThis program is free, open-source software. You may redistribute\nit and/or modify it under the terms of the GNU General Public\nLicense (GPL) as published by the Free Software Foundation.\nhttp://www.gnu.org/licenses/gpl.html\n\nThe linear programming library used (QSopt) is NOT open-source.\nIt can be used at no cost for research or education purposes; all\nrights to QSopt are maintained by the authors, David Applegate,\nWilliam Cook, Sanjeeb Dash, and Monika Mevenkamp. More information\ncan be found on the QSopt webpage:\nhttp://www2.isye.gatech.edu/~wcook/qsopt/\n\nA completely open-source version (using the Gnu LP Kit instead of\nQSopt) is also available on our webpage:\nhttp://xitip.epfl.ch\n");
gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog),"http://xitip.epfl.ch");
/* Set the application authors, documenters and translators. */
gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);

gtk_dialog_run (GTK_DIALOG (dialog));
gtk_widget_destroy (dialog);
}

void callHELP  ()
{
  GtkWidget *helpwindow, *help_scrolled_win, *help_textview;
  GtkTextBuffer *help_buffer;
  char help_string[3000]; // Do an "ls -l help.txt" and add a value higher than this.
  
  helpwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (helpwindow), "Xitip Help");
  
  gtk_container_set_border_width (GTK_CONTAINER (helpwindow), 10);

  help_scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (help_scrolled_win, 570, 400);
  help_textview = gtk_text_view_new ();
  gtk_text_view_set_editable(GTK_TEXT_VIEW (help_textview), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW (help_textview), FALSE);

  //---------Beginning of the help display unit-----------------
  /* display text: */
  strcpy(help_string,"                      Xitip Help\n                      __________\n\nXitip is a software tool to prove Information theoretic inequalities.\nThe tool will check the correctness of any valid information inequality.\nA valid information inequality is a linear inequality involving measures\nsuch as (single, conditional or joint ) entropy and mutual informations.\nOptional information theoretic constraints can be provided. Any number of\noptional constraints can be provided. The two entry\nboxes are meant to input the information inequality and the optional\ninformation constraint(s). The information inequality, which needs to be\nverified is entered into the top entry (single row entry box). The second\n(large) entry box takes the constraints. If there are multiple constraints,\nlist them on sepeare lines. Entropy is represented by 'H' and mutual \ninformation by 'I'.  \n\nInformation Inequality must be entered in a single line. The expression \nmust have a single inequality or an equality. Some examples are:\n1) I(X;Y)+3 H(A)-1.234 I(X;A|Z) >=0\n2) -H(Snow_Fall)+I(Snow_Fall;Winter|Temperature) >=0\n3) I(X;Y)=H(X)-H(X|Y)\n\nInformation measures can be scaled by real constants. However, additive\nconstants, other than 0 are not allowed. For instance, H(X)+3 >=0 is \nnot a valid information inequality, whereas \n1.245 H(X|Y)- 12.234 I(A;B|H) >=0 is a valid information inequality.\n\nThe inequality or equality can appear anywhere in the expression, but \nonly one equality or inequality allowed per expression. For example,\nthe following inequalities are one and the same. You can input \nthis expression onto this software in any of these forms.\n\n1) H(X)-H(X|Y) >=0\n2) H(X)>= H(X|Y)\n3) H(X|Y) <=0 H(X)\n\n\nInformation inequality Syntax Restrictions:\n___________________________________________\n\n1) Information inequality term can have either \"=\", \">=\" or \"<=\". \nThis means, an equal sign always must appear. Expressions with plain\n\">\" or \"<\" is not allowed.\n2) There is no restriction on the space between random variables or\nexpressions. \n\nInformation Constraints:\n________________________\n\nConstraints are input on to the second (large) text box. Any number of\nconstraints can be provided. Each new constraint must be entered on\ndifferent lines. The following types of constraints are supported.\na) Markov chains\nb) Independence of random variables\nc) Functional relationships\nd) A valid informatione expression with equality (Inequalities as \n   constraints are not supported)\n\nSyntax for Information Constraints:\n__________________________________\n\n1) A dot (\".\") refers to independence. For example A.B means the random\nvariables A and B are independent.\n2) A Forward slash (\"/\") refers to Markov chain. For instance A/B/C\nrefers to A->B->C. i.e., A, B and C forming a simple Markov chain.\n3) A colon refers to functional dependence. For example A:B means\nthat A is a deterministic function of B.\n\nAdditional help\n_______________\n\nFor additional help refer to the Xitip webpage and also the user manual.\n");
  help_buffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(GTK_TEXT_BUFFER(help_buffer),help_string,strlen(help_string));
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(help_textview),GTK_TEXT_BUFFER(help_buffer));
  gtk_container_add (GTK_CONTAINER (help_scrolled_win), help_textview);
  gtk_container_add (GTK_CONTAINER (helpwindow), help_scrolled_win);
  gtk_widget_show_all (helpwindow);
}
