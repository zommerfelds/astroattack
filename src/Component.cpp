/*
 * Component.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "Component.h"
#include "GameEvents.h" // Steuert die Spielerreignisse

GameEvents* Component::gameEvents = NULL;
