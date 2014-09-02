AstroAttack
===========

AstroAttack is a space themed platform and puzzle game in 2D. The player comes from a small planet that was attacked by an evil character and the player's mission is to explore the universe to recover all the stolen resources and captured inhabitants. The game features moveable objects, different forms of gravitation, free-form shapes and interesting graphics. The game is in a very early development stage.

* Game design, programming and most assets by Christian Zommerfelds
* Copyright © 2014
* If you have any comments, feel free to send me a message with the address `aero_super [at] yahoo [dot] com` or use the GitHub issue tracker.

![Screenshot 1](https://raw.github.com/aero-z/astroattack/gh-pages/images/screenshot1.png)
![Screenshot 2](https://raw.github.com/aero-z/astroattack/gh-pages/images/screenshot2.png)

Dependencies
------------

To play the game, you will have to install the dependencies manually and build from source. **Please send any feedback if you experience problems compiling and running AstroAttack.** I'll be glad to help and improve the process. This is the list of requirements:

* OpenGL (should be alread installed)
* SDL 1.2
* SDL-mixer
* DevIL
* Box2D 2.3
* Boost
* wxWidgets (for the editor)
* Google Test (optional, for unit tests, use `--disable-test` in the `./configure` command to disable)


How to build on a Debian based system
-------------------------------------

Install the requirements using the following command:

```
sudo apt-get install autoconf g++ make libsdl1.2-dev libftgl-dev libsdl-mixer1.2-dev libdevil-dev libboost-dev libgtest-dev wx-common libwxgtk2.8-dev libbox2d-dev
```

Then compile and run AstroAttack:

```
./bootstrap
./configure
make
./AstroAttack
```

AstroAttack needs Box2D 2.3. As of this writing libbox2d in Debian stable was only 2.0. If you use Debian testing or Ubuntu you can skip this part.

* Install CMake: `sudo apt-get install cmake`
* Download Box2D with SVN: `svn checkout http://box2d.googlecode.com/svn/tags/v2.3.1` 
* Build the Box2D package (see Building.txt) with `cmake -DBOX2D_INSTALL=ON -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_EXAMPLES=OFF .` then `make`
* Install the Box2D library with `sudo make install`


How to build on Mac OS X with Homebrew
--------------------------------------

I was able to build AstroAttack on a Mac with the following commands:

```
brew install automake wxwidgets sdl sdl_mixer ftgl box2d gcc devil
./bootstrap
./configure --disable-test CPPFLAGS="`pkg-config --cflags freetype2` `sdl-config --cflags`" LDFLAGS="`sdl-config --libs`"
make
./AstroAttack
```

Any suggestions for improvement are welcome.


How to build on Windows with Visual Studio
------------------------------------------

Open the Visual Studio solution inside the directory `vs2010`. After adding all the library dependencies to Visual Studio, you should be able to build and run normally.


Troubleshooting
---------------

#### 1. Compilation error `ftheader.h not found`

If you get the message `/usr/include/ft2build.h:56:38: error: freetype/config/ftheader.h: No such file or directory`, it means that the FTGL library can't find the Freetype library.

Try to add ```CPPFLAGS="`pkg-config --cflags freetype2`"``` to the configure command.
(i.e.: ```./configure CPPFLAGS="`pkg-config --cflags freetype2`"```)
Alternatively you can add a link to the library:

`sudo ln -s /usr/include/freetype2/freetype /usr/include/freetype`


My Cheatsheet
-------------

```
../configure CXX="colorgcc -lstdc++"
../configure CXXFLAGS="-Wall -g" CPPFLAGS="`pkg-config --cflags freetype2`"
```

