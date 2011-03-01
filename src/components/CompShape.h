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

    const CompIdType& ComponentId() const { return COMPONENT_ID; }

    static boost::shared_ptr<CompShape> LoadFromXml(const pugi::xml_node& compElem);

    virtual boost::shared_ptr<b2Shape> toB2Shape() const = 0;

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
    static boost::shared_ptr<CompShapePolygon> LoadFromXml(const pugi::xml_node& polyElem);
    void WriteToXml(pugi::xml_node& compElem) const;

    boost::shared_ptr<b2Shape> toB2Shape() const; // up to the caller to delete the shape

    void SetVertex(size_t i, const Vector2D& v);

    const Vector2D* GetVertex(size_t i) const;
    size_t GetVertexCount() const { return m_vertices.size(); }

    Type GetType() const { return Polygon; }

    static const unsigned int cMaxVertices = 8;

private:
    std::vector<Vector2D> m_vertices;
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

    static boost::shared_ptr<CompShapeCircle> LoadFromXml(const pugi::xml_node& circleElem);
    void WriteToXml(pugi::xml_node& compElem) const;

    boost::shared_ptr<b2Shape> toB2Shape() const;

    float GetRadius() const { return m_radius; }
    const Vector2D& GetCenter() const { return m_center; }

    Type GetType() const { return Circle; }

private:
    Vector2D m_center;
    float m_radius;
};
//--------------------------------------------//
//------ End of CompShapeCircle class   ------//
//--------------------------------------------//

#endif
