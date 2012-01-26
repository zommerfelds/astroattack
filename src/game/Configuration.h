/*
 * Configuration.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <boost/property_tree/ptree.hpp>

// the global configuration property tree
extern boost::property_tree::ptree gConfig;

void loadConfig(const std::string& fileName, boost::property_tree::ptree& config);
void loadDefaultConfig(boost::property_tree::ptree& config);

// write config with comments
bool writeConfig(const std::string& fileName, const boost::property_tree::ptree& config);

#endif
