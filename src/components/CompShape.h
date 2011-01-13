/*----------------------------------------------------------\
|                       CompShape.h                         |
|                       -----------                         |
|                   Part of AstroAttack                     |
|                  Christian Zommerfelds                    |
|                          2010                             |
\----------------------------------------------------------*/

#ifndef COMPSHAPE_H
#define COMPSHAPE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>

class Vector2D;
class TiXmlElement;
class b2Shape;

//--------------------------------------------//
//-----------   CompShape class   ------------//
//--------------------------------------------//
class CompShape : public Component
{
public:
    const CompIdType& ComponentId() const { return m_componentId; }

    static boost::shared_ptr<CompShape> LoadFromXml(const TiXmlElement& compElem);
    virtual void StoreToXml(TiXmlElement& compElem) = 0;

    virtual b2Shape* toB2Shape() = 0;

private:
	static const CompIdType m_componentId;
};
//--------------------------------------------//
//--------  End of CompShape class   ---------//
//--------------------------------------------//

//--------------------------------------------//
//-------   CompShapePolygon class   ---------//
//--------------------------------------------//
class CompShapePolygon : public CompShape
{
public:

    static boost::shared_ptr<CompShapePolygon> LoadFromXml(const TiXmlElement& polyElem);
    void StoreToXml(TiXmlElement& compElem) {};

    b2Shape* toB2Shape(); // up to the caller to delete the shape

    static const int cMaxVertices = 8;

private:
    std::vector<boost::shared_ptr<Vector2D> > m_vertices;
};
//--------------------------------------------//
//----- End of CompShapePolygon class   ------//
//--------------------------------------------//

//--------------------------------------------//
//--------   CompShapeCircle class   ---------//
//--------------------------------------------//
class CompShapeCircle : public CompShape
{
public:
	CompShapeCircle();
	CompShapeCircle(const Vector2D& center, float radius);

    static boost::shared_ptr<CompShapeCircle> LoadFromXml(const TiXmlElement& circleElem);
    void StoreToXml(TiXmlElement& compElem) {};

    b2Shape* toB2Shape();

private:
    boost::scoped_ptr<Vector2D> m_center;
    float m_radius;
};
//--------------------------------------------//
//------ End of CompShapeCircle class   ------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2010
