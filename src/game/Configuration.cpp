/*
 * Configuration.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "Configuration.h"
#include <fstream>
#include <boost/foreach.hpp>
#include <map>

using boost::property_tree::ptree;

ptree gConfig;

bool writeConfig(const std::string& fileName, const ptree& config)
{
	std::ofstream fout (fileName.c_str());
	if (!fout)
		return false;
	BOOST_FOREACH(const ptree::value_type &v, config)
	{
		std::string name = v.first;
        std::string data = v.second.data();
        if (data.find(' ') != std::string::npos)
        	data = "\"" + data + "\"";

        std::string comment;
        if (name == "AntiAliasing")
        	comment = "Multisample anti-aliasing - 0: off, 2: 2x, 4: 4x...";
        else if (name == "EditorLevel")
        	comment = "The level that will be edited in the level editor";
        else if (name == "FullScreen")
        	comment = "true: full screen, false: windowed";
        else if (name == "MouseSensitivity")
        	comment = "Mouse sensitivity - between 0.0 and 1.0";
        else if (name == "ScreenBpp")
        	comment = "Bits per pixel";
        else if (name == "ScreenWidth")
        	comment = "Resolution width";
        else if (name == "ScreenHeight")
        	comment = "Resolution height";
        else if (name == "TexQuality")
        	comment = "Texture quality from 0 to 4 (0 is best)";
        else if (name == "V-Sync")
        	comment = "Vertical synchronization (fixed FPS)";
        else if (name == "VolMaster")
        	comment = "Master volume - between 0.0 and 1.0";
        else if (name == "VolMusic")
        	comment = "Music volume - between 0.0 and 1.0";
        else if (name == "VolSound")
        	comment = "FX volume - between 0.0 and 1.0";
        else if (name == "WideScreen")
        	comment = "Wide screen mode - true: on, false: off";
        else if (name == "LogConsoleLevel")
        	comment = "Logging level for stdout - Error, Warning, Info, Detail, Debug or Off";
        else if (name == "LogFileLevel")
        	comment = "Logging level for log file - Error, Warning, Info, Detail, Debug or Off";
        else if (name == "LogFileName")
        	comment = "Name of the log file";

        if (!comment.empty())
        	fout << "; " << comment << std::endl;
        fout << name << " " << data << std::endl << std::endl;
	}
	return true;
}
