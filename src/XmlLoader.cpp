/*----------------------------------------------------------\
|                     XmlLoader.cpp                         |
|                     -------------                         |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

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

#include <tinyxml.h>

#include "Renderer.h"
#include "Physics.h"

#include "states/SlideShowState.h"

#include <sstream>
//#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include "Font.h"
#include "Texture.h"

// TODO: don't read XML file again to unload textures
// TODO: a lot of work to do here (error checking etc..)

// Load Level from XML
void XmlLoader::LoadXmlToWorld( const char* pFileName, GameWorld* pGameWorld, SubSystems* pSubSystems )
{
	using boost::shared_ptr;
	using boost::make_shared;

    // OPEN XML
    gAaLog.Write ( "Loading XML file \"%s\"...\n", pFileName );
    gAaLog.IncreaseIndentationLevel();

    TiXmlDocument doc(pFileName);
	if (!doc.LoadFile())
	{
		gAaLog.Write ( "[ Error opening file! ]\n\n", pFileName );
		return;
	}

	TiXmlHandle hRoot(0);
    {
        TiXmlElement* pElem = doc.FirstChildElement();
	    if (!pElem)
		{
			gAaLog.Write ( "[ Error reading XML! ]\n\n", pFileName );
			return;
		}
	    hRoot=TiXmlHandle(pElem);
    }
    // END OF OPEN XML

    TiXmlElement* entityElement = hRoot.FirstChildElement().ToElement();

    for( ; entityElement; entityElement = entityElement->NextSiblingElement() )
    {
        const char* entityName = entityElement->Attribute("name");
        shared_ptr<Entity> pEntity( make_shared<Entity>( entityName ) );
        gAaLog.Write ( "Creating entity \"%s\"\n", entityName );
        gAaLog.IncreaseIndentationLevel();

        TiXmlElement* componentElement = entityElement->FirstChildElement();

        for( ; componentElement; componentElement = componentElement->NextSiblingElement() )
        {
            const char* compId = componentElement->Attribute("id");
            std::string componentId;
            if ( compId != NULL )
                componentId = compId;
            else
                continue;
            const char* componentName = componentElement->Attribute("name");
            gAaLog.Write ( "Creating component \"%s\"... ", componentId.c_str() );

            shared_ptr<Component> component;

            if ( componentId == "CompShape" )
            {
                component = CompShape::LoadFromXml( *componentElement );
            }
            else if ( componentId == "CompPhysics" )
            {
                bool allowSleep = true;
                float angle = 0.0f;
                float angularDamping = 0.0f, linearDamping = 0.0f;
                bool fixedRotation = false;
                bool isBullet = false;
                //float posX = 0.0f, posY = 0.0f;
                //bool saveContacts = false; // wenn true ist allowSleep automatisch false
                bool dynamic = false;
                Vector2D rotationPoint;
                Vector2D gravitationPoint;

                /*TiXmlElement* posElement = componentElement->FirstChildElement( "pos" );
                posElement->QueryFloatAttribute( "x", &posX );
                posElement->QueryFloatAttribute( "y", &posY );
                posElement->QueryFloatAttribute( "a", &angle );*/

                TiXmlElement* sleepElement = componentElement->FirstChildElement( "dontSleep" );
                if ( sleepElement )
                    allowSleep = false;

                TiXmlElement* dampingElement = componentElement->FirstChildElement( "damping" );
                dampingElement->QueryFloatAttribute( "linear", &linearDamping );
                dampingElement->QueryFloatAttribute( "angular", &angularDamping );

                TiXmlElement* fixedRotationElement = componentElement->FirstChildElement( "fixedRotation" );
                if ( fixedRotationElement )
                    fixedRotation = true;

                TiXmlElement* isBulletElement = componentElement->FirstChildElement( "bullet" );
                if ( isBulletElement )
                    isBullet = true;

                TiXmlElement* dynamicElement = componentElement->FirstChildElement( "dynamic" );
                if ( dynamicElement )
                    dynamic = true;

                TiXmlElement* rotationPointElement = componentElement->FirstChildElement( "rotationPoint" );
                if ( rotationPointElement )
                {
                    rotationPointElement->QueryFloatAttribute( "x", &rotationPoint.x );
                    rotationPointElement->QueryFloatAttribute( "y", &rotationPoint.y );
                }

                TiXmlElement* gravitationPointElement = componentElement->FirstChildElement( "gravitationPoint" );
                if ( gravitationPointElement )
                {
                    gravitationPointElement->QueryFloatAttribute( "x", &gravitationPoint.x );
                    gravitationPointElement->QueryFloatAttribute( "y", &gravitationPoint.y );
                }

                /*TiXmlElement* saveContactsElement = componentElement->FirstChildElement( "saveContacts" );
                if ( saveContactsElement )
                    saveContacts = true;*/

                /*if ( saveContacts )
                    allowSleep = false;*/
                BodyDef body_def;
                body_def.angle = angle;
                body_def.angularDamping = angularDamping;
                body_def.fixedRotation = fixedRotation;
                body_def.bullet = isBullet;
                body_def.linearDamping = linearDamping;
                //body_def->position.Set( posX, posY );
                //if (dynamic)
                    //body_def->type = b2_dynamicBody;
                shared_ptr<CompPhysics> compPhysics = make_shared<CompPhysics>();
                compPhysics->SetLocalRotationPoint( rotationPoint );
                compPhysics->SetLocalGravitationPoint( gravitationPoint );

                TiXmlElement* shapeElement = componentElement->FirstChildElement("shape");
				for (; shapeElement; shapeElement = shapeElement->NextSiblingElement("shape"))
				{
					//TODO: tell tinyxml to use std::strings
					const char* name = shapeElement->Attribute("comp_name");
					std::string shapeName;
					if (name != NULL)
						shapeName = name;

                    float density = 0.0f;
                    float friction = 0.0f;
                    float restitution = 0.0f;
                    bool isSensor = false;

                    shapeElement->QueryFloatAttribute("density", &density);
                    shapeElement->QueryFloatAttribute("friction", &friction);
                    shapeElement->QueryFloatAttribute("restitution", &restitution);

                    if (density != 0.0f)
                        body_def.type = BodyDef::dynamicBody;

                    //body_def->

					const char* strIsSensor = shapeElement->Attribute("isSensor");
					if (strIsSensor != NULL && std::string(strIsSensor)=="true") {
						isSensor = true;
					}

					compPhysics->AddShapeDef( boost::make_shared<ShapeDef>(shapeName, density, friction, restitution, isSensor) );
				}

				compPhysics->SetBodyDef(body_def);
                component = compPhysics;
            }
            else if ( componentId == "CompPlayerController" )
            {
                shared_ptr<CompPlayerController> compPlayerController( make_shared<CompPlayerController>( pSubSystems->input.get(), pGameWorld->GetItToVariable( "JetpackEnergy" ), boost::bind( &PhysicsSubSystem::Refilter, pSubSystems->physics.get(), _1 ) ) );

                component = compPlayerController;
            }
            else if ( componentId == "CompPosition" )
            {
                float x = 0.0f;
                float y = 0.0f;
                float a = 0.0f;
                componentElement->QueryFloatAttribute( "x", &x );
                componentElement->QueryFloatAttribute( "y", &y );
                componentElement->QueryFloatAttribute( "a", &a );

                if (y<0)
                    a = 1;

                shared_ptr<CompPosition> compPos = make_shared<CompPosition>();
                compPos->SetPosition( Vector2D(x, y) );
                compPos->SetOrientation( a );

                component = compPos;
            }
            else if ( componentId == "CompVisualAnimation" )
            {
                float hw = 1.0f, hh = 1.0f;
                float centerX = 0.0f, centerY = 0.0f;
                const char* animName = NULL;
                bool start = false;

                TiXmlElement* animElement = componentElement->FirstChildElement( "anim" );
                animName = animElement->Attribute("name");

                TiXmlElement* dimElement = componentElement->FirstChildElement( "dim" );
                dimElement->QueryFloatAttribute("hw",&hw);
                dimElement->QueryFloatAttribute("hh",&hh);

                TiXmlElement* centerElement = componentElement->FirstChildElement( "center" );
                centerElement->QueryFloatAttribute("x",&centerX);
                centerElement->QueryFloatAttribute("y",&centerY);

                TiXmlElement* startElement = componentElement->FirstChildElement( "start" );
                if ( startElement )
                    start = true;

                shared_ptr<CompVisualAnimation> compAnim( make_shared<CompVisualAnimation>(pSubSystems->renderer->GetAnimationManager()->GetAnimInfo(animName) ) );
                compAnim->Center()->Set(centerX,centerY);
                compAnim->SetDimensions( hw, hh );
                component = compAnim;
                if ( start )
                    compAnim->Start();
            }
            else if ( componentId == "CompVisualTexture" )
            {
                component = CompVisualTexture::LoadFromXml( *componentElement );
            }
            else if ( componentId == "CompVisualMessage" )
            {
                TiXmlElement* msgElement = componentElement->FirstChildElement( "msg" );
                const char* msg = msgElement->Attribute("text");

                shared_ptr<CompVisualMessage> compMsg( make_shared<CompVisualMessage>(msg) );

                component = compMsg;
            }
			else if ( componentId == "CompGravField" ) // uidaternuditarendutirane
            {
                TiXmlElement* gravElement = componentElement->FirstChildElement( "grav" );
                const char* tmpType = gravElement->Attribute("type");
				std::string typeStr;
                if ( tmpType == NULL )
                    typeStr = "noType";
                else
                    typeStr = tmpType;

                shared_ptr<CompGravField> compGrav( make_shared<CompGravField>() );

				if ( typeStr == "directional" )
				{
					compGrav->SetGravType( Directional );
					
					float gx = 0.0f, gy = 1.0f;
					gravElement->QueryFloatAttribute("gx",&gx);
					gravElement->QueryFloatAttribute("gy",&gy);

					compGrav->SetGravDir( Vector2D( gx, gy ) );
				}
				else if ( typeStr == "radial" )
				{
					compGrav->SetGravType( Radial );
					
					float cx = 0.0f, cy = 1.0f, s=1.0f;
					gravElement->QueryFloatAttribute("cx",&cx);
					gravElement->QueryFloatAttribute("cy",&cy);
					gravElement->QueryFloatAttribute("s",&s);

					compGrav->SetGravCenter( Vector2D( cx, cy ), s );
				}
                component = compGrav;
            }
            else if ( componentId == "CompTrigger" )
            {
                shared_ptr<CompTrigger> compTrig ( make_shared<CompTrigger>() );

                TiXmlElement* condElement = componentElement->FirstChildElement( "Condition" );
                for ( ; condElement ; condElement = condElement->NextSiblingElement( "Condition" ) )
                {
                    const char* condId_temp = condElement->Attribute( "id" );
                    std::string condId;
                    if ( condId_temp != NULL )
                        condId = condId_temp;
                    else
                        continue;

                    if ( condId == "CompareVariable" )
                    {
                        TiXmlElement* paramsElement = condElement->FirstChildElement( "params" );
                        std::string varName;
                        std::string strCompareType;
                        CompareOperator compareType = EqualTo;
                        int numToCompare = 0;
                        if (paramsElement)
                        {
                            const char* varName_temp = paramsElement->Attribute("var");
                            if ( varName_temp != NULL )
                                varName = varName_temp;
                            else
                                continue;
                            const char* strCompareType_temp = paramsElement->Attribute("compare");
                            if ( strCompareType_temp != NULL )
                                strCompareType = strCompareType_temp;
                            else
                                continue;
                            paramsElement->QueryIntAttribute( "num", &numToCompare );
                            if ( strCompareType=="gt" )
                                compareType = GreaterThan;
                            else if ( strCompareType=="gtoet" )
                                compareType = GreaterThanOrEqualTo;
                            else if ( strCompareType=="lt" )
                                compareType = LessThan;
                            else if ( strCompareType=="ltoet" )
                                compareType = LessThanOrEqualTo;
                            else if ( strCompareType=="et" )
                                compareType = EqualTo;
                            else if ( strCompareType=="net" )
                                compareType = NotEqualTo;
                            else
                                gAaLog.Write ( "WARNING: No compare operation with name \"%s\" found!", strCompareType.c_str() ); 
                        }
                        shared_ptr<ConditionCompareVariable> pCompare ( make_shared<ConditionCompareVariable>( pGameWorld->GetItToVariable(varName),compareType,numToCompare) );
                        compTrig->AddCondition( pCompare );
                    }
                    else if ( condId == "EntityTouchedThis" )
                    {
                        TiXmlElement* paramsElement = condElement->FirstChildElement( "params" );
                        std::string entityName;
                        if (paramsElement)
                        {
                            const char* entityName_temp = paramsElement->Attribute("entity");
                            if ( entityName_temp != NULL )
                                entityName = entityName_temp;
                            else
                                continue;
                        }
                        shared_ptr<ConditionEntityTouchedThis> pTouched ( make_shared<ConditionEntityTouchedThis>(entityName) );
                        compTrig->AddCondition( pTouched );
                    }
                    else
                        gAaLog.Write ( "WARNING: No condition found with id \"%s\"!", condId.c_str() );
                }

                TiXmlElement* effectElement = componentElement->FirstChildElement( "Effect" );
                for ( ; effectElement ; effectElement = effectElement->NextSiblingElement( "Effect" ) )
                {
                    const char* effctId_temp = effectElement->Attribute( "id" );
                    std::string effctId;
                    if ( effctId_temp != NULL )
                        effctId = effctId_temp;
                    else
                        continue;

                    if ( effctId == "KillEntity" )
                    {
                        TiXmlElement* paramsElement = effectElement->FirstChildElement( "params" );
                        const char* entity = paramsElement->Attribute( "entity" );

                        shared_ptr<EffectKillEntity> pDestroy ( make_shared<EffectKillEntity>( entity, pGameWorld ) );
                        compTrig->AddEffect( pDestroy );
                    }
                    else if ( effctId == "DispMessage" )
                    {
                        TiXmlElement* paramsElement = effectElement->FirstChildElement( "params" );
                        const char* msg = paramsElement->Attribute( "msg" );
                        int time = 3000;
                        paramsElement->QueryIntAttribute( "timems", &time );
                        shared_ptr<EffectDispMessage> pMsg ( make_shared<EffectDispMessage>( msg, time, pGameWorld ) );
                        compTrig->AddEffect( pMsg );
                    }
                    else if ( effctId == "EndLevel" )
                    {
                        TiXmlElement* paramsElement = effectElement->FirstChildElement( "params" );
                        const char* msg = paramsElement->Attribute( "msg" );
                        int win = 0;
                        paramsElement->QueryIntAttribute( "win", &win );
                        shared_ptr<EffectEndLevel> pEndGame ( make_shared<EffectEndLevel>( msg, win==1 ) );
                        compTrig->AddEffect( pEndGame );
                    }
                    /*else if ( effctId == "LoseLevel" )
                    {
                        TiXmlElement* paramsElement = effectElement->FirstChildElement( "params" );
                        const char* msg = paramsElement->Attribute( "msg" );
                        shared_ptr<EffectLoseLevel> pEndGame ( make_shared<EffectLoseLevel>( msg ) );
                        compTrig->AddEffect( pEndGame );
                    }*/
                    else if ( effctId == "ChangeVariable" )
                    {
                        TiXmlElement* paramsElement = effectElement->FirstChildElement( "params" );
                        std::string varName;
                        std::string strChangeType;
                        ChangeType changeType = Set;
                        int num = 0;
                        if (paramsElement)
                        {
                            const char* varName_temp = paramsElement->Attribute("var");
                            if ( varName_temp != NULL )
                                varName = varName_temp;
                            else
                                continue;
                            const char* strChangeType_temp = paramsElement->Attribute("change");
                            if ( strChangeType_temp != NULL )
                                strChangeType = strChangeType_temp;
                            else
                                continue;
                            paramsElement->QueryIntAttribute( "num", &num );
                            if ( strChangeType=="set" )
                                changeType = Set;
                            else if ( strChangeType=="increase" )
                                changeType = Increase;
                            else if ( strChangeType=="multiply" )
                                changeType = Multiply;
                            else if ( strChangeType=="divide" )
                                changeType = Divide;
                            else
                                gAaLog.Write ( "WARNING: No change operation with name \"%s\" found!", strChangeType.c_str() ); 
                        }
                        shared_ptr<EffectChangeVariable> pChange ( make_shared<EffectChangeVariable>( pGameWorld->GetItToVariable(varName),changeType,num) );
                        compTrig->AddEffect( pChange );
                    }
                    else
                        gAaLog.Write ( "WARNING: No effect found with id \"%s\"!", effctId.c_str() );
                }
                component = compTrig;
            }
            else
            {
                gAaLog.Write ( "Component ID not found!" );
                continue;
            }

            if ( !component )
            {
                gAaLog.Write ( "Error, component was not allocated!" );
                continue;
            }

            if ( componentName )
            	component->SetName( componentName );
            pEntity->AddComponent( component );

            gAaLog.Write ( "[ Done ]\n" );
        }

        pGameWorld->AddEntity( pEntity );
        gAaLog.DecreaseIndentationLevel();
    }

    gAaLog.DecreaseIndentationLevel();
    gAaLog.Write ( "[ Done loading XML file \"%s\" ]\n\n", pFileName );
}

