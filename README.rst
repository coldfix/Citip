Citip
=====

Information Theoretic Inequality Prover (C++/CLI version).

This program is a fork of Xitip_ which is based on ITIP_ and depends on
QSopt_.

The only difference from Xitip (so far) is, that the GTK frontend was
replaced by a simple CLI frontend. *Why?* I found the GUI inconvenient to
work with and pointless for this particular application. CLI applications
can be used for automated purposes much more easily.

.. _Xitip: http://xitip.epfl.ch/
.. _ITIP: http://user-www.ie.cuhk.edu.hk/~ITIP/
.. _QSopt: http://www.math.uwaterloo.ca/~bico/qsopt/


Build
-----

First, you need to obtain a version of QSopt_ suited for your platform. Copy
the ``qsopt.h`` as well as the ``qsopt.a`` files into the Citip folder. Note
that QSopt is not open source but can be used at no cost for research and
educational purposes.

In the best case scenario, Citip can now be built by simply running the
``make`` command. Everything put together, your build process could look as
follows:

.. code-block:: bash

    git clone https://github.com/coldfix/Citip.git
    cd Citip

    # QSopt for 64bit linux:
    wget http://www.math.uwaterloo.ca/~bico//qsopt/beta/codes/linux64/qsopt.a
    wget http://www.math.uwaterloo.ca/~bico//qsopt/beta/codes/linux64/qsopt.h

    # build
    make


Usage
-----

The command line interface is pretty basic at the moment. The inequality to
be proven and constraints can be passed either as command line arguments or
(if no command line arguments are provided or the last one is -) via STDIN.

The first command line argument (or line from STDIN) is taken to be the
inequality to be proven. All the others are constraints. Example:

.. code-block:: bash

    $ ./Citip 'I(X;Y|Z) <= I(X;Y)'

    The information expression (without any further constraint) is Not solvable by Xitip: This implies either of the following situations
     1.	 The inequality is FALSE, or
     2.	 This expression is a non-Shannon type inequality which is true.
     	 Currently Xitip is equipped enough to verify only the Shannon type inequalities

    (exit code = 1)

    $ ./Citip 'I(X;Y|Z) <= I(X;Y)' 'H(Z) = 0'

    The information expression (with the given constraints) is TRUE.

    (exit code = 0)

    $ ./Citip 'I(X;;Y|Z) <= I(X;Y)'

    Syntax ERROR: Re-enter the information expression
    	"I(X;;Y|Z)<=I(X;Y)"

    (exit code = 2)

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
