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
	~CompShape() {}

    const CompIdType& ComponentId() const { return COMPONENT_ID; }

    static boost::shared_ptr<CompShape> LoadFromXml(const TiXmlElement& compElem);
    virtual void StoreToXml(TiXmlElement& compElem) = 0;

    virtual boost::shared_ptr<b2Shape> toB2Shape() = 0;

    enum Type { Polygon, Circle };
    virtual Type GetType() const = 0;

	static const CompIdType COMPONENT_ID;
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

    boost::shared_ptr<b2Shape> toB2Shape(); // up to the caller to delete the shape

    void SetVertex(size_t i, const Vector2D& v);

    const Vector2D* GetVertex(size_t i) const;
    size_t GetVertexCount() const { return m_vertices.size(); }

    Type GetType() const { return Polygon; }

    static const unsigned int cMaxVertices = 8;

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

    boost::shared_ptr<b2Shape> toB2Shape();

    float GetRadius() const { return m_radius; }
    const Vector2D& GetCenter() const { return *m_center; }

    Type GetType() const { return Circle; }

private:
    boost::scoped_ptr<Vector2D> m_center;
    float m_radius;
};
//--------------------------------------------//
//------ End of CompShapeCircle class   ------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2010
