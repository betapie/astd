#pragma once
#ifndef ASTD_ERROR
#define ASTD_ERROR

#include <avr/wdt.h>

/*
All of this is not part of the stl in any way. However, since we do not want to use exceptions and
exit() etc. do not behave in a usefull way on arduino devices, we need some other way to check for
runtime errors...
exceptions and false asserts (in Debug) at runtime will print an error and reset the board via the
watchdog.
*/

namespace astd
{
    void reset_board()
    {
        // Set watchdog timer to minimum
        wdt_enable(WDTO_15MS);
        while (true) // Wait for watchdog to time out
        {
        }
    }

    void verify(bool expression, const char* text)
    {
        if (!expression)
        {
#ifdef HardwareSerial_h
            if (Serial)
                Serial.print(text);
#endif
            reset_board();
        }
    }
}
#endif // ASTD_ERROR