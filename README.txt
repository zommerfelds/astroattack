*** Astro Attack ***
--------------------

Game by Christian Zommerfelds
czommerfelds@yahoo.com
Copyright (c) 2010

Please send me an E-mail if you have any problems/suggestions.


How to build on GNU/Linux
-------------------------

Before building AstroAttack, you will need to install the following packages:

autoconf
g++
libsdl1.2-dev
libftgl-dev
libsdl-mixer1.2-dev
libdevil-dev
libboost1.40-dev
cmake (for Box2D)

Download Box2D 2.1.x.
Build the Box2D package (see Building.txt) with "cmake -DBOX2D_INSTALL=ON -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_EXAMPLES=OFF .."
Install the Box2D library (sudo make install)

Then, you may have to type the following line in a terminal because the FTGL library can't find the freetype library.
sudo ln -s /usr/include/freetype2/freetype/ /usr/include/freetype

Now, in the terminal, go to the directory where this file is located. Type:

./configure

Then (if everything went well), type:

make

To play the game:

./src/AstroAttack


Useful commands for developpment
--------------------------------

Building with warnings activated:
./configure CXXFLAGS="-Wall"

Building with warnings activated and using colorgcc for color output
./configure CXX="colorgcc -lstdc++" CXXFLAGS="-Wall"

