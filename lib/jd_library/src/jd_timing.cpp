// Timing related functions

#include "jd_timing.h"
unsigned long jd_previousMillis; // Stores the last time since program started
// A non-blocking delay .. time in milli seconds
void jd_delay(unsigned long delayInMs)
{
    jd_previousMillis = millis();
    while (millis() - jd_previousMillis < delayInMs)
    {
    }
    return;
}
void jd_hang()
{
    while (true)
    {
    }
}