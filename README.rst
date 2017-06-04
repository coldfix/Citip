Citip
=====

Information Theoretic Inequality Prover (C++/CLI version).

This program is derived from Xitip_ which is based on ITIP_. Xitip was made
by *Rethnakaran Pulikkoonattu*, *Etienne Perron* and *Suhas Diggavi*. ITIP
was created by *Raymond W. Yeung* and *Ying-On Yan*.

.. _Xitip: http://xitip.epfl.ch/
.. _ITIP: http://user-www.ie.cuhk.edu.hk/~ITIP/


Why another fork?
-----------------

Originally, I just wanted to replace the GTK frontend by a more convenient
CLI interface, because I found the GUI annoying, inconvenient to work with
and pointless for this particular application. Thinking further, CLI based
applications are also better for automatization and require fewer run-time
dependencies.

-rant

Another reason was to provide a public platform for possible continued
development of the application. I was unable to get in contact with any of
the original authors of Xitip (emails are dead), and there is (AFAIK) no
public VCS. **If you have any ideas and/or patches to contribute, don't be
shy!**, just open an issue or send me a pull-request.

I didn't plan to do much apart from that. By now all the source files have
been completely rewritten. The main differences to Xitip are:

- replace the GTK frontend by a simple CLI frontend
- ported to the free software GLPK_ library for linear programming
- extend the accepted grammar in a few places
- more maintainable code base
- compilation requires a C++11 compliant compiler and recent versions of
  flex and bison

For a more detailed list of changes see CHANGES.rst_ and ultimately the
commit history.

.. _GLPK: https://www.gnu.org/software/glpk/
.. _CHANGES.rst: https://github.com/coldfix/Citip/blob/master/CHANGES.rst


Build
-----

First, you need to obtain a version of GLPK_ suited for your platform. It
is highly likely that you can grab GLPK from your distribution's official
repositories. Otherwise, see their website for instructions.

In the best case scenario, Citip can now be built by simply running the
``make`` command. Everything put together, your build process could look as
follows:

.. code-block:: bash

    sudo apt-get install libglpk-dev
    git clone https://github.com/coldfix/Citip.git
    cd Citip
    make

If this fails, try building with cmake as follows:

.. code-block::

    make clean
    mkdir build
    cd build
    cmake ..
    make

Please report if the compilation or flex/bison source generation fails. If
the problem is only with flex/bison, I can provide generated sources.

Usage
-----

The command line interface is pretty basic at the moment. The inequality to
be proven and constraints can be passed either as command line arguments or
(if no command line arguments are provided or the last one is -) via STDIN.

The first command line argument (or line from STDIN) is taken to be the
inequality to be proven. All the others are constraints. Example:

.. code-block:: bash

    $ ./Citip 'I(X;Y|Z) <= I(X;Y)'

    The information expression is either:
        1. FALSE, or
        2. a non-Shannon type inequality

    (exit code = 1)


    $ ./Citip 'I(X;Y|Z) <= I(X;Y)' 'H(Z) = 0'

    The information expression is TRUE.

    (exit code = 0)


    $ ./Citip 'I(X;;Y|Z) <= I(X;Y)'

    ERROR: syntax error, unexpected ';', expecting NAME
    in row 0 col 5:

        I(X;;Y|Z) <= I(X;Y)
            ^^

    (exit code = 2)

Note that the location indicators for syntax errors are only approximate.

The program exit code can be used to determine the outcome. The meaning of
exit codes is as follows::

    0 - Inequality is TRUE
    1 - Truth can not be decided by Citip
    2 - Error


License
-------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
