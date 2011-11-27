/*
 * Vector2D.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "Vector2D.h"

#include <Box2D/Box2D.h>
#include <boost/make_shared.hpp>

const float cPi = 3.141592653589793238462643383279502884f;

float degToRad( float angleDeg )
{
    return angleDeg/180.0f * cPi;
}

float radToDeg( float angleRad )
{
    return angleRad/cPi * 180.0f;
}

Vector2D::Vector2D ( const b2Vec2& v ) : x ( v.x ), y ( v.y ) {}

boost::shared_ptr<b2Vec2> Vector2D::to_b2Vec2() const
{
    return boost::shared_ptr<b2Vec2>(boost::make_shared<b2Vec2>(x,y));
}

// Macht den Vektor zu einem Einheitsvektor
void Vector2D::normalize()
{
    float fLength = length();

    if ( fLength == 0.0f )
        return;

    ( *this ) *= ( 1.0f / fLength );
}

// Ob rV sich Rechts von diesem Vektor befindet (=kleinster Winkel ist im Uhrzeigersinn)
bool Vector2D::isRight( const Vector2D &rV ) const
{
	if ( x * rV.y - y * rV.x < 0.0f )
		return true;
	else
		return false;
}

// Winkel herausfinden, dass der Vektor mit der X-Achse schliesst ( [1,0] hat Winkel 0, im Gegenuhrzeigersinn +, im Uhrzeigersinn - )
// Der erhaltene Winkel ist in Radian (zwischen -pi und +pi)
float Vector2D::getAngle() const
{
	if ( x == 0 && y == 0 )
		return 0;
	return atan2( y, x );
}

// Vektor um Winkel angle drehen (in Bogenmass) im Gegenuhrzeigersinn
void Vector2D::rotate ( float angle )
{
    float tx = x;
    x =  x * cos ( angle ) - y * sin ( angle );
    y = tx * sin ( angle ) + y * cos ( angle );
}

// Gibt einen Vektor zurück, der um Winkel angle (in Bogenmass) im Gegenuhrzeigersinn gedreht ist
Vector2D Vector2D::rotated( float angle ) const
{
    return Vector2D( x * cos ( angle ) - y * sin ( angle ),
                     x * sin ( angle ) + y * cos ( angle ) );
}
