/*
 * GameStates.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GameStates.h"
#include "main.h"
#include "GameApp.h"

GameState::GameState( SubSystems& subSystems ) : m_subSystems ( subSystems )
{}

StateManager::StateManager()
{
}

StateManager::~StateManager()
{
    clear();
}

void StateManager::changeState( const boost::shared_ptr<GameState>& pState )
{
    gAaLog.write ( "\n=== Changing State to \"%s\" ===\n\n", pState->getId().c_str() );

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

void StateManager::pushState( const boost::shared_ptr<GameState>& pState )
{
	// Momentaner State anhalten
	if ( !m_states.empty() )
    {
        gAaLog.write ( "\n=== Pausing State \"%s\" ===\n\n", m_states.back()->getId().c_str() );
		m_states.back()->pause();
	}

    gAaLog.write ( "\n=== Pushing State \"%s\" ===\n\n", pState->getId().c_str() );
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
        gAaLog.write ( "\n=== Popping State \"%s\" ===\n\n", m_states.back()->getId().c_str() );
		m_states.back()->cleanup();
        m_states.back()->getSubSystems().isLoading = true;
		m_states.pop_back();
	}

	// Voheriger State wiederaufnehmen
	if ( !m_states.empty() )
    {
        gAaLog.write ( "\n=== Resuming State \"%s\" ===\n\n", m_states.back()->getId().c_str() );
		m_states.back()->resume();
	}
}

void StateManager::clear()
{
    // Alle States aufräumen
    while ( !m_states.empty() )
    {
        gAaLog.write ( "\n=== Cleaning up State \"%s\" ===\n\n", m_states.back()->getId().c_str() );
        m_states.back()->cleanup();
        m_states.pop_back();
    }
}
