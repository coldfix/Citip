Citip
=====

Information Theoretic Inequality Prover (C++/CLI version).

This program is a fork of Xitip_ which is based on ITIP_. So far, the
differences from Xitip are:

- replace the GTK frontend by a simple CLI frontend
- ported to the free software GLPK_ library for linear programming

*Why fork?*

- I found the GUI inconvenient to work with and pointless for this
  particular application. CLI applications can be used for automated
  purposes much more easily.
- To provide a public platform for possible continued development of the
  application. I was unable to get in contact with any of the original
  authors of Xitip (emails are dead), and there is (AFAIK) no public VCS. I
  probably won't do much on this repository on my own, but **if you have
  any ideas and/or patches to contribute, don't be shy!** I probably won't
  be able to answer any questions, but asking can't hurt. In any case, just
  open an issue or send me a pull-request.

.. _Xitip: http://xitip.epfl.ch/
.. _ITIP: http://user-www.ie.cuhk.edu.hk/~ITIP/
.. _GLPK: https://www.gnu.org/software/glpk/


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
