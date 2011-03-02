/*
 * CompShape.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */


#ifndef COMPSHAPE_H
#define COMPSHAPE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"

#include <boost/shared_ptr.hpp>
#include <vector>

#include "../Vector2D.h"
namespace pugi { class xml_node; }
class b2Shape;

//--------------------------------------------//
//-----------   CompShape class   ------------//
//--------------------------------------------//
class CompShape : public Component
{
public:
	virtual ~CompShape() {}

    const CompIdType& getComponentId() const { return COMPONENT_ID; }

    static boost::shared_ptr<CompShape> loadFromXml(const pugi::xml_node& compElem);

    virtual boost::shared_ptr<b2Shape> toB2Shape() const = 0;

    enum Type { Polygon, Circle };
    virtual Type getType() const = 0;

	static const CompIdType COMPONENT_ID;
};

//--------------------------------------------//
//-------   CompShapePolygon class   ---------//
//--------------------------------------------//
class CompShapePolygon : public CompShape
{
public:
    static boost::shared_ptr<CompShapePolygon> loadFromXml(const pugi::xml_node& polyElem);
    void writeToXml(pugi::xml_node& compElem) const;

    boost::shared_ptr<b2Shape> toB2Shape() const; // up to the caller to delete the shape

    void setVertex(size_t i, const Vector2D& v);

    const Vector2D* getVertex(size_t i) const;
    size_t getVertexCount() const { return m_vertices.size(); }

    Type getType() const { return Polygon; }

    static const unsigned int cMaxVertices = 8;

private:
    std::vector<Vector2D> m_vertices;
};

//--------------------------------------------//
//--------   CompShapeCircle class   ---------//
//--------------------------------------------//
class CompShapeCircle : public CompShape
{
public:
	CompShapeCircle();
	CompShapeCircle(const Vector2D& center, float radius);

    static boost::shared_ptr<CompShapeCircle> loadFromXml(const pugi::xml_node& circleElem);
    void writeToXml(pugi::xml_node& compElem) const;

    boost::shared_ptr<b2Shape> toB2Shape() const;

    float getRadius() const { return m_radius; }
    const Vector2D& getCenter() const { return m_center; }

    Type getType() const { return Circle; }

private:
    Vector2D m_center;
    float m_radius;
};

#endif