void XmlLoader::LoadSlideShow( const char* pFileName, SlideShow* pSlideShow )
{
    // OPEN XML
    gAaLog.Write ( "Loading XML file \"%s\"...\n", pFileName );
    gAaLog.IncreaseIndentationLevel();

    TiXmlDocument doc(pFileName);
	if (!doc.LoadFile())
	{
		gAaLog.Write ( "[ Error opening file! ]\n\n", pFileName );
		return;
	}

	TiXmlHandle hRoot(0);
    {
        TiXmlElement* pElem = doc.FirstChildElement();
	    if (!pElem)
		{
			gAaLog.Write ( "[ Error reading XML! ]\n\n", pFileName );
			return;
		}
	    hRoot=TiXmlHandle(pElem);
    }
    // END OF OPEN XML

    pSlideShow->musicFileName = "";
    pSlideShow->timerDelay = 0;

    TiXmlElement* musicElement = hRoot.FirstChildElement( "music" ).ToElement();
    if ( musicElement )
        pSlideShow->musicFileName = musicElement->Attribute( "file" );

    TiXmlElement* timerElement = hRoot.FirstChildElement( "timer" ).ToElement();
    if ( timerElement )
        timerElement->QueryIntAttribute( "delayms", &pSlideShow->timerDelay );

    TiXmlElement* slideElement = hRoot.FirstChildElement( "slide" ).ToElement();
    for ( ; slideElement ; slideElement = slideElement->NextSiblingElement( "slide" ) )
    {
        Slide slide;
        slide.imageFileName = slideElement->Attribute( "image" );
        std::string text;
        
        TiXmlNode* child = slideElement->FirstChild();
        for ( ; child; child = child->NextSibling() )
        {
            int t = child->Type(); // ELEMENT=1 TEXT=3

            if ( t == TiXmlNode::/*TINYXML_*/ELEMENT ) // ELEMENT changed to TINYXML_ELEMENT in tinyxml Revision 1.102
            {
                TiXmlElement* el = child->ToElement();
                const char* value_temp = el->Value();
                std::string value;
                if ( value_temp != NULL )
                    value = value_temp;
                if ( value == "n" )
                    text += "\n";
                else if (value == "p" && !text.empty())
                {
                    slide.textPages.push_back(text);
                    text.clear();
                }
            }
            else if ( t == TiXmlNode::/*TINYXML_*/TEXT )
            {
                TiXmlText* tx = child->ToText();
                text += tx->Value();
            }
        }
        if (!text.empty())
            slide.textPages.push_back(text);
        pSlideShow->slides.push_back( slide );
    }

    gAaLog.DecreaseIndentationLevel();
    gAaLog.Write ( "[ Done loading XML file \"%s\" ]\n\n", pFileName );
}

