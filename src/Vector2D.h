/*----------------------------------------------------------\
|                        Vector.h                           |
|                        --------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// 2D Vektor

#ifndef VECTOR2D_H
#define VECTOR2D_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <cmath>
#include <boost/shared_ptr.hpp>

extern const float cPi;

float degToRad( float angleDeg );
float radToDeg( float angleRad );

struct b2Vec2;

/*
    Diese Klasse speichert Daten von einem zweidimensinaler Vektor.
	Einige Operationen für Vektoren sind auch implementiert.
*/
class Vector2D
{
public:
    // Default constructor, vector will be [0,0]
    Vector2D() : x ( 0.0f ), y ( 0.0f ) {}
    // Constructor with x and y components
    Vector2D( float x_comp, float y_comp ) : x ( x_comp ), y ( y_comp ) {}
    // Copy constructor
    Vector2D( const Vector2D& v ) : x ( v.x ), y ( v.y ) {}
    // Construct from a Box2D vector
    Vector2D( const b2Vec2& v );
    // Destructor
    ~Vector2D() {}

    // The vector coordinates
    float x, y;

    // Convert to a Box2D vector
    boost::shared_ptr<b2Vec2> To_b2Vec2();

    // Set new values for x and y
    inline void Set( float new_x, float new_y )
    {
        x = new_x;
        y = new_y;
    }

    // Set new x/y values from polar coordinates
    inline void SetPolar( float lenght, float angle )
    {
        x = lenght * cos(angle);
        y = lenght * sin(angle);
    }

	// Skalarmultiplikation
    inline Vector2D operator * ( const float  s ) const
    {
        return Vector2D ( x*s, y*s );
    }

	// "Skalardivision"
    inline Vector2D operator / ( const float  s )			const
    {
        return Vector2D ( x/s, y/s );
    }

	// Vektoraddition
    inline Vector2D operator + ( const Vector2D &rV )	const
    {
        return Vector2D ( x+rV.x, y+rV.y );
    }

	// Vektorsubtraktion
    inline Vector2D operator - ( const Vector2D &rV )	const
    {
        return Vector2D ( x-rV.x, y-rV.y );
    }

	// Negativer Vektor
    inline Vector2D operator - ()	const
    {
        return Vector2D ( -x, -y );
    }

	// ==
    inline bool operator == ( const Vector2D &rV )	const
    {
        return ( x==rV.x && y==rV.y );
    }

	// !=
    inline bool operator != ( const Vector2D &rV )	const
    {
        return ( x!=rV.x || y!=rV.y );
    }

	// durch einen Skalar dividieren
    inline Vector2D &operator /= ( const float scalar )
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

	// mit einem Skalar multiplizieren
    inline Vector2D &operator *= ( const float scalar )
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

	// um rOther vergrössern
    inline Vector2D &operator += ( const Vector2D &rOther )
    {
        x += rOther.x;
        y += rOther.y;
        return *this;
    }

	// um rOther verkleinern
    inline Vector2D &operator -= ( const Vector2D &rOther )
    {
        x -= rOther.x;
        y -= rOther.y;
        return *this;
    }

	// Vektor festlegen
    inline Vector2D &operator = ( const Vector2D &rV )
    {
        x = rV.x;
        y = rV.y;
        return *this;
    }

	// Skalarprodukt
    inline float operator * ( const Vector2D &rV )	const
    {
        return ( x*rV.x ) + ( y*rV.y );    // dot product
    }

	// Betrag
    inline float Length() const
    {
        return sqrt ( x*x + y*y );
    }

	// Betrag im Quadrat (is schneller als Lenght())
    inline float LengthSquared() const
    {
        return x*x + y*y ;
    }

    // Skaliert den Vektor auf Länge 1
    void Normalise();

	// Gibt zurück ob rV sich Rechts von diesem Vektor befindet (=kleinster Winkel ist im Uhrzeigersinn von rV aus)
	bool IsRight( const Vector2D &rV ) const;

	// Winkel herausfinden, dass der Vektor mit der X-Achse schliesst ( [1,0] hat Winkel 0, im Gegenuhrzeigersinn +, im Uhrzeigersinn - )
	// Der erhaltene Winkel ist in Radian (zwischen -pi und +pi)
	float GetAngle() const;

    // Vektor um Winkel angle (in Bogenmass) drehen im Gegenuhrzeigersinn
    void Rotate( float angle );

    // Gibt einen Vektor zurück, der um Winkel angle (in Bogenmass) im Gegenuhrzeigersinn gedreht ist
    Vector2D Rotated( float angle ) const;

    // Gibt einen Vektor zurück von dem Punkt mit diesem Positionsvektor (this) bis zu dem Punkt mit Positionsvektor V
    inline Vector2D Distance( const Vector2D &V ) const
    {
        return ( V - ( *this ) );
    }

    // Gibt die Richtung des Vektors als Einheitsvektor zurück
    inline Vector2D GetUnitVector() const
    {
        float fLength = Length();

        if ( fLength == 0.0f )
            return Vector2D(0.0f,0.0f);

        return ( *this ) * ( 1.0f / fLength );
    }
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
