/*
 * CompShape.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompPosition.h für mehr Informationen

#include "CompShape.h"

#include "common/Foreach.h"

#include <Box2D/Box2D.h> // need this for converting to Box2D shapes
#include <boost/property_tree/ptree.hpp>
#include <boost/make_shared.hpp>

using boost::property_tree::ptree;

// eindeutige ID
const ComponentTypeId& CompShape::getTypeIdStatic()
{
    static ComponentTypeId* typeId = new ComponentTypeId("CompShape");
    return *typeId;
}

CompShape::CompShape(const ComponentIdType& id, GameEvents& gameEvents)
: Component(id, gameEvents)
{}

CompShapePolygon::CompShapePolygon(const ComponentIdType& id, GameEvents& gameEvents)
: CompShape(id, gameEvents)
{}

void CompShapePolygon::loadFromPropertyTree(const ptree& propTree)
{
    foreach(const ptree::value_type &v, propTree.get_child("polygon"))
    {
        const ptree& vertex = v.second;
        float x = vertex.get<float>("x");
        float y = vertex.get<float>("y");
        m_vertices.push_back( Vector2D(x, y) );
    }
}

void CompShapePolygon::writeToPropertyTree(ptree& propTree) const
{
    for ( size_t i = 0; i < getVertexCount(); ++i )
    {
        ptree vertexPropTree;
        vertexPropTree.add("x", m_vertices[i].x);
        vertexPropTree.add("y", m_vertices[i].y);
        propTree.add_child("polygon.vertex", vertexPropTree);
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

CompShapeCircle::CompShapeCircle(const ComponentIdType& id, GameEvents& gameEvents, const Vector2D& center, float radius) :
        CompShape(id, gameEvents),
        m_center (center),
        m_radius (radius)
{}

void CompShapeCircle::loadFromPropertyTree(const boost::property_tree::ptree& propTree)
{
    m_center.x = propTree.get("circle.x", 0.0f);
    m_center.y = propTree.get("circle.y", 0.0f);
    m_radius = propTree.get("circle.r", 1.0f);
}

void CompShapeCircle::writeToPropertyTree(ptree& propTree) const
{
    propTree.add("circle.x", m_center.x);
    propTree.add("circle.y", m_center.y);
    propTree.add("circle.r", m_radius);
}

boost::shared_ptr<b2Shape> CompShapeCircle::toB2Shape() const
{
    boost::shared_ptr<b2CircleShape> circle_shape = boost::make_shared<b2CircleShape>();
    circle_shape->m_radius = m_radius;
    circle_shape->m_p = *m_center.to_b2Vec2();
    return circle_shape;
}
