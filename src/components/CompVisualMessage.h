/*----------------------------------------------------------\
|                  CompVisualMessage.h                      |
|                  -------------------                      |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Zeigt eine Nachricht auf dem Bildschirm

#ifndef COMPVISUALMESSAGE_H
#define COMPVISUALMESSAGE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include <string>
#include <boost/shared_ptr.hpp>
namespace pugi { class xml_node; }

//--------------------------------------------//
//-------- CompVisualMessage Klasse ----------//
//--------------------------------------------//
class CompVisualMessage : public Component
{
public:
    CompVisualMessage( std::string text );
    ~CompVisualMessage();
    const CompIdType& ComponentId() const { return COMPONENT_ID; }
	static const CompIdType COMPONENT_ID; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompCollectable.cpp)

    const std::string& GetMsg() const { return m_text; }

    static boost::shared_ptr<CompVisualMessage> LoadFromXml(const pugi::xml_node& compElem);
    void WriteToXml(pugi::xml_node& compNode) const;

private:
    std::string m_text;
};
//--------------------------------------------//
//------ Ende CompVisualMessage Klasse -------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