void XmlLoader::LoadGraphics( const char* pFileName, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager )
{
    // OPEN XML
    gAaLog.Write ( "Loading XML file \"%s\"...\n", pFileName );
    gAaLog.IncreaseIndentationLevel();

    TiXmlDocument doc(pFileName);
	if (!doc.LoadFile())
	{
		gAaLog.Write ( "[ Error opening file! ]\n\n", pFileName );
		return;
	}

	TiXmlHandle hRoot(0);
    {
        TiXmlElement* pElem = doc.FirstChildElement();
	    if (!pElem)
		{
			gAaLog.Write ( "[ Error reading XML! ]\n\n", pFileName );
			return;
		}
	    hRoot=TiXmlHandle(pElem);
    }
    // END OF OPEN XML

    bool noMipmaps = false;
    TiXmlElement* noMipmapsElement = hRoot.FirstChildElement( "noMipmaps" ).ToElement();
    if ( noMipmapsElement )
        noMipmaps = true;

    // Texturen laden
    if ( pTextureManager )
    {
        TiXmlElement* texElement = hRoot.FirstChildElement( "Texture" ).ToElement();
        for ( ; texElement ; texElement = texElement->NextSiblingElement( "Texture" ) )
        {
            const char* name = texElement->Attribute("file");
            const char* id = texElement->Attribute("id");
            int mipMaps = 0;
            texElement->QueryIntAttribute("mipMaps",&mipMaps);
            int texRepeat = 0; // deprecated
            int texRepeatX = 0;
            int texRepeatY = 0;
            texElement->QueryIntAttribute("texRepeat",&texRepeat);
            texElement->QueryIntAttribute("texRepeatX",&texRepeatX);
            texElement->QueryIntAttribute("texRepeatY",&texRepeatY);
            if (texRepeat)
            {
                texRepeatX = 1;
                texRepeatY = 1;
            }
            float scale = 1.0f;
            texElement->QueryFloatAttribute("scale",&scale);

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

            pTextureManager->LoadTexture(name,id,info,gAaConfig.GetInt("TexQuality"));
        }
    }

    // Animationen laden
    if ( pAnimationManager )
    {
        TiXmlElement* animElement = hRoot.FirstChildElement( "Animation" ).ToElement();
        for ( ; animElement ; animElement = animElement->NextSiblingElement( "Animation" ) )
        {
            const char* name = animElement->Attribute("file");
            const char* id = animElement->Attribute("id");
            int mipMaps = 0;
            animElement->QueryIntAttribute("mipMaps",&mipMaps);
            int texRepeat = 0; // deprecated
            int texRepeatX = 0;
            int texRepeatY = 0;
            animElement->QueryIntAttribute("texRepeat",&texRepeat);
            animElement->QueryIntAttribute("texRepeatX",&texRepeatX);
            animElement->QueryIntAttribute("texRepeatY",&texRepeatY);
            if (texRepeat)
            {
                texRepeatX = 1;
                texRepeatY = 1;
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

            pAnimationManager->LoadAnimation(name,id,info,gAaConfig.GetInt("TexQuality"));
        }
    }

    // Schriften laden
    if ( pFontManager )
    {
        TiXmlElement* fontElement = hRoot.FirstChildElement( "Font" ).ToElement();
        for ( ; fontElement ; fontElement = fontElement->NextSiblingElement( "Font" ) )
        {
            const char* name = fontElement->Attribute("file");
            const char* id = fontElement->Attribute("id");
            int size = 0;
            fontElement->QueryIntAttribute("size",&size);

            pFontManager->LoadFont(name,size,id);
        }
    }
    gAaLog.DecreaseIndentationLevel();
    gAaLog.Write ( "[ Done loading XML file \"%s\" ]\n\n", pFileName );
}

void XmlLoader::UnLoadGraphics( const char* pFileName, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager )
{
    // OPEN XML
    gAaLog.Write ( "Loading XML file \"%s\"...\n", pFileName );
    gAaLog.IncreaseIndentationLevel();

    TiXmlDocument doc(pFileName);
	if (!doc.LoadFile())
	{
		gAaLog.Write ( "[ Error opening file! ]\n\n", pFileName );
		return;
	}

	TiXmlHandle hRoot(0);
    {
        TiXmlElement* pElem = doc.FirstChildElement();
	    if (!pElem)
		{
			gAaLog.Write ( "[ Error reading XML! ]\n\n", pFileName );
			return;
		}
	    hRoot=TiXmlHandle(pElem);
    }
    // END OF OPEN XML

    // Texturen laden
    if ( pTextureManager )
    {
        TiXmlElement* texElement = hRoot.FirstChildElement( "Texture" ).ToElement();
        for ( ; texElement ; texElement = texElement->NextSiblingElement( "Texture" ) )
        {
            const char* id = texElement->Attribute("id");
            pTextureManager->FreeTexture(id);
        }
    }

    // Animationen laden
    if ( pAnimationManager )
    {
        TiXmlElement* animElement = hRoot.FirstChildElement( "Animation" ).ToElement();
        for ( ; animElement ; animElement = animElement->NextSiblingElement( "Animation" ) )
        {
            const char* id = animElement->Attribute("id");
            pAnimationManager->FreeAnimation(id);
        }
    }

    // Schriften laden
    if ( pFontManager )
    {
        TiXmlElement* fontElement = hRoot.FirstChildElement( "Font" ).ToElement();
        for ( ; fontElement ; fontElement = fontElement->NextSiblingElement( "Font" ) )
        {
            const char* id = fontElement->Attribute("id");
            pFontManager->FreeFont(id);
        }
    }
    gAaLog.DecreaseIndentationLevel();
    gAaLog.Write ( "[ Done loading XML file \"%s\" ]\n\n", pFileName );
}

void XmlLoader::SaveWorldToXml( const char* pFileName, GameWorld* pGameWorld )
{
    gAaLog.Write ( "Saving XML file \"%s\"...\n", pFileName );
    gAaLog.IncreaseIndentationLevel();
    
    TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
	
	TiXmlElement* rootElement = new TiXmlElement( "level" );
    rootElement->SetAttribute("name","My level");
	doc.LinkEndChild( rootElement );
	
    const EntityMap* entities = pGameWorld->GetAllEntities();
    for ( EntityMap::const_iterator e_it = entities->begin(); e_it != entities->end(); ++e_it )
    {
        TiXmlElement* entityElement = new TiXmlElement( "Entity" );
        entityElement->SetAttribute( "name", e_it->second->GetId().c_str() );
        rootElement->LinkEndChild( entityElement );

        const ComponentMap* comps = e_it->second->GetAllComponents();
        for ( ComponentMap::const_iterator c_it = comps->begin(); c_it != comps->end(); ++c_it )
        {
            TiXmlElement* compElement = new TiXmlElement( "Component" );
            compElement->SetAttribute( "id", c_it->second->ComponentId().c_str() );
            compElement->SetAttribute( "name", c_it->second->GetName().c_str() );
            entityElement->LinkEndChild( compElement );

            if ( c_it->second->ComponentId() == "CompPhysics" )
            {
                CompPhysics* compPhys = static_cast<CompPhysics*>(c_it->second.get());

                /*{
                    T   iXmlElement* posElement = new TiXmlElement( "pos" );
                    posElement->SetDoubleAttribute( "x", compPhys->GetBody()->GetPosition().x );
                    posElement->SetDoubleAttribute( "y", compPhys->GetBody()->GetPosition().y );
                    posElement->SetDoubleAttribute( "a", compPhys->GetBody()->GetAngle() );
                    compElement->LinkEndChild( posElement );
                }*/

                /* no longer suported
                 * if ( compPhys->IsAllowedToSleep() )
                {
                    TiXmlElement* sleepElement = new TiXmlElement( "dontSleep" );
                    compElement->LinkEndChild( sleepElement );
                }*/

                {
                    TiXmlElement* dampingElement = new TiXmlElement( "damping" );
                    dampingElement->SetDoubleAttribute( "linear", compPhys->GetLinearDamping() );
                    dampingElement->SetDoubleAttribute( "angular", compPhys->GetAngularDamping() );
                    compElement->LinkEndChild( dampingElement );
                }

                if ( compPhys->IsFixedRotation() )
                {
                    TiXmlElement* frElement = new TiXmlElement( "fixedRotation" );
                    compElement->LinkEndChild( frElement );
                }

                if ( compPhys->IsBullet() )
                {
                    TiXmlElement* bulletElement = new TiXmlElement( "isBullet" );
                    compElement->LinkEndChild( bulletElement );
                }

                for ( ShapeInfoVec::const_iterator it = compPhys->GetShapeInfos().begin(); it != compPhys->GetShapeInfos().end(); ++it )
                {
                    TiXmlElement* shapeElement = new TiXmlElement( "shape" );
                    shapeElement->SetAttribute( "comp_name", (*it)->compName ); // Namen der Form
                    compElement->LinkEndChild( shapeElement );

                    {
                        shapeElement->SetDoubleAttribute( "density", (*it)->density );
                        shapeElement->SetDoubleAttribute( "friction", (*it)->friction );
                        shapeElement->SetDoubleAttribute( "restitution", (*it)->restitution );

                        if ( (*it)->isSensor )
                        	shapeElement->SetAttribute("isSensor", "true");
                    }
                }
            }
            else if ( c_it->second->ComponentId() == "CompShape" )
            {

            	CompShape* compShape = static_cast<CompShape*>(c_it->second.get());
            	switch (compShape->GetType()) {
                    case CompShape::Polygon:
                    {
        				TiXmlElement* polyElement = new TiXmlElement( "polygon" );
                    	compElement->LinkEndChild( polyElement );

                    	CompShapePolygon* sp = static_cast<CompShapePolygon*>(compShape);
                        for ( size_t i = 0; i < sp->GetVertexCount(); ++i )
                        {
                            TiXmlElement* vertexElement = new TiXmlElement( "vertex" );
                            vertexElement->SetDoubleAttribute( "x", sp->GetVertex(i)->x );
                            vertexElement->SetDoubleAttribute( "y", sp->GetVertex(i)->y );
                            polyElement->LinkEndChild( vertexElement );
                        }
                        break;
                    }
                    case CompShape::Circle:
                    {
                    	CompShapeCircle* sc = static_cast<CompShapeCircle*>(compShape);

                        TiXmlElement* circleElement = new TiXmlElement( "circle" );
                        circleElement->SetDoubleAttribute( "x", sc->GetCenter().x );
                        circleElement->SetDoubleAttribute( "y", sc->GetCenter().y );
                        circleElement->SetDoubleAttribute( "r", sc->GetRadius() );
                        compElement->LinkEndChild( circleElement );
                        break;
                    }
                    default:
                        break;
                }
            }
            else if ( c_it->second->ComponentId() == "CompVisualTexture" )
            {
                CompVisualTexture* compTex = static_cast<CompVisualTexture*>(c_it->second.get());

                {
                    TiXmlElement* texElement = new TiXmlElement( "tex" );
                    texElement->SetAttribute( "name", compTex->GetTexture().c_str() );
                    compElement->LinkEndChild( texElement );
                }
            }
            else if ( c_it->second->ComponentId() == "CompVisualMessage" )
            {
                CompVisualMessage* compMsg = static_cast<CompVisualMessage*>(c_it->second.get());

                {
                    TiXmlElement* textElement = new TiXmlElement( "msg" );
                    textElement->SetAttribute( "text", compMsg->GetMsg().c_str() );
                    compElement->LinkEndChild( textElement );
                }
            }
            else if ( c_it->second->ComponentId() == "CompPosition" )
            {
                CompPosition* compPos = static_cast<CompPosition*>(c_it->second.get());

                std::stringstream ss;
                ss << compPos->GetPosition().x;
                compElement->SetAttribute( "x", ss.str().c_str() );
                ss.str("");ss.clear();
                ss << compPos->GetPosition().y;
                compElement->SetAttribute( "y", ss.str().c_str() );
                ss.str("");ss.clear();
                ss << compPos->GetOrientation();
                compElement->SetAttribute( "a", ss.str().c_str() );
            }
            else if ( c_it->second->ComponentId() == "CompVisualAnimation" )
            {
                CompVisualAnimation* compAnim = static_cast<CompVisualAnimation*>(c_it->second.get());

                {
                    TiXmlElement* animElement = new TiXmlElement( "anim" );
                    animElement->SetAttribute( "name", compAnim->GetAnimInfo()->name.c_str() );
                    compElement->LinkEndChild( animElement );
                }
                {
                    std::stringstream ss;
                    TiXmlElement* centerElement = new TiXmlElement( "center" );
                    ss << compAnim->Center()->x;
                    centerElement->SetAttribute( "x", ss.str().c_str() );
                    ss.str("");ss.clear();
                    ss << compAnim->Center()->y;
                    centerElement->SetAttribute( "y", ss.str().c_str() );
                    compElement->LinkEndChild( centerElement );
                }
                {
                    std::stringstream ss;
                    TiXmlElement* dimElement = new TiXmlElement( "dim" );
                    float hw = 1.0f, hh = 1.0f;
                    compAnim->GetDimensions( &hw, &hh );
                    ss << hw;
                    dimElement->SetAttribute( "hw", ss.str().c_str() );
                    ss.str("");ss.clear();
                    ss << hh;
                    dimElement->SetAttribute( "hh", ss.str().c_str() );
                    compElement->LinkEndChild( dimElement );
                }
                if ( compAnim->IsRunning() )
                {
                    TiXmlElement* startElement = new TiXmlElement( "start" );
                    compElement->LinkEndChild( startElement );
                }
            }
            else if ( c_it->second->ComponentId() == "CompTrigger" )
            {
                CompTrigger* compTrig = static_cast<CompTrigger*>(c_it->second.get());

                // Alle Kontidionen
                for ( unsigned int i = 0; i < compTrig->GetConditions().size(); ++i )
                {
                    Condition* pCond = compTrig->GetConditions()[i].get();
                    TiXmlElement* condEl = new TiXmlElement( "Condition" );
                    condEl->SetAttribute( "id", pCond->ID().c_str() );
                    compElement->LinkEndChild( condEl );

                    if ( pCond->ID() == "CompareVariable" )
                    {
                        ConditionCompareVariable* condComp = static_cast<ConditionCompareVariable*>(pCond);
                        TiXmlElement* paramsElement = new TiXmlElement( "params" );
                        paramsElement->SetAttribute( "var", condComp->GetVariable().c_str() );
                        switch ( condComp->GetCompareType() )
                        {
                        case GreaterThan:
                            paramsElement->SetAttribute( "compare", "gt" );
                            break;
                        case GreaterThanOrEqualTo:
                            paramsElement->SetAttribute( "compare", "gtoet" );
                            break;
                        case LessThan:
                            paramsElement->SetAttribute( "compare", "lt" );
                            break;
                        case LessThanOrEqualTo:
                            paramsElement->SetAttribute( "compare", "ltoet" );
                            break;
                        case EqualTo:
                            paramsElement->SetAttribute( "compare", "et" );
                            break;
                        case NotEqualTo:
                            paramsElement->SetAttribute( "compare", "net" );
                            break;
                        }
                        std::stringstream ss;
                        ss << condComp->GetNum();
                        paramsElement->SetAttribute( "num", ss.str().c_str() );
                        condEl->LinkEndChild( paramsElement );
                    }
                    else if ( pCond->ID() == "EntityTouchedThis" )
                    {
                        ConditionEntityTouchedThis* condTouched = static_cast<ConditionEntityTouchedThis*>(pCond);
                        TiXmlElement* paramsElement = new TiXmlElement( "params" );
                        paramsElement->SetAttribute( "entity", condTouched->GetEntityName().c_str() );
                        condEl->LinkEndChild( paramsElement );
                    }
                }

                // Alle Effekt
                for ( unsigned int i = 0; i < compTrig->GetEffects().size(); ++i )
                {
                    Effect* pEffect = compTrig->GetEffects()[i].get();
                    TiXmlElement* effctEl = new TiXmlElement( "Effect" );
                    effctEl->SetAttribute( "id", pEffect->ID().c_str() );
                    compElement->LinkEndChild( effctEl );

                    if ( pEffect->ID() == "KillEntity" )
                    {
                        EffectKillEntity* effctKill = static_cast<EffectKillEntity*>(pEffect);
                        TiXmlElement* paramsElement = new TiXmlElement( "params" );
                        paramsElement->SetAttribute( "entity", effctKill->GetEntityName().c_str() );
                        effctEl->LinkEndChild( paramsElement );
                    }
                    else if ( pEffect->ID() == "DispMessage" )
                    {
                        EffectDispMessage* effctMsg = static_cast<EffectDispMessage*>(pEffect);
                        TiXmlElement* paramsElement = new TiXmlElement( "params" );
                        paramsElement->SetAttribute( "msg", effctMsg->GetMessage().c_str() );
                        std::stringstream ss;
                        ss << effctMsg->GetTotalTime();
                        paramsElement->SetAttribute( "timems", ss.str().c_str() );
                        effctEl->LinkEndChild( paramsElement );
                    }
                    else if ( pEffect->ID() == "EndLevel" )
                    {
                        EffectEndLevel* effctWin = static_cast<EffectEndLevel*>(pEffect);
                        TiXmlElement* paramsElement = new TiXmlElement( "params" );
                        paramsElement->SetAttribute( "msg", effctWin->GetMessage().c_str() );
                        std::stringstream ss;
                        ss << (effctWin->GetWin()?1:0);
                        paramsElement->SetAttribute( "win", ss.str().c_str() );
                        effctEl->LinkEndChild( paramsElement );
                    }
                    else if ( pEffect->ID() == "ChangeVariable" )
                    {
                        EffectChangeVariable* effctChange = static_cast<EffectChangeVariable*>(pEffect);
                        TiXmlElement* paramsElement = new TiXmlElement( "params" );
                        paramsElement->SetAttribute( "var", effctChange->GetVariable().c_str() );
                        switch ( effctChange->GetChangeType() )
                        {
                        case Set:
                            paramsElement->SetAttribute( "change", "set" );
                            break;
                        case Increase:
                            paramsElement->SetAttribute( "change", "increase" );
                            break;
                        case Multiply:
                            paramsElement->SetAttribute( "change", "multiply" );
                            break;
                        case Divide:
                            paramsElement->SetAttribute( "change", "divide" );
                            break;
                        }
                        std::stringstream ss;
                        ss << effctChange->GetNum();
                        paramsElement->SetAttribute( "num", ss.str().c_str() );
                        effctEl->LinkEndChild( paramsElement );
                    }
                }

            }
        }
    }

    doc.SaveFile( pFileName );
    doc.Clear();
    
    gAaLog.DecreaseIndentationLevel();
    gAaLog.Write ( "[ Done saving XML file \"%s\" ]\n\n", pFileName );
}

// Astro Attack - Christian Zommerfelds - 2009
