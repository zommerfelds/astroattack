AstroAttack
===========

* Game design, programming and most assets by Christian Zommerfelds
* Copyright © 2014
* Feel free to send me a message to (aero_super [at] yahoo [dot] com) if you have any comments.

![](https://raw.github.com/aero-z/astroattack/gh-pages/images/screenshot.jpg)

Dependencies
------------

* OpenGL
* SDL 1.2
* SDL-mixer
* DevIL
* Box2D 2.3
* Boost
* wxWidgets (for the editor)
* Google Test (for unit tests)


How to build on a Debian based system
-------------------------------------

Install the following packages:

`sudo apt-get install autoconf g++ make libsdl1.2-dev libftgl-dev libsdl-mixer1.2-dev libdevil-dev libboost-dev libgtest-dev wx-common libwxgtk2.8-dev libbox2d-dev`

Then compile and run AstroAttack:

```
./bootstrap
./configure
make
./AstroAttack
```

AstroAttack needs Box2D 2.3. As of this writing libbox2d in Debian stable was only 2.0. If you use Debian testing or Ubuntu you don't need a custom build of Box2D.

- Install CMake: `sudo apt-get install cmake`
- Download Box2D with SVN: `svn checkout http://box2d.googlecode.com/svn/tags/v2.3.1` 
- Build the Box2D package (see Building.txt) with `cmake -DBOX2D_INSTALL=ON -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_EXAMPLES=OFF .` then `make`
- Install the Box2D library with `sudo make install`


How to build on Mac OS X with Homebrew
--------------------------------------

I was able to build AstroAttack on my Mac with the following commands:

`brew install automake wxwidgets sdl sdl_mixer ftgl box2d gcc devil`
```./configure --disable-test CPPFLAGS="`pkg-config --cflags freetype2` `sdl-config --cflags`" LDFLAGS="`sdl-config --libs`"```
`make`

This process could definitely be improved.


Troubleshooting
---------------

### Compilation error `ftheader.h not found`

If you get `/usr/include/ft2build.h:56:38: error: freetype/config/ftheader.h: No such file or directory`, it means that the FTGL library can't find the Freetype library.

Try to add ```CPPFLAGS="`pkg-config --cflags freetype2`"``` to the configure command.
(i.e.: `./configure CPPFLAGS="`pkg-config --cflags freetype2`"`)
Alternatively you can add a link to the library:

`sudo ln -s /usr/include/freetype2/freetype /usr/include/freetype`


My Cheatsheet
-------------

`../configure CXX="colorgcc -lstdc++"`
`../configure CXXFLAGS="-Wall -g" CPPFLAGS="`pkg-config --cflags freetype2`"`

