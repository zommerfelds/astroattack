                                                          
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
   czommerfelds@yahoo.com
   Copyright (c) 2011

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
box2d

Possibly
libxi-dev (for Box2D with examples)

Download Box2D 2.1.x.
Build the Box2D package (see Building.txt) with "cmake -DBOX2D_INSTALL=ON -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_EXAMPLES=OFF .."
Install the Box2D library (sudo make install)

Now, in the terminal, go to the directory where this file is located. Type:

./configure

Then (if everything went well), type:

make

When:
/usr/include/ft2build.h:56: fatal error: freetype/config/ftheader.h: No such file or directory
Do:
sudo ln -s /usr/include/freetype2/freetype /usr/include/freetype
(FTGL library can't find the freetype library)

To play the game:

./src/AstroAttack


SVN
---

Set the line ending style for all source files:
find . -name '*.cpp' -not -iwholename '*.svn*' -exec svn ps svn:eol-style LF {} \;
find . -name '*.h' -not -iwholename '*.svn*' -exec svn ps svn:eol-style LF {} \;


Useful commands for development
--------------------------------

Building with warnings activated:
../configure CXXFLAGS="-Wall"

Building with warnings and debug symbols activated and using colorgcc for color output
../configure CXX="colorgcc -lstdc++" CXXFLAGS="-Wall -g"

