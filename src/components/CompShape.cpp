/*----------------------------------------------------------\
|                      CompShape.cpp                        |
|                      -------------                        |
|                   Part of AstroAttack                     |
|                  Christian Zommerfelds                    |
|                          2010                             |
\----------------------------------------------------------*/
// CompPosition.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "CompShape.h"
#include "../Vector2D.h"

#include <Box2D/Box2D.h> // need this for converting to Box2D shapes

#include <tinyxml.h>
#include <boost/make_shared.hpp>

// eindeutige ID
const CompIdType CompShape::m_componentId = "CompShape";

boost::shared_ptr<CompShape> CompShape::LoadFromXml(
		const TiXmlElement& compElem)
{
	const TiXmlElement* polygonElement = compElem.FirstChildElement("polygon");

	// Shape is a polygon
	if (polygonElement != NULL)
	{
		return CompShapePolygon::LoadFromXml(*polygonElement);
	}

	const TiXmlElement* circleElement = compElem.FirstChildElement("circle");

	// Shape is a circle
	if (circleElement != NULL)
	{
		return CompShapeCircle::LoadFromXml(*circleElement);
	}

	// TODO: error

	return boost::shared_ptr<CompShape>();
}

void CompShape::StoreToXml(TiXmlElement& compElem)
{
}

boost::shared_ptr<CompShapePolygon> CompShapePolygon::LoadFromXml(
		const TiXmlElement& compPolygon)
{
	boost::shared_ptr<CompShapePolygon> pPoly = boost::make_shared<CompShapePolygon>();

	const TiXmlElement* vertexElement = compPolygon.FirstChildElement("vertex");
	for (size_t i=0; vertexElement && i<cMaxVertices; vertexElement = vertexElement->NextSiblingElement("vertex"))
	{
		float x = 0.0f, y = 0.0f;
		vertexElement->QueryFloatAttribute("x", &x);
		vertexElement->QueryFloatAttribute("y", &y);
		pPoly->m_vertices.push_back( boost::make_shared<Vector2D>(x, y) );
	}

	/*TiXmlElement* boxElement = compPolygon->FirstChildElement( "box" );
    if ( boxElement )
    {
        float hw = 1.0f, hh = 1.0f;
        Vector2D center;
        float angle = 0.0f;
        boxElement->QueryFloatAttribute("hw", &hw);
        boxElement->QueryFloatAttribute("hh", &hh);
        boxElement->QueryFloatAttribute("x", &center.x);
        boxElement->QueryFloatAttribute("y", &center.y);
        boxElement->QueryFloatAttribute("a", &angle);
        poly_shape->SetAsBox(hw,hh,center,angle);
        vertexCount = 4;
    }*/

	// TODO: check for errors
	return pPoly;
}

// TEMP
//#include "../Entity.h"

b2Shape* CompShapePolygon::toB2Shape()
{
	b2PolygonShape* poly_shape = new b2PolygonShape;
    b2Vec2 vertices[b2_maxPolygonVertices];
    for (unsigned int i=0; i<m_vertices.size() && i<b2_maxPolygonVertices; i++)
    {
    	vertices[i].Set(m_vertices[i]->x, m_vertices[i]->y);
    }
    //std::string str = GetOwnerEntity()->GetId();
    //int s = m_vertices.size();
    poly_shape->Set(vertices, m_vertices.size());
    return poly_shape;
}

void CompShapePolygon::SetVertex(size_t i, const Vector2D& v)
{
	if (i==GetVertexCount())
	{
		m_vertices.push_back(boost::make_shared<Vector2D>(v));
	}
	else if (i<GetVertexCount())
	{
		*m_vertices[i]=v;
	}
	// out of range
}

const Vector2D* CompShapePolygon::GetVertex(size_t i) const
{
	if (i>=GetVertexCount())
		return NULL;
	return m_vertices[i].get();
}

CompShapeCircle::CompShapeCircle() :
		m_center ( new Vector2D ),
		m_radius (1.0f)
{}

CompShapeCircle::CompShapeCircle(const Vector2D& center, float radius) :
		m_center ( new Vector2D(center) ),
		m_radius (radius)
{}

boost::shared_ptr<CompShapeCircle> CompShapeCircle::LoadFromXml(
		const TiXmlElement& compCircle)
{
	Vector2D center;
	float radius = 1.0f;
	compCircle.QueryFloatAttribute("x", &center.x);
	compCircle.QueryFloatAttribute("y", &center.y);
	compCircle.QueryFloatAttribute("r", &radius);

	return boost::make_shared<CompShapeCircle>(center, radius);
}

b2Shape* CompShapeCircle::toB2Shape()
{
    b2CircleShape* circle_shape = new b2CircleShape;
    circle_shape->m_radius = m_radius;
    circle_shape->m_p = *m_center->To_b2Vec2();
    return circle_shape;
}

// Astro Attack - Christian Zommerfelds - 2010
