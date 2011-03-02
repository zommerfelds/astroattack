/*
 * CompShape.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompPosition.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "CompShape.h"

#include <Box2D/Box2D.h> // need this for converting to Box2D shapes

#include "../contrib/pugixml/pugixml.hpp"
#include <boost/make_shared.hpp>

// eindeutige ID
const CompIdType CompShape::COMPONENT_ID = "CompShape";

boost::shared_ptr<CompShape> CompShape::loadFromXml(const pugi::xml_node& compNode)
{
    pugi::xml_node polygonElement = compNode.child("polygon");

	// Shape is a polygon
	if (polygonElement)
	{
		return CompShapePolygon::loadFromXml(polygonElement);
	}

	pugi::xml_node circleElement = compNode.child("circle");

	// Shape is a circle
	if (circleElement)
	{
		return CompShapeCircle::loadFromXml(circleElement);
	}

	// TODO: error

	return boost::shared_ptr<CompShape>();
}

boost::shared_ptr<CompShapePolygon> CompShapePolygon::loadFromXml(
		const pugi::xml_node& polyElem)
{
	boost::shared_ptr<CompShapePolygon> pPoly = boost::make_shared<CompShapePolygon>();

	pugi::xml_node vertexElement = polyElem.child("vertex");
	for (size_t i=0; vertexElement && i<cMaxVertices; vertexElement = vertexElement.next_sibling("vertex"))
	{
		float x = vertexElement.attribute("x").as_float();
        float y = vertexElement.attribute("y").as_float();
		pPoly->m_vertices.push_back( Vector2D(x, y) );
	}

	// TODO: check for errors
	return pPoly;
}

void CompShapePolygon::writeToXml(pugi::xml_node& compNode) const
{
    pugi::xml_node polyNode = compNode.append_child("polygon");

    for ( size_t i = 0; i < getVertexCount(); ++i )
    {
        pugi::xml_node vertexNode = polyNode.append_child("vertex");
        vertexNode.append_attribute("x").set_value(m_vertices[i].x);
        vertexNode.append_attribute("y").set_value(m_vertices[i].y);
    }
}

boost::shared_ptr<b2Shape> CompShapePolygon::toB2Shape() const
{
    boost::shared_ptr<b2PolygonShape> poly_shape = boost::make_shared<b2PolygonShape>();
    b2Vec2 vertices[b2_maxPolygonVertices];
    for (size_t i=0; i<m_vertices.size() && i<b2_maxPolygonVertices; i++)
    {
    	vertices[i].Set(m_vertices[i].x, m_vertices[i].y);
    }
    poly_shape->Set(vertices, m_vertices.size());
    return poly_shape;
}

void CompShapePolygon::setVertex(size_t i, const Vector2D& v)
{
	if (i==getVertexCount())
	{
		m_vertices.push_back(v);
	}
	else if (i<getVertexCount())
	{
		m_vertices[i]=v;
	}
	// out of range
}

const Vector2D* CompShapePolygon::getVertex(size_t i) const
{
	if (i>=getVertexCount())
		return NULL;
	return &m_vertices[i];
}

CompShapeCircle::CompShapeCircle() :
		m_radius (1.0f)
{}

CompShapeCircle::CompShapeCircle(const Vector2D& center, float radius) :
		m_center ( center ),
		m_radius (radius)
{}

boost::shared_ptr<CompShapeCircle> CompShapeCircle::loadFromXml(
		const pugi::xml_node& circleElem)
{
	Vector2D center;
	center.x = circleElem.attribute("x").as_float();
    center.y = circleElem.attribute("y").as_float();
	float radius = circleElem.attribute("r").as_float();
	if (radius == 0.0f)
	    radius = 1.0f;

	return boost::make_shared<CompShapeCircle>(center, radius);
}

void CompShapeCircle::writeToXml(pugi::xml_node& compNode) const
{
    pugi::xml_node circleNode = compNode.append_child("circle");
    circleNode.append_attribute("x").set_value(m_center.x);
    circleNode.append_attribute("y").set_value(m_center.y);
    circleNode.append_attribute("r").set_value(m_radius);
}

boost::shared_ptr<b2Shape> CompShapeCircle::toB2Shape() const
{
    boost::shared_ptr<b2CircleShape> circle_shape = boost::make_shared<b2CircleShape>();
    circle_shape->m_radius = m_radius;
    circle_shape->m_p = *m_center.to_b2Vec2();
    return circle_shape;
}
