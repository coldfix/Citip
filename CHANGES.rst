CHANGELOG
=========

2.0.0
~~~~~

Date: 2015-05-09

- Rewrite the parser engine. A few comments:

  The new code base uses C++11 to do more work in much fewer lines of code.
  The number of lines of code has decreased from about 1600 to 900 while at
  the same time increasing the amount of guiding comments.

  Previously, the parser code was essentially a hard-to-grap state machine
  (which b.t.w. could also have been implemented using only flex). This has
  been remedied. The code complexity has decreased by a lot and the process
  is severed into distinct parsing phases. This makes the code a lot easier
  to understand and modify.

  Downside: the code base is now fully dependent on C++11 which requires a
  recent compiler.

- Remove some arbitrary restrictions in the language:

  - You can now use the shorthand notation 'A / B,C / D' to define markov
    chain with a link being a list of variables. The same limitation was
    overcome for the function-of syntax 'A:B' and the mutual independence
    statement 'A.B.C'.

  - You can now use inequalities as constraints

  - You can now use information expressions with arbitrary constants such
    as 'H(X) >= 1'. This is probably not very useful, but then again, there
    was no real reason not to allow it (or I didn't think of it, yet).

- Don't not create temporary files anymore. Flex supports parsing in-memory
  data which is a clear winner compared to needless filesystem access.

- use the lexer to split the input stream into tokens.

  Previously, the lexer was only used to do some obscur preprocessing the
  purpose of which is still completely unclear to me. In fact, I find it
  likely that this separate usage of the lexer could lead to inconsistent
  behaviour in some cases.

- Variable names cannot contain white spaces any longer. This notation is
  now treated as a syntax error.

  In fact, in the face of the grammar definition, I believe this was an
  unintended side effect the hand-written yylex function just stripping
  away all whitespace. For example, 'H(X Y)' would be equivalent to 'H(XY)'
  but not to 'H(X,Y)', while their was a grammar rule trying to achieve the
  inverse.

- Always perform stack unwinding in the main function

- Slightly improve behaviour in the face of some errors

- I am currently missing (at least?) one optimization that was present in
  Xitip: collapsing sets of variables that only appear together.


1.1.0
~~~~~

Date: 2015-04-18

- Port to GLPK
- Fix behaviour when checking for basic information measure (the
  mis-feature was inherited from Xitip)
- obsessive style fixes
- Reenable 'A:B' syntax (was broken due to my ignorance)


1.0.0 (now Citip)
~~~~~~~~~~~~~~~~~

Date: 2015-04-01

- Replace the GTK frontend by a simple CLI.
- Thus eliminate build time and run time dependencies on GTK, etc.
- Start a proper README


Xitip (unspecified version)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Date: 2008-07-03

This is the version of Xitip as could be downloaded in a .tar.gz archive
from their website in early 2015.

Unfortunately, I couldn't get into contact with any of the Xitip project
authors (3 dead emails!), so I don't have access to their version
history nor development branches or version number.

The release date guess (03 July 2008) is based on the most recent mtime
among the extracted source files.

I excluded the binary and temporary files and the .svn metadata folder
contained in the upstream tarball for obvious reasons.

Furthermore, I excluded the qsopt files. These should be downloaded by
the user separately (which is on 64bit distributions necessary anyway).
Though, the main reason not to include these files are the license issues.
