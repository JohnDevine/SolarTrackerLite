/************************************************************************/
/** NOTE: Include this header in the main.cpp and only once.           **/
/** For all other includes use jd_timeFunctions.h                      **/
/************************************************************************/

#include "jd_timeFunctions.h"

void jd_setTime(int EZHour, int EZMinute, int EZSecond, int EZDay, int EZMonth, int EZYear)
{
  myTZ.setTime(EZHour, EZMinute, EZSecond, EZDay, EZMonth, EZYear);
}

void testTime()
{
  TRACE();
#if (ARDUINOTRACE_ENABLE)
  Serial.println();
  Serial.println("UTC:             " + UTC.dateTime());

  Serial.println();
  Serial.println("Time in various internet standard formats ...");
  Serial.println();
  Serial.println("ATOM:        " + myTZ.dateTime(ATOM));
  Serial.println("COOKIE:      " + myTZ.dateTime(COOKIE));
  Serial.println("IS8601:      " + myTZ.dateTime(ISO8601));
  Serial.println("RFC822:      " + myTZ.dateTime(RFC822));
  Serial.println("RFC850:      " + myTZ.dateTime(RFC850));
  Serial.println("RFC1036:     " + myTZ.dateTime(RFC1036));
  Serial.println("RFC1123:     " + myTZ.dateTime(RFC1123));
  Serial.println("RFC2822:     " + myTZ.dateTime(RFC2822));
  Serial.println("RFC3339:     " + myTZ.dateTime(RFC3339));
  Serial.println("RFC3339_EXT: " + myTZ.dateTime(RFC3339_EXT));
  Serial.println("RSS:         " + myTZ.dateTime(RSS));
  Serial.println("W3C:         " + myTZ.dateTime(W3C));
  Serial.println();
  Serial.println(" ... and any other format, like \"" + myTZ.dateTime("l ~t~h~e jS ~o~f F Y, g:i A") + "\"");
#endif
}

// Set up timezone and NTP connetion and debug level from:
// 	NONE,
//	ERROR,
//	INFO,
//	DEBUG
void initEZTime(ezDebugLevel_t debug_level, uint8_t EZDay, uint8_t EZMonth, uint16_t EZYear, uint8_t EZHour, uint8_t EZMinute, uint8_t EZSecond)
{
  TRACE();
  // Set up time server
  // Uncomment the line below to see what it does behind the scenes
  // setDebug(INFO);
#if (ARDUINOTRACE_ENABLE)
  setDebug(debug_level);
#else
  setDebug(NONE);
#endif
  jd_setTime(EZHour, EZMinute, EZSecond, EZDay, EZMonth, EZYear);

  // setInterval(NTPSERVER_QUERY_INTERVAL_SECONDS); // Set the query to NTP server in seconds
  // waitForSync();                                // Wait for NTP time sync

  // *****+++++++++++++++++++++++++ Sort out below problem later!!!!!!!
  // myTZ.setInterval();  // disable NTP server lookup (I use the GPS to get the time)
  // Provide official timezone names
  // https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
  // myTZ.setLocation(MY_TIMEZONE);
  // Wait a little bit to not trigger DDoS protection on server
  // See https://github.com/ropg/ezTime#timezonedropnl
  // delay(5000);

  // Get the time from the GPS unit

  testTime(); // Note that ARDUINOTRACE_ENABLE must be set to true in the sketch to see this output
}
void jd_getCurrentTime(char *out_time, int maxlen, String format)
{

  String tmp_time = myTZ.dateTime(format);
  if (tmp_time.length() + 1 > maxlen)
  {
    tmp_time = tmp_time.substring(0, maxlen);
  }
  tmp_time.toCharArray(out_time, tmp_time.length() + 1);
  Serial.println("Payload String in get: " + tmp_time);
  Serial.println("Length: ");
  Serial.print(tmp_time.length());
  Serial.println("Payload char * in get: ");
  Serial.print(out_time);
}
