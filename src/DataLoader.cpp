/*
 * DataLoader.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include <sstream>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>

#include "DataLoader.h"
#include "World.h"
#include "Logger.h"
#include "Configuration.h"
#include "GameApp.h"
#include "Renderer.h"
#include "Physics.h"
#include "Font.h"
#include "Texture.h"

#include "components/CompGravField.h"
#include "components/CompVisualTexture.h"
#include "components/CompPhysics.h"
#include "components/CompPosition.h"
#include "components/CompPlayerController.h"
#include "components/CompVisualAnimation.h"
#include "components/CompTrigger.h"
#include "components/CompTrigger_Effects.h"
#include "components/CompTrigger_Conditions.h"
#include "components/CompVisualMessage.h"
#include "components/CompShape.h"

#include "states/SlideShowState.h"

#include "contrib/pugixml/pugixml.hpp"
#include "contrib/pugixml/foreach.hpp"

// TODO: a lot of work to do here (error checking etc..)

using boost::property_tree::ptree;

namespace
{

void xmlNodeToPropertyTree(const pugi::xml_node& xmlNode, ptree& propertyTree)
{
    for(pugi::xml_attribute_iterator ait = xmlNode.attributes_begin(); ait != xmlNode.attributes_end(); ait++) {
        propertyTree.put(ait->name(), ait->value());
    }
    BOOST_FOREACH(const pugi::xml_node& node, xmlNode )
    {
        if (!std::string(node.name()).empty())
            xmlNodeToPropertyTree(node, propertyTree.add_child(node.name(), ptree()));
    }
}

} // namespace

DataLoadException::DataLoadException(const std::string& msg)
 : Exception (msg)
{
}

// Load Level from XML
void DataLoader::loadWorld(const std::string& fileName, World& gameWorld, SubSystems& subSystems)
{
    try
    {
        using boost::shared_ptr;
        using boost::make_shared;

        gAaLog.write ( "Loading world file \"%s\"...\n", fileName.c_str() );
        gAaLog.increaseIndentationLevel();

        ptree levelPropTree;
        read_info(fileName, levelPropTree);

        BOOST_FOREACH(const ptree::value_type &value1, levelPropTree)
        {
            if (value1.first != "entity")
                throw DataLoadException(fileName + " - At top level: parse error, expected 'entity', got '" + value1.first + "'"); // TODO: add node path
            const ptree& entityPropTree = value1.second;

            std::string entityId = entityPropTree.get<std::string>("id");
            ComponentList entity;
            gAaLog.write ( "Creating entity \"%s\"\n", entityId.c_str() );
            gAaLog.increaseIndentationLevel();

            BOOST_FOREACH(const ptree::value_type &value2, entityPropTree)
            {
                if (value2.first == "id")
                    continue;
                if (value2.first != "component")
                    throw DataLoadException(fileName + " - In entity '" + entityId + "': parse error, expected 'component', got '" + value2.first + "'"); // TODO: add node path
                const ptree& compPropTree = value2.second;

                std::string compType = compPropTree.get<std::string>("type");
                std::string compId = compPropTree.get("id", "");
                gAaLog.write ( "Creating component \"%s\"... ", compType.c_str() );

                shared_ptr<Component> component;

                if ( compType == "CompShape" )
                {
                    if (compPropTree.count("polygon"))
                        component = boost::shared_ptr<CompShapePolygon>(new CompShapePolygon(compId, subSystems.events));
                    else
                        component = boost::shared_ptr<CompShapeCircle>(new CompShapeCircle(compId, subSystems.events));
                }
                else if ( compType == "CompPhysics" )
                    component = boost::shared_ptr<CompPhysics>(new CompPhysics(compId, subSystems.events));
                else if ( compType == "CompPlayerController" )
                    component = boost::shared_ptr<CompPlayerController>(new CompPlayerController(compId, subSystems.events, subSystems.input, gameWorld.getItToVariable( "JetpackEnergy" )));
                else if ( compType == "CompPosition" )
                    component = boost::shared_ptr<CompPosition>(new CompPosition(compId, subSystems.events));
                else if ( compType == "CompVisualAnimation" )
                    component = boost::shared_ptr<CompVisualAnimation>(new CompVisualAnimation(compId, subSystems.events, subSystems.renderer.getAnimationManager()));
                else if ( compType == "CompVisualTexture" )
                    component = boost::shared_ptr<CompVisualTexture>(new CompVisualTexture(compId, subSystems.events));
                else if ( compType == "CompVisualMessage" )
                    component = boost::shared_ptr<CompVisualMessage>(new CompVisualMessage(compId, subSystems.events));
                else if ( compType == "CompGravField" )
                    component = boost::shared_ptr<CompGravField>(new CompGravField(compId, subSystems.events));
                else if ( compType == "CompTrigger" )
                    component = boost::shared_ptr<CompTrigger>(new CompTrigger(compId, gameWorld, subSystems.events));
                else
                    throw DataLoadException(fileName + " - In entity '" + entityId + "' component '" + compId + "': invalid component type '" + compType + "'");

                assert(component);

                component->loadFromPropertyTree(compPropTree);

                entity.push_back(component);

                gAaLog.write ( "[ Done ]\n" );
            }

            gameWorld.getCompManager().addEntity(entityId, entity);
            gAaLog.decreaseIndentationLevel();
        }

        gAaLog.decreaseIndentationLevel();
        gAaLog.write ( "[ Done ]\n\n", fileName.c_str() );
    }
    catch (boost::property_tree::ptree_error& e)
    {
        throw DataLoadException(std::string("PropertyTree error: ") + e.what());
    }
}

void DataLoader::loadSlideShow( const std::string& fileName, SlideShow* pSlideShow )
{
    try
    {
        using boost::shared_ptr;
        using boost::make_shared;

        gAaLog.write ( "Loading slide show file \"%s\"...\n", fileName.c_str() );
        gAaLog.increaseIndentationLevel();

        ptree propTree;
        read_info(fileName, propTree);

        pSlideShow->musicFileName = propTree.get<std::string>("music");
        pSlideShow->timerDelay = 5000;

        BOOST_FOREACH(const ptree::value_type &value, propTree)
        {
            if (value.first == "slide")
            {
                ptree slidePropTree = value.second;
                Slide slide;
                slide.imageFileName = slidePropTree.get<std::string>("image");

                BOOST_FOREACH(const ptree::value_type &value2, slidePropTree)
                {
                    if (value2.first == "text")
                    {
                        std::string text = value2.second.get_value("");
                        slide.textPages.push_back(text);
                    }
                }

                pSlideShow->slides.push_back( slide );
            }
        }

        gAaLog.decreaseIndentationLevel();
        gAaLog.write ( "[ Done ]\n\n", fileName.c_str() );
    }
    catch (boost::property_tree::ptree_error& e)
    {
        throw DataLoadException(std::string("PropertyTree error: ") + e.what());
    }
}

ResourceIds DataLoader::loadGraphics( const std::string& fileName, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager )
{
    gAaLog.write ( "Loading XML file \"%s\"...\n", fileName.c_str() );
    gAaLog.increaseIndentationLevel();

    ResourceIds loadedResources;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(fileName.c_str());
    if (!result)
    {
        gAaLog.write( "[ Error parsing file '%s' at offset %d!\nError description: %s ]\n\n", fileName.c_str(), result.offset, result.description() );
        return loadedResources;
    }

    pugi::xml_node rootElem = doc.first_child();

    bool noMipmaps = !rootElem.child("noMipmaps").empty();

    // Texturen laden
    if ( pTextureManager )
    {
        for (pugi::xml_node texElement = rootElem.child("Texture"); texElement ; texElement = texElement.next_sibling("Texture"))
        {
            const char* name = texElement.attribute("file").value();
            const char* id = texElement.attribute("id").value();
            bool mipMaps = texElement.attribute("mipMaps").as_bool();
            bool texRepeat = texElement.attribute("texRepeat").as_bool(); // deprecated
            bool texRepeatX = texElement.attribute("texRepeatX").as_bool();
            bool texRepeatY = texElement.attribute("texRepeatY").as_bool();
            if (texRepeat)
            {
                texRepeatX = true;
                texRepeatY = true;
            }

            float scale = texElement.attribute("scale").as_float();
            if (scale == 0.0f)
                scale = 1.0f;

            LoadTextureInfo info;
            if ( mipMaps && !noMipmaps )
                info.loadMipmaps = true;
            else
                info.loadMipmaps = false;

            if ( texRepeatX )
                info.wrapModeX = LoadTextureInfo::WrapRepeat;
            else
                info.wrapModeX = LoadTextureInfo::WrapClamp;
            if ( texRepeatY )
                info.wrapModeY = LoadTextureInfo::WrapRepeat;
            else
                info.wrapModeY = LoadTextureInfo::WrapClamp;

            info.scale = scale;
            info.quality = (LoadTextureInfo::Quality) gConfig.get<int>("TexQuality");

            pTextureManager->loadTexture(name, id, info);
            loadedResources.textures.insert(id);
        }
    }

    // Animationen laden
    if ( pAnimationManager )
    {
        for (pugi::xml_node animElement = rootElem.child("Animation"); animElement ; animElement = animElement.next_sibling("Animation"))
        {
            const char* name = animElement.attribute("file").value();
            const char* id = animElement.attribute("id").value();
            bool mipMaps = animElement.attribute("mipMaps").as_bool();
            bool texRepeat = animElement.attribute("texRepeat").as_bool(); // deprecated
            bool texRepeatX = animElement.attribute("texRepeatX").as_bool();
            bool texRepeatY = animElement.attribute("texRepeatY").as_bool();
            if (texRepeat)
            {
                texRepeatX = true;
                texRepeatY = true;
            }

            LoadTextureInfo info;
            if ( mipMaps && !noMipmaps )
                info.loadMipmaps = true;
            else
                info.loadMipmaps = false;

            if ( texRepeatX )
                info.wrapModeX = LoadTextureInfo::WrapRepeat;
            else
                info.wrapModeX = LoadTextureInfo::WrapClamp;
            if ( texRepeatY )
                info.wrapModeY = LoadTextureInfo::WrapRepeat;
            else
                info.wrapModeY = LoadTextureInfo::WrapClamp;

            info.scale = 1.0f;
            info.quality = (LoadTextureInfo::Quality) gConfig.get<int>("TexQuality");

            pAnimationManager->loadAnimation(name, id, info);
            loadedResources.animations.insert(id);
        }
    }

    // Schriften laden
    if ( pFontManager )
    {
        for (pugi::xml_node fontElement = rootElem.child("Font"); fontElement ; fontElement = fontElement.next_sibling("Font"))
        {
            const char* name = fontElement.attribute("file").value();
            const char* id = fontElement.attribute("id").value();
            int size = fontElement.attribute("size").as_int();

            pFontManager->loadFont(name,size,id);
            loadedResources.fonts.insert(id);
        }
    }
    gAaLog.decreaseIndentationLevel();
    gAaLog.write ( "[ Done ]\n\n", fileName.c_str() );
    return loadedResources;
}

void DataLoader::unLoadGraphics( const ResourceIds& resourcesToUnload, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager )
{
    gAaLog.write ( "Unloading resources... " );

    // Texturen laden
    if ( pTextureManager )
    {
        BOOST_FOREACH(const TextureId& id, resourcesToUnload.textures)
            pTextureManager->freeTexture(id);
    }

    // Animationen laden
    if ( pAnimationManager )
    {
        BOOST_FOREACH(const AnimationId& id, resourcesToUnload.animations)
            pAnimationManager->freeAnimation(id);
    }

    // Schriften laden
    if ( pFontManager )
    {
        BOOST_FOREACH(const FontId& id, resourcesToUnload.fonts)
            pFontManager->freeFont(id);
    }
    gAaLog.write ( "[ Done ]\n\n" );
}

void DataLoader::saveWorldToXml(const std::string& fileName, const World& gameWorld)
{
    gAaLog.write ( "Saving XML file \"%s\"...\n", fileName.c_str() );
    gAaLog.increaseIndentationLevel();
    
    ptree levelPropTree;
	
    const EntityMap& entities = gameWorld.getCompManager().getAllEntities();
    BOOST_FOREACH(const EntityMap::value_type& entPair, entities)
    {
        ptree entityPropTree;
        entityPropTree.add("id", entPair.first);

        const ComponentMap& comps = entPair.second;
        BOOST_FOREACH(const ComponentMap::value_type& compPair, comps)
        {
            ptree compPropTree;
            compPropTree.add("type", compPair.second->getTypeId());
            std::string compId = compPair.second->getId();
            if (!compId.empty())
                compPropTree.add("id", compId);
            compPair.second->writeToPropertyTree(compPropTree);

            entityPropTree.add_child("component", compPropTree);
        }

        levelPropTree.add_child("entity", entityPropTree);
    }
    
    write_info(fileName, levelPropTree, std::locale(), boost::property_tree::info_parser::info_writer_settings<char>('\t',1));

    gAaLog.decreaseIndentationLevel();
    gAaLog.write ( "[ Done saving XML file \"%s\" ]\n\n", fileName.c_str() );
}
