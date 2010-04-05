/*----------------------------------------------------------\
|                     GameStates.cpp                        |
|                     --------------                        |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "GameStates.h"
#include "main.h"
#include "GameApp.h"

GameState::GameState( SubSystems* pSubSystems )
{
    m_pSubSystems = pSubSystems;
}

StateManager::StateManager()
{
}

StateManager::~StateManager()
{
    // Alle States aufräumen
	while ( !m_states.empty() )
    {
        gAaLog.Write ( "\n=== Cleaning up State \"%s\" ===\n\n", m_states.back()->StateID().c_str() );
		m_states.back()->Cleanup();
		m_states.pop_back();
	}
}

void StateManager::ChangeState( boost::shared_ptr<GameState> pState ) 
{
    gAaLog.Write ( "\n=== Changing State to \"%s\" ===\n\n", pState->StateID().c_str() );

	// Momentaner State aufräumen und löschen
	if ( !m_states.empty() )
    {
		m_states.back()->Cleanup();
		m_states.pop_back();
	}

	// Neuer State speichern und initialisieren
	m_states.push_back( pState );
	m_states.back()->Init();
    m_states.back()->GetSubSystems()->isLoading = true;
}

void StateManager::PushState( boost::shared_ptr<GameState> pState )
{
	// Momentaner State anhalten
	if ( !m_states.empty() )
    {
        gAaLog.Write ( "\n=== Pausing State \"%s\" ===\n\n", m_states.back()->StateID().c_str() );
		m_states.back()->Pause();
	}

    gAaLog.Write ( "\n=== Pushing State \"%s\" ===\n\n", pState->StateID().c_str() );
	// Neuer State speichern und initialisieren
	m_states.push_back( pState );
	m_states.back()->Init();
    m_states.back()->GetSubSystems()->isLoading = true;
}

void StateManager::PopState()
{
	// Momentaner State aufräumen und löschen
	if ( !m_states.empty() )
    {
        gAaLog.Write ( "\n=== Popping State \"%s\" ===\n\n", m_states.back()->StateID().c_str() );
		m_states.back()->Cleanup();
        m_states.back()->GetSubSystems()->isLoading = true;
		m_states.pop_back();
	}

	// Voheriger State wiederaufnehmen
	if ( !m_states.empty() )
    {
        gAaLog.Write ( "\n=== Resuming State \"%s\" ===\n\n", m_states.back()->StateID().c_str() );
		m_states.back()->Resume();
	}
}

// Astro Attack - Christian Zommerfelds - 2009
