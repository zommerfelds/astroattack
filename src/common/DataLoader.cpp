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
#include "common/World.h"
#include "game/Logger.h"
#include "game/Configuration.h"
#include "common/GameEvents.h"
#include "common/Renderer.h"
#include "common/Physics.h"
#include "common/Font.h"
#include "common/Texture.h"

#include "common/components/CompGravField.h"
#include "common/components/CompVisualTexture.h"
#include "common/components/CompPhysics.h"
#include "common/components/CompPosition.h"
#include "common/components/CompPlayerController.h"
#include "common/components/CompVisualAnimation.h"
#include "common/components/CompTrigger.h"
#include "common/components/CompTrigger_Effects.h"
#include "common/components/CompTrigger_Conditions.h"
#include "common/components/CompVisualMessage.h"
#include "common/components/CompShape.h"

#include "game/states/SlideShowState.h"

// TODO better checking of input data

using boost::property_tree::ptree;

DataLoadException::DataLoadException(const std::string& msg)
 : Exception (msg)
{
}

// Load Level from XML
void DataLoader::loadWorld(const std::string& fileName, World& gameWorld, GameEvents& events)
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
                throw DataLoadException(fileName + " - At top level: parse error, expected 'entity', got '" + value1.first + "'");
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
                    throw DataLoadException(fileName + " - In entity '" + entityId + "': parse error, expected 'component', got '" + value2.first + "'");
                const ptree& compPropTree = value2.second;

                std::string compType = compPropTree.get<std::string>("type");
                std::string compId = compPropTree.get("id", "");
                gAaLog.write ( "Creating component \"%s\"... ", compType.c_str() );

                shared_ptr<Component> component;

                if ( compType == "CompShape" )
                {
                    if (compPropTree.count("polygon"))
                        component = boost::shared_ptr<CompShapePolygon>(new CompShapePolygon(compId, events));
                    else
                        component = boost::shared_ptr<CompShapeCircle>(new CompShapeCircle(compId, events));
                }
                else if ( compType == "CompPhysics" )
                    component = boost::shared_ptr<CompPhysics>(new CompPhysics(compId, events));
                else if ( compType == "CompPlayerController" )
                    component = boost::shared_ptr<CompPlayerController>(new CompPlayerController(compId, events, gameWorld.getItToVariable( "JetpackEnergy" )));
                else if ( compType == "CompPosition" )
                    component = boost::shared_ptr<CompPosition>(new CompPosition(compId, events));
                else if ( compType == "CompVisualAnimation" )
                    component = boost::shared_ptr<CompVisualAnimation>(new CompVisualAnimation(compId, events));
                else if ( compType == "CompVisualTexture" )
                    component = boost::shared_ptr<CompVisualTexture>(new CompVisualTexture(compId, events));
                else if ( compType == "CompVisualMessage" )
                    component = boost::shared_ptr<CompVisualMessage>(new CompVisualMessage(compId, events));
                else if ( compType == "CompGravField" )
                    component = boost::shared_ptr<CompGravField>(new CompGravField(compId, events));
                else if ( compType == "CompTrigger" )
                    component = boost::shared_ptr<CompTrigger>(new CompTrigger(compId, gameWorld, events));
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
        throw DataLoadException(std::string("PropertyTree error parsing file '" + fileName + "': ") + e.what());
    }
}

ResourceIds DataLoader::loadGraphics( const std::string& fileName, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager )
{
	gAaLog.write ( "Loading graphics resource file \"%s\"...\n", fileName.c_str() );
	gAaLog.increaseIndentationLevel();

	ResourceIds loadedResources;

    try
	{
		using boost::shared_ptr;
		using boost::make_shared;

		ptree propTree;
		read_info(fileName, propTree);

		bool noMipmaps = propTree.get("noMipmaps", false);

		// Texturen laden
		if ( pTextureManager )
		{
			BOOST_FOREACH(const ptree::value_type &value, propTree)
			{
				if (value.first != "texture")
					continue;
				ptree texPropTree = value.second;

				std::string fileName = texPropTree.get<std::string>("file");
				std::string id = texPropTree.get<std::string>("id");
				bool mipMaps = texPropTree.get("mipMaps", true);
				bool texRepeat = texPropTree.get("texRepeat", false); // deprecated (?)
				bool texRepeatX = texPropTree.get("texRepeatX", false);
				bool texRepeatY = texPropTree.get("texRepeatY", false);
				if (texRepeat)
				{
					texRepeatX = true;
					texRepeatY = true;
				}

				float scale = texPropTree.get("scale", 1.0f);

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
				info.quality = LoadTextureInfo::QualityBest; // TODO (LoadTextureInfo::Quality) gConfig.get<int>("TexQuality");

				pTextureManager->loadTexture(fileName, id, info);
				loadedResources.textures.insert(id);
			}
		}

		// Animationen laden
		if ( pAnimationManager )
		{
			BOOST_FOREACH(const ptree::value_type &value, propTree)
			{
				if (value.first != "animation")
					continue;
				ptree animPropTree = value.second;

				std::string fileName = animPropTree.get<std::string>("file");
				std::string id = animPropTree.get<std::string>("id");
				bool mipMaps = animPropTree.get("mipMaps", true);
				bool texRepeat = animPropTree.get("texRepeat", false); // deprecated (?)
				bool texRepeatX = animPropTree.get("texRepeatX", false);
				bool texRepeatY = animPropTree.get("texRepeatY", false);
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
				info.quality = LoadTextureInfo::QualityBest; // TODO (LoadTextureInfo::Quality) gConfig.get<int>("TexQuality");

				pAnimationManager->loadAnimation(fileName, id, info);
				loadedResources.animations.insert(id);
			}
		}

		// Schriften laden
		if ( pFontManager )
		{
			BOOST_FOREACH(const ptree::value_type &value, propTree)
			{
				if (value.first != "font")
					continue;
				ptree fontPropTree = value.second;

				std::string name = fontPropTree.get<std::string>("file");
				std::string id = fontPropTree.get<std::string>("id");
				int size = fontPropTree.get<int>("size");

				pFontManager->loadFont(name, size, id);
				loadedResources.fonts.insert(id);
			}
		}
	}
	catch (boost::property_tree::ptree_error& e)
	{
        throw DataLoadException(std::string("PropertyTree error parsing file '" + fileName + "': ") + e.what());
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

void DataLoader::saveWorld(const std::string& fileName, const World& gameWorld)
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
