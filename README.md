AstroAttack
===========

![](https://raw.github.com/aero-z/astroattack/gh-pages/images/screenshot.jpg)

* Game by Christian Zommerfelds
* Contact: aero_super \<at\> yahoo \<dot\> com
* Copyright (c) 2013

Please send me an e-mail if you have any problems/suggestions.

Dependencies
------------

This is the list of what you need to build and run AstroAttack:
* 3D-accelerated OpenGL drivers installed
* C++ build tools for your platform
* The following libraries:
 - Boost (recent version)
 - SDL 1.2
 - SDL-mixer
 - OpenGL
 - DevIL
 - Box2D
 - wxWidgets (for the editor)
 - Google C++ Testing Framework (for unit tests)


How to build on Debian based system
-----------------------------------

Install the following packages:
* autoconf
* g++
* make
* libsdl1.2-dev
* libftgl-dev
* libsdl-mixer1.2-dev
* libdevil-dev
* libboost-dev
* libgtest-dev (optional, or use the --disable-test option in the ./configure step)
* wx-common (for the editor; currently no way to disable the editor)
* libwxgtk2.8-dev (same thing)

AstroAttack needs Box2D 2.1 (as of this writing libbox2d in Debian was only 2.0)
Here an example of how you could install Box2D:

- Install the packages:
* cmake (for building Box2D)
* libxi-dev (optional; for Box2D with examples)
- Download Box2D 2.1.x from the box2d.org.
- Build the Box2D package (see Building.txt) with "cmake -DBOX2D_INSTALL=ON -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_EXAMPLES=OFF .." then "make"
- Install the Box2D library (sudo make install)

Now, in the terminal, go to the directory where this file is located.
Type each of the following commands and check the output for errors.

    ./bootstrap
    ./configure
    make
    ./AstroAttack


Troubleshooting
---------------

1) ftheader.h not found

In case of

    /usr/include/ft2build.h:56:38: error: freetype/config/ftheader.h: No such file or directory
That means that the FTGL library can't find the freetype library.

Try to add `CPPFLAGS=-I/usr/include/freetype2` to the configure command.
(i.e.: `./configure CPPFLAGS=...`)
Alternatively you can add a link to the library:

    sudo ln -s /usr/include/freetype2/freetype /usr/include/freetype


Development
-----------

Building with warnings activated:

    ../configure CXXFLAGS="-Wall"

Building with warnings and debug symbols activated and using colorgcc for color output

    ../configure CXX="colorgcc -lstdc++" CXXFLAGS="-Wall -g"

