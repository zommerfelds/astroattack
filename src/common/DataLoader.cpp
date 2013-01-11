/*
 * DataLoader.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "DataLoader.h"

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
#include "common/components/CompVariable.h"
#include "common/components/CompShape.h"
#include "common/components/CompPath.h"
#include "common/components/CompPathMove.h"

#include "common/ComponentManager.h"
#include "common/Logger.h"
#include "common/GameEvents.h"
#include "common/Renderer.h"
#include "common/Physics.h"
#include "common/Font.h"
#include "common/Texture.h"
#include "common/Foreach.h"

#include <sstream>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

using boost::property_tree::ptree;

DataLoadException::DataLoadException(const std::string& msg)
 : Exception (msg)
{
}

namespace
{
    template <typename T>
    T get(const ptree& prop, const std::string& key, const std::string& errorMsg = "")
    {
        try
        {
            return prop.get<T>(key);
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            throw DataLoadException(errorMsg + e.what());
        }
    }
}

// Load Level from XML
void DataLoader::loadToWorld(const std::string& fileName, ComponentManager& compMgr, GameEvents& events)
{
    std::string errMsg;
    try
    {
        using boost::shared_ptr;
        using boost::make_shared;

        log(Info) << "Loading world file \"" << fileName << "\"...\n";

        ptree levelPropTree;
        read_info(fileName, levelPropTree);

        foreach(const ptree::value_type &value1, levelPropTree)
        {
            if (value1.first != "entity")
                throw DataLoadException(fileName + ": at top level: parse error, expected 'entity', got '" + value1.first + "'");
            const ptree& entityPropTree = value1.second;

            EntityId entityId = get<std::string>(entityPropTree, "id", fileName + ": entity must have node 'id'");

            ComponentList entity;
            log(Info) << "  Creating entity \"" << entityId << "\"\n";

            foreach(const ptree::value_type &value2, entityPropTree)
            {
                if (value2.first == "id")
                    continue;
                if (value2.first != "component")
                    throw DataLoadException(fileName + ": in entity '" + entityId + "': parse error, expected 'component', got '" + value2.first + "'");
                const ptree& compPropTree = value2.second;

                std::string compType = get<std::string>(compPropTree, "type", fileName + ": in entity '"+entityId+"', component missing 'type' node");
                std::string compId = compPropTree.get("id", "");
                log(Info) << "    Creating component \"" << compType << "\"... ";

                shared_ptr<Component> component;

                if ( compType == CompShape::getTypeIdStatic() )
                {
                    if (compPropTree.count("polygon"))
                        component = boost::shared_ptr<CompShapePolygon>(new CompShapePolygon(compId, events));
                    else
                        component = boost::shared_ptr<CompShapeCircle>(new CompShapeCircle(compId, events));
                }
                else if ( compType == CompPhysics::getTypeIdStatic() )
                    component = boost::shared_ptr<CompPhysics>(new CompPhysics(compId, events));
                else if ( compType == CompPlayerController::getTypeIdStatic() )
                    component = boost::shared_ptr<CompPlayerController>(new CompPlayerController(compId, events));
                else if ( compType == CompPosition::getTypeIdStatic() )
                    component = boost::shared_ptr<CompPosition>(new CompPosition(compId, events));
                else if ( compType == CompVisualAnimation::getTypeIdStatic() )
                    component = boost::shared_ptr<CompVisualAnimation>(new CompVisualAnimation(compId, events));
                else if ( compType == CompVisualTexture::getTypeIdStatic() )
                    component = boost::shared_ptr<CompVisualTexture>(new CompVisualTexture(compId, events));
                else if ( compType == CompVisualMessage::getTypeIdStatic() )
                    component = boost::shared_ptr<CompVisualMessage>(new CompVisualMessage(compId, events));
                else if ( compType == CompGravField::getTypeIdStatic() )
                    component = boost::shared_ptr<CompGravField>(new CompGravField(compId, events));
                else if ( compType == CompTrigger::getTypeIdStatic() )
                    component = boost::shared_ptr<CompTrigger>(new CompTrigger(compId, events));
                else if ( compType == CompVariable::getTypeIdStatic() )
                    component = boost::shared_ptr<CompVariable>(new CompVariable(compId, events));
                else if ( compType == CompPath::getTypeIdStatic() )
                    component = boost::make_shared<CompPath>(compId, boost::ref(events));
                else if ( compType == CompPathMove::getTypeIdStatic() )
                    component = boost::make_shared<CompPathMove>(compId, boost::ref(events));
                else
                    throw DataLoadException(fileName + ": in entity '" + entityId + "' component '" + compId + "', invalid component type '" + compType + "'");

                assert(component);

                try
                {
                    component->loadFromPropertyTree(compPropTree);
                }
                catch (boost::property_tree::ptree_bad_path& e)
                {
                    throw DataLoadException(fileName + ": in " + entityId + "." + compId + "(" + compType + "), " + e.what());
                }

                entity.push_back(component);

                log(Info) << "  [ Done ]\n";
            }

            compMgr.addEntity(entityId, entity);
        }

        log(Info) << "[ Done ]\n\n";
    }
    catch (boost::property_tree::ptree_error& e)
    {
        throw DataLoadException(e.what()); // ptree_error shows filename
    }
}

ResourceIds DataLoader::loadGraphics(const std::string& fileName, TextureManager* textureManager, AnimationManager* animationManager, FontManager* fontManager, TexQuality quality)
{
    log(Info) << "Loading graphics resource file \"" << fileName << "\"...\n";

    ResourceIds loadedResources;

    try
    {
        using boost::shared_ptr;
        using boost::make_shared;

        ptree propTree;
        read_info(fileName, propTree);

        bool noMipmaps = propTree.get("noMipmaps", false);

        // Texturen laden
        if (textureManager)
        {
            foreach(const ptree::value_type &value, propTree)
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
                    info.wrapModeX = WrapRepeat;
                else
                    info.wrapModeX = WrapClamp;
                if ( texRepeatY )
                    info.wrapModeY = WrapRepeat;
                else
                    info.wrapModeY = WrapClamp;

                info.scale = scale;
                info.quality = quality;

                textureManager->loadTexture(fileName, id, info);
                loadedResources.textures.insert(id);
            }
        }

        // Animationen laden
        if (animationManager)
        {
            foreach(const ptree::value_type &value, propTree)
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
                    info.wrapModeX = WrapRepeat;
                else
                    info.wrapModeX = WrapClamp;
                if ( texRepeatY )
                    info.wrapModeY = WrapRepeat;
                else
                    info.wrapModeY = WrapClamp;

                info.scale = 1.0f;
                info.quality = quality;

                animationManager->loadAnimation(fileName, id, info);
                loadedResources.animations.insert(id);
            }
        }

        // Schriften laden
        if (fontManager)
        {
            foreach(const ptree::value_type &value, propTree)
            {
                if (value.first != "font")
                    continue;
                ptree fontPropTree = value.second;

                std::string name = fontPropTree.get<std::string>("file");
                std::string id = fontPropTree.get<std::string>("id");
                float sizeF = fontPropTree.get<float>("size");
                int sizeI = fontPropTree.get("size", 0);
                bool fix = fontPropTree.get("fix", false);

                if (fix)
                    fontManager->loadFontFix(name, sizeI, id);
                else
                    fontManager->loadFontRel(name, sizeF, id);
                loadedResources.fonts.insert(id);
            }
        }
    }
    catch (boost::property_tree::ptree_error& e)
    {
        throw DataLoadException(std::string("reading file '" + fileName + "': ") + e.what());
    }
    log(Info) << "[ Done ]\n\n";
    return loadedResources;
}

void DataLoader::unLoadGraphics( const ResourceIds& resourcesToUnload, TextureManager* textureManager, AnimationManager* animationManager, FontManager* fontManager )
{
    if (!resourcesToUnload.isEmpty())
        log(Info) << "Unloading resources... ";

    // Texturen laden
    if (textureManager)
    {
        foreach(const TextureId& id, resourcesToUnload.textures)
            textureManager->freeTexture(id);
    }

    // Animationen laden
    if (animationManager)
    {
        foreach(const AnimationId& id, resourcesToUnload.animations)
            animationManager->freeAnimation(id);
    }

    // Schriften laden
    if (fontManager)
    {
        foreach(const FontId& id, resourcesToUnload.fonts)
            fontManager->freeFont(id);
    }

    log(Info) << "[ Done ]\n\n";
}

void DataLoader::saveWorld(const std::string& fileName, const ComponentManager& compMgr)
{
    log(Info) << "Saving world to file \"" << fileName << "\"...\n";
    
    ptree levelPropTree;
    
    const EntityMap& entities = compMgr.getAllEntities();
    foreach(const EntityMap::value_type& entPair, entities)
    {
        ptree entityPropTree;
        entityPropTree.add("id", entPair.first);

        const ComponentMap& comps = entPair.second;
        foreach(const ComponentMap::value_type& compPair, comps)
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

    log(Info) << "[ Done ]\n\n";
}
