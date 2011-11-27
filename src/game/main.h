/*
 * main.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// -> Haupt-Header <-
// Wer etwas in die Hauptlogdatei schreiben möchte oder
// auf Programmeinstellungen zugreifen möchte
// sollte dieser Header inkludieren.

// TODO: refactor this file

#ifndef MAIN_H
#define MAIN_H

// Version des Spieles
#define GAME_NAME "AstroAttack"
#define GAME_VERSION "0.1.0"
#define LOG_FILE_NAME GAME_NAME ".log" // GAME_NAME + .log, also AstroAttack.log

// Bemerkung: in Visual C++ haben viele CRT (C Run-Time Library) Funktionen eine neue sichere Alternative.
// Falls sie zur verfügung stehen (in Visual C++ .NET) sollte man USE_SAFE_CRT_FUNCTIONS definieren,
// dann benutzt AstroAttack automatisch die neuen Funktionen.
// #define USE_SAFE_CRT_FUNCTIONS // ( oder in den Projektoptionen definieren )
// Mehr Info hier: http://msdn2.microsoft.com/de-de/library/8ef0s5kh(VS.80).aspx


//-------------------------------- Globale Variablen --------------------------------//

extern bool gDoRestart; // Ob AstroAttack neu gestartet werden soll, nochdem es beendet wurde

#endif
