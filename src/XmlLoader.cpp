/*
 * XmlLoader.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "XmlLoader.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "World.h"
#include "main.h"
#include "GameApp.h"

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

#include <boost/foreach.hpp>
#include "contrib/pugixml/pugixml.hpp"
#include "contrib/pugixml/foreach.hpp"

#include "Renderer.h"
#include "Physics.h"

#include "states/SlideShowState.h"

#include <sstream>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include "Font.h"
#include "Texture.h"

// TODO: a lot of work to do here (error checking etc..)

// Load Level from XML
void XmlLoader::loadXmlToWorld( const char* pFileName, GameWorld& gameWorld, SubSystems& subSystems )
{
	using boost::shared_ptr;
	using boost::make_shared;

    gAaLog.write ( "Loading XML file \"%s\"...\n", pFileName );
    gAaLog.increaseIndentationLevel();

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(pFileName);
    if (!result)
    {
        gAaLog.write( "[ Error parsing file '%s' at offset %d!\nError description: %s ]\n\n", pFileName, result.offset, result.description() );
        return;
    }

    BOOST_FOREACH(const pugi::xml_node& entityElem, doc.first_child() )
    {
        const char* entityName = entityElem.attribute("name").value();
        shared_ptr<Entity> pEntity = make_shared<Entity>( entityName );
        gAaLog.write ( "Creating entity \"%s\"\n", entityName );
        gAaLog.increaseIndentationLevel();

        BOOST_FOREACH(const pugi::xml_node& compElem, entityElem)
        {
            std::string compId = compElem.attribute("id").value();
            if ( compId.empty() )
                continue;
            std::string compName = compElem.attribute("name").value();
            gAaLog.write ( "Creating component \"%s\"... ", compId.c_str() );

            shared_ptr<Component> component;

            if ( compId == "CompShape" )
            {
                component = CompShape::loadFromXml( compElem );
            }
            else if ( compId == "CompPhysics" )
            {
                component = CompPhysics::loadFromXml( compElem );
            }
            else if ( compId == "CompPlayerController" )
            {
                component = CompPlayerController::loadFromXml( compElem, subSystems.input, gameWorld.getItToVariable( "JetpackEnergy" ) );
            }
            else if ( compId == "CompPosition" )
            {
                component = CompPosition::loadFromXml( compElem );
            }
            else if ( compId == "CompVisualAnimation" )
            {
                component = CompVisualAnimation::loadFromXml( compElem, subSystems.renderer.getAnimationManager() );
            }
            else if ( compId == "CompVisualTexture" )
            {
                component = CompVisualTexture::loadFromXml( compElem );
            }
            else if ( compId == "CompVisualMessage" )
            {
                component = CompVisualMessage::loadFromXml( compElem );
            }
			else if ( compId == "CompGravField" )
            {
			    component = CompGravField::loadFromXml( compElem );
            }
            else if ( compId == "CompTrigger" )
            {
                component = CompTrigger::loadFromXml( compElem, gameWorld );
            }
            else
            {
                gAaLog.write ( "Component ID not found!" );
                continue;
            }

            if ( !component )
            {
                gAaLog.write ( "Error, component was not allocated!" );
                continue;
            }

            if ( !compName.empty() )
            	component->setName( compName );
            pEntity->addComponent( component );

            gAaLog.write ( "[ Done ]\n" );
        }

        gameWorld.addEntity( pEntity );
        gAaLog.decreaseIndentationLevel();
    }

    gAaLog.decreaseIndentationLevel();
    gAaLog.write ( "[ Done ]\n\n", pFileName );
}

void XmlLoader::loadSlideShow( const char* pFileName, SlideShow* pSlideShow )
{
    gAaLog.write ( "Loading XML file \"%s\"...\n", pFileName );
    gAaLog.increaseIndentationLevel();

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(pFileName);
    if (!result)
    {
		gAaLog.write( "[ Error parsing file '%s' at offset %d!\nError description: %s ]\n\n", pFileName, result.offset, result.description() );
		return;
	}

	pugi::xml_node rootElem = doc.first_child();

    pSlideShow->musicFileName = "";
    pSlideShow->timerDelay = 0;

    pSlideShow->musicFileName = rootElem.child("music").attribute("file").value();

    pSlideShow->timerDelay = rootElem.child("timer").attribute("delayms").as_int();

    for(pugi::xml_node slideElem = rootElem.child("slide"); slideElem; slideElem = slideElem.next_sibling("slide"))
    {
        Slide slide;
        slide.imageFileName = slideElem.attribute("image").value();
        std::string text;
        
        BOOST_FOREACH(const pugi::xml_node& childElem, slideElem)
        {
            if (std::string("p") == childElem.name())
            {
                slide.textPages.push_back(text);
                text.clear();
            }
            else
            {
                text += childElem.value();
            }
        }
        if (!text.empty())
            slide.textPages.push_back(text);
        pSlideShow->slides.push_back( slide );
    }

    gAaLog.decreaseIndentationLevel();
    gAaLog.write ( "[ Done ]\n\n" );
}

ResourceIds XmlLoader::loadGraphics( const char* pFileName, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager )
{
    gAaLog.write ( "Loading XML file \"%s\"...\n", pFileName );
    gAaLog.increaseIndentationLevel();

    ResourceIds loadedResources;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(pFileName);
    if (!result)
    {
        gAaLog.write( "[ Error parsing file '%s' at offset %d!\nError description: %s ]\n\n", pFileName, result.offset, result.description() );
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
                info.textureWrapModeX = TEX_REPEAT;
            else
                info.textureWrapModeX = TEX_CLAMP;
            if ( texRepeatY )
                info.textureWrapModeY = TEX_REPEAT;
            else
                info.textureWrapModeY = TEX_CLAMP;

            info.scale = scale;

            pTextureManager->loadTexture(name,id,info,gAaConfig.getInt("TexQuality"));
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
                info.textureWrapModeX = TEX_REPEAT;
            else
                info.textureWrapModeX = TEX_CLAMP;
            if ( texRepeatY )
                info.textureWrapModeY = TEX_REPEAT;
            else
                info.textureWrapModeY = TEX_CLAMP;

            info.scale = 1.0f;

            pAnimationManager->loadAnimation(name,id,info,gAaConfig.getInt("TexQuality"));
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
    gAaLog.write ( "[ Done ]\n\n", pFileName );
    return loadedResources;
}

void XmlLoader::unLoadGraphics( const ResourceIds& resourcesToUnload, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager )
{
    gAaLog.write ( "Unloading resources... " );

    // Texturen laden
    if ( pTextureManager )
    {
        BOOST_FOREACH(const TextureIdType& id, resourcesToUnload.textures)
            pTextureManager->freeTexture(id);
    }

    // Animationen laden
    if ( pAnimationManager )
    {
        BOOST_FOREACH(const AnimationIdType& id, resourcesToUnload.animations)
            pAnimationManager->freeAnimation(id);
    }

    // Schriften laden
    if ( pFontManager )
    {
        BOOST_FOREACH(const FontIdType& id, resourcesToUnload.fonts)
            pFontManager->freeFont(id);
    }
    gAaLog.write ( "[ Done ]\n\n" );
}

void XmlLoader::saveWorldToXml( const char* pFileName, const GameWorld& gameWorld )
{
    gAaLog.write ( "Saving XML file \"%s\"...\n", pFileName );
    gAaLog.increaseIndentationLevel();
    
    pugi::xml_document doc;

    pugi::xml_node levelNode = doc.append_child();
    levelNode.set_name("level");
	
    const EntityMap& entities = gameWorld.getAllEntities();
    BOOST_FOREACH(const EntityMap::value_type& entPair, entities)
    {
        pugi::xml_node entityNode = levelNode.append_child("entity");
        entityNode.append_attribute("name").set_value(entPair.second->getId().c_str());

        const ComponentMap& comps = entPair.second->getAllComponents();
        BOOST_FOREACH(const ComponentMap::value_type& compPair, comps)
        {
            pugi::xml_node compNode = entityNode.append_child("component");
            compNode.append_attribute("id").set_value(compPair.second->getComponentId().c_str());
            compNode.append_attribute("name").set_value(compPair.second->getName().c_str());

            compPair.second->writeToXml(compNode);
        }
    }

    doc.save_file(pFileName);
    
    gAaLog.decreaseIndentationLevel();
    gAaLog.write ( "[ Done saving XML file \"%s\" ]\n\n", pFileName );
}
