/*
 * GameStates.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "GameState.h"

#include "game/GameApp.h"
#include "common/Logger.h"

GameState::GameState( SubSystems& subSystems ) : m_subSystems ( subSystems )
{}

StateManager::StateManager()
{
}

StateManager::~StateManager()
{
    clear();
}

void StateManager::changeState(boost::shared_ptr<GameState> pState)
{
    log(Info) << "\n=== Changing State to \"" << pState->getId() << "\" ===\n\n";

    // Momentaner State aufräumen und löschen
    if ( !m_states.empty() )
    {
        m_states.back()->cleanup();
        m_states.pop_back();
    }

    // Neuer State speichern und initialisieren
    m_states.push_back( pState );
    m_states.back()->init();
    m_states.back()->getSubSystems().isLoading = true;
}

void StateManager::pushState(boost::shared_ptr<GameState> pState)
{
    // Momentaner State anhalten
    if ( !m_states.empty() )
    {
        log(Info) << "\n=== Pausing State \"" << m_states.back()->getId() << "\" ===\n\n";
        m_states.back()->pause();
    }

    log(Info) << "\n=== Pushing State \"" << pState->getId() << "\" ===\n\n";
    // Neuer State speichern und initialisieren
    m_states.push_back( pState );
    m_states.back()->init();
    m_states.back()->getSubSystems().isLoading = true;
}

void StateManager::popState()
{
    // Momentaner State aufräumen und löschen
    if ( !m_states.empty() )
    {
        log(Info) << "\n=== Popping State \"" << m_states.back()->getId() << "\" ===\n\n";
        m_states.back()->cleanup();
        m_states.back()->getSubSystems().isLoading = true;
        m_states.pop_back();
    }

    // Voheriger State wiederaufnehmen
    if ( !m_states.empty() )
    {
        log(Info) << "\n=== Resuming State \"" << m_states.back()->getId() << "\" ===\n\n";
        m_states.back()->resume();
    }
}

void StateManager::clear()
{
    // Alle States aufräumen
    while ( !m_states.empty() )
    {
        log(Info) << "\n=== Cleaning up State \"" << m_states.back()->getId() << "\" ===\n\n";
        m_states.back()->cleanup();
        m_states.pop_back();
    }
}
