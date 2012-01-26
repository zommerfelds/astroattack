/*
 * CompShape.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */


#ifndef COMPSHAPE_H
#define COMPSHAPE_H

#include "common/Component.h"
#include "common/Vector2D.h"

#include <vector>
#include <boost/shared_ptr.hpp>

class b2Shape;

//--------------------------------------------//
//-----------   CompShape class   ------------//
//--------------------------------------------//
class CompShape : public Component
{
public:
    CompShape(const ComponentIdType& id, GameEvents& gameEvents);
    virtual ~CompShape() {}

    const ComponentTypeId& getTypeId() const { return getTypeIdStatic(); }

    virtual boost::shared_ptr<b2Shape> toB2Shape() const = 0;

    enum Type { Polygon, Circle };
    virtual Type getType() const = 0;

    static const ComponentTypeId& getTypeIdStatic();
};

//--------------------------------------------//
//-------   CompShapePolygon class   ---------//
//--------------------------------------------//
class CompShapePolygon : public CompShape
{
public:
    CompShapePolygon(const ComponentIdType& id, GameEvents& gameEvents);
    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;

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
    CompShapeCircle(const ComponentIdType& id, GameEvents& gameEvents, const Vector2D& center = Vector2D(), float radius = 1.0f);

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;

    boost::shared_ptr<b2Shape> toB2Shape() const;

    float getRadius() const { return m_radius; }
    const Vector2D& getCenter() const { return m_center; }

    Type getType() const { return Circle; }

private:
    Vector2D m_center;
    float m_radius;
};

#endif
