/*
 * GameStates.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Basisklass für Game States

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

struct SubSystems;

typedef std::string GameStateId;

class GameState
{
public:
    GameState( SubSystems& subSystems );
    virtual ~GameState() {};

    virtual const GameStateId& getId() const = 0; // Komponente ID -> Name der Komponente

    virtual void init() = 0;                    // State starten
    virtual void cleanup() = 0;                 // State abbrechen

    virtual void pause() = 0;                   // State anhalten
    virtual void resume() = 0;                  // State wiederaufnehmen

    virtual void update() = 0;                  // Spiel aktualisieren
    virtual void frame( float deltaTime ) = 0;  // pro Frame einmal aufgerufen (am Anfang)
    virtual void draw( float accumulator ) = 0; // Spiel zeichnen

    SubSystems& getSubSystems() { return m_subSystems; }
private:
    SubSystems& m_subSystems;
};

class StateManager
{
public:
    StateManager();
    ~StateManager();

    void changeState(boost::shared_ptr<GameState> pState);
    void pushState(boost::shared_ptr<GameState> pState);
    void popState();
    void clear();

    GameState* getCurrentState() { return m_states.back().get(); }
private:
    // Stack von States
    std::vector< boost::shared_ptr<GameState> > m_states;
};

#endif
