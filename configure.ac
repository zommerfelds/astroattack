AC_INIT([AstroAttack], [0.1.0])
AM_INIT_AUTOMAKE([foreign no-define subdir-objects])

AC_PROG_CXX
AC_LANG(C++)

# enable compilation of unit tests (needs google test library)
enabletest=true
AC_ARG_ENABLE( [test],
               [  --disable-test    Compile the tests (Requires Google C++ Testing Framework)],
               [case "${enableval}" in
               yes) enabletest=true ;;
               no)  enabletest=false ;;
               *)   AC_MSG_ERROR([bad value ${enableval} for --enable-test]) ;;
               esac],
               [test=false]
             )

AS_VAR_IF(enabletest, true,
[AC_CHECK_HEADER([gtest/gtest.h],[],[AC_MSG_ERROR([You need the Google C++ Testing Framework (libgtest). Or use --disable-test to disable unit tests.])])],
[])
AM_CONDITIONAL([ENABLETEST], [test x$enabletest = xtrue])

AC_SEARCH_LIBS(tan, m, [], [AC_MSG_ERROR("libm required!")])
AC_SEARCH_LIBS(SDL_Init, SDL, [], [AC_MSG_ERROR("SDL required!")])
AC_SEARCH_LIBS(ftglGetFontError, ftgl, [], [AC_MSG_ERROR("FTGL required!")])
AC_SEARCH_LIBS(b2_version, Box2D, [], [AC_MSG_ERROR("Box2D required!")]) # Box2D vs box2d?
AC_SEARCH_LIBS(Mix_OpenAudio, SDL_mixer, [], [AC_MSG_ERROR("SDL_mixer required!")])
AC_SEARCH_LIBS(ilInit, IL, [], [AC_MSG_ERROR("DevIL (libIL) required!")])
AC_SEARCH_LIBS(iluInit, ILU, [], [AC_MSG_ERROR("DevIL (libILU) required!")])
AC_CHECK_HEADER(boost/shared_ptr.hpp, [], [AC_MSG_ERROR("boost/shared_ptr.hpp required!")] )
AC_CHECK_HEADER(boost/make_shared.hpp, [], [AC_MSG_ERROR("boost/make_shared.hpp required!")] )
AC_CHECK_HEADER(boost/scoped_ptr.hpp, [], [AC_MSG_ERROR("boost/scoped_ptr.hpp required!")] )
AC_CHECK_HEADER(boost/foreach.hpp, [], [AC_MSG_ERROR("boost/foreach.hpp required!")] )
AC_CHECK_HEADER(boost/bind.hpp, [], [AC_MSG_ERROR("boost/bind.hpp required!")] )

# Check for OpenGL (Original or Mesa) (or osX-framework)
AC_CHECK_HEADERS(GL/gl.h GL/glu.h,,
         [AC_CHECK_HEADERS(OpenGL/gl.h OpenGL/glu.h,,
           [echo "OpenGL is mandatory";exit 1])]
)

if test `uname -s` != Darwin;
then
AC_CHECK_LIB(GL, glInitNames,,
     AC_CHECK_LIB(MesaGL,glInitNames,,
            echo "OpenGL is mandatory";exit 1)
)

AC_CHECK_LIB(GLU, gluLookAt, ,
     AC_CHECK_LIB(MesaGLU,gluLookAt,,
     echo "incomplete OpenGL (no GLU)";exit 1)
)
fi

AM_OPTIONS_WXCONFIG
reqwx=2.4.0
AM_PATH_WXCONFIG($reqwx, wxWin=1)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.
 
		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi

AC_SUBST(WX_GL_LIBS, `wx-config --gl-libs`)

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"

AC_CONFIG_FILES(Makefile)
AC_OUTPUT

AS_VAR_IF(enabletest, true,
[AC_MSG_NOTICE(* Tests are enabled (use --disable-test to disable))],
[AC_MSG_NOTICE(* Tests are disabled)])

AC_MSG_NOTICE(* You can now run make.)
