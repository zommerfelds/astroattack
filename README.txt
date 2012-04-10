                                                          
                                 ,d                              
                                 88                              
      ,adPPYYba,  ,adPPYba,  MM88MMM  8b,dPPYba,   ,adPPYba,   
      ""     `Y8  I8[    ""    88     88P'   "Y8  a8"     "8a  
      ,adPPPPP88   `"Y8ba,     88     88          8b       d8  
      88,    ,88  aa    ]8I    88,    88          "8a,   ,a8"  
      `"8bbdP"Y8  `"YbbdP"'    "Y888  88           `"YbbdP"'        
      
   ===============================================================   

                 ,d       ,d                             88         
                 88       88                             88         
   ,adPPYYba,  MM88MMM  MM88MMM  ,adPPYYba,   ,adPPYba,  88   ,d8   
   ""     `Y8    88       88     ""     `Y8  a8"     ""  88 ,a8"    
   ,adPPPPP88    88       88     ,adPPPPP88  8b          8888[      
   88,    ,88    88,      88,    88,    ,88  "8a,   ,aa  88`"Yba,   
   `"8bbdP"Y8    "Y888    "Y888  `"8bbdP"Y8   `"Ybbd8"'  88   `Y8a  
   
 ====================================================================  
           
   Game by Christian Zommerfelds
   aero_super@yahoo.com
   Copyright (c) 2012

Please send me an E-mail if you have any problems/suggestions.

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

Try to add CPPFLAGS=-I/usr/include/freetype2 to the configure command.
(i.e.: ./configure CPPFLAGS=...)
Alternatively you can add a link to the library:
sudo ln -s /usr/include/freetype2/freetype /usr/include/freetype

2) others

Send a mail to aero_super@yahoo.com. :-)


Development
-----------

1) SVN

Set the line ending style for all source files:
find . -name '*.cpp' -not -iwholename '*.svn*' -exec svn ps svn:eol-style LF {} \;
find . -name '*.h' -not -iwholename '*.svn*' -exec svn ps svn:eol-style LF {} \;


2) Useful commands for development

Building with warnings activated:
../configure CXXFLAGS="-Wall"

Building with warnings and debug symbols activated and using colorgcc for color output
../configure CXX="colorgcc -lstdc++" CXXFLAGS="-Wall -g"

