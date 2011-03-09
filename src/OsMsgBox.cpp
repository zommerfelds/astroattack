/*
 * OsMsgBox.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Dialog-Box für win32, macosx und gewöhnliche command line

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <iostream>
#include <string>

// Das Programm ist plattform-übergreifend, also Text für verschiedene Systeme ausgeben:
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined( __APPLE__ ) && defined( __MACH__ )
#include <Carbon/Carbon.h>
#endif

// Zeigt eine Nachricht message mit Titel title (Platform-unabhängig).
// Kann Windows Dialogboxen, Mac OS X Dialogboxen und die standard Ausgabe verwenden (cout)
void OsMsgBox( const std::string& message, const std::string& title )
{

#ifdef WIN32 // Windows
    // TODO: change to MessageBox (unicode)
    MessageBoxA( NULL, message.c_str(), title.c_str(), MB_OK ); // Dialogbox in Windows anzeigen
    // ( | MB_ICONERROR  für Fehlerdialogbox  )

#elif defined( __APPLE__ ) && defined( __MACH__ ) // Mac OS X
    // Referenz:
    // http://developer.apple.com/documentation/Carbon/Conceptual/HandlingWindowsControls/hitb-wind_cont_tasks/chapter_3_section_4.html

    DialogRef theItem;
    DialogItemIndex itemIndex;

    // Create a string ref of the message:
    CFStringRef msgStrRef = CFStringCreateWithCString ( NULL, message.c_str(), kCFStringEncodingASCII );

    // Create the dialog:
    CreateStandardAlert ( kAlertPlainAlert, CFSTR ( title.c_str() ), msgStrRef, NULL, &theItem );

    // Show it:
    RunStandardAlert ( theItem, NULL, &itemIndex );

    // Release the string:
    CFRelease ( msgStrRef );

#else // Sonnst einfach in standard Ausgabe

    std::cout << title << ":\n" << message << "\n";

#endif

}
