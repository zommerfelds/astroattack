/*
 * Configuration.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <boost/property_tree/ptree.hpp>
#include <string>

// the global configuration property tree
extern boost::property_tree::ptree gConfig;

// write config with comments
bool writeConfig(const std::string& fileName, const boost::property_tree::ptree& config);

#endif
