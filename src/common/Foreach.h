/*
 * Foreach.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#ifndef FOREACH_H
#define FOREACH_H

#include <boost/foreach.hpp>
// NOTE: the '(x,y)' shouln't be here, but with boost 1.50 it didn't work without
// see http://www.boost.org/doc/libs/1_50_0/doc/html/foreach.html#foreach.introduction.making__literal_boost_foreach__literal__prettier
// and https://svn.boost.org/trac/boost/ticket/6131
#define foreach(x,y) BOOST_FOREACH(x,y)

#endif /* FOREACH_H */
