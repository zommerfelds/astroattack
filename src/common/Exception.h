/*
 * Exception.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "common/Logger.h"
#include <string>

class Exception : public std::exception
{
public:
    Exception ( const std::string &msg ) // std::string als Parameter
    : m_msg ( msg )
    {}

    ~Exception() throw() {};

    virtual const std::string& getMsg() const // Abfrage nach dem Errorstring
    {
        return m_msg;
    }

    virtual const char* what() const throw()
    {
        return m_msg.c_str();
    }

private:
    std::string m_msg;
};

//-------------------------------- Funktionen --------------------------------//
// TODO: refactor place

// Plattform-übergreifende Dialog-Box Funktion
// Nur benutzen in schweren Fällen wann die GUI noch nicht bereit ist.
void OsMsgBox( const std::string& message, const std::string& title ); // in OsMsgBox.cpp

#endif
