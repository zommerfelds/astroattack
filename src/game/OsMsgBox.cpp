/*
 * OsMsgBox.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include <iostream>
#include <string>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined( __APPLE__ ) && defined( __MACH__ )
#include <CoreFoundation/CoreFoundation.h>
#endif

void OsMsgBox( const std::string& message, const std::string& title )
{

#ifdef WIN32
    // TODO: change to MessageBox (unicode)
    MessageBoxA( NULL, message.c_str(), title.c_str(), MB_OK ); // or use | MB_ICONERROR

#elif defined( __APPLE__ ) && defined( __MACH__ )

    CFStringRef headerRef  = CFStringCreateWithCString(NULL, title.c_str(), strlen(title.c_str()));
    CFStringRef messageRef = CFStringCreateWithCString(NULL, message.c_str(), strlen(message.c_str()));

    CFUserNotificationDisplayAlert(0, kCFUserNotificationNoteAlertLevel, NULL, NULL, NULL, headerRef, messageRef, NULL, NULL, NULL, NULL);

    CFRelease(headerRef);
    CFRelease(messageRef);

#else
    std::cout << title << ":\n" << message << "\n";
#endif

}
