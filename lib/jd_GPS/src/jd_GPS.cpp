#include "jd_GPS.h"

TinyGPSPlus gps;
HardwareSerial *SerialGPS;

// Initialise the GPS unit
// 1. set up UART
// 2. Get gps data
// whichSerial is either 1 or 2 for ESP32 ; ESP32 has 3 hardware serial ports, 0 is reserved for onboard programming usb connection
// long baud,
// uint32_t config (For GPS is usually "SERIAL_8N1")
// int8_t rxPin (For me usually Pin 17 (green) to GPS tx pin)
// int8_t txPin (For me Pin 16 (yellow) to GPS rx pin)

int initGPS(int whichSerial, long baudRate, uint32_t config, int8_t rxPin, int8_t txPin)
{
  // setup the UART port
  int wait_so_far = 0;

  SerialGPS = new HardwareSerial(whichSerial);      // choose which hardware serial port (1 or 2)
  SerialGPS->begin(baudRate, config, rxPin, txPin); // configure the UART port

  while (SerialGPS->available() == 0)
  {
    wait_so_far += interval_between_checks;
    if (wait_so_far > max_gps_wait)
    {
      return kUARTFailed;
    }
    jd_delay(interval_between_checks); // Wait for UART to come up
  }

  // Read the GPS data.

  if (ReadGPS() == kReadFailed)
  {
    return kReadFailed;
  }

  return kGoodGPS;
}
int ReadGPS()
{
  // flush the UART buffer first as the system may have been just piling up data from gps unit
  // SerialGPS->flush();  // NOT SO SURE THIS IS NEEDED

  // Read in data character by character and keep encoding it until no data left
  while (SerialGPS->available() > 0)
  {
    gps.encode(SerialGPS->read());
  }

  // check to see if we have valid location data
  if (gps.location.isValid() && gps.date.isValid())  // Note that the location data may be valid before the date comes through
  {
    return kGoodGPSRead;
  }
  // If GPS is good BUT we have not got a fix return "No Fix Yet" status
  if (gps.charsProcessed() > 0)
  {
    return kNoGPSFixYet;
  }
  TRACE();
  DUMP(gps.charsProcessed());
  DUMP(gps.sentencesWithFix());
  DUMP(gps.failedChecksum());
  DUMP(gps.passedChecksum());

  return kReadFailed;
}

// Keep reading the GPS until we get a lock and valid data
int ReadTillValidGPS()
{
  TRACE();
  int GPSStatus;
  while (GPSStatus = ReadGPS() == kNoGPSFixYet)
  {
    TRACE();
    blinkLED(ESP32_LED_BUILTIN, kWaitingOnGPSFix, false); // let user know what is going on
    jd_delay(1000); // Wait a second to give GPS time to read
  }
  return GPSStatus;
}

// Get latitude and longitude
void testGPS()
{
  // TRACE();
  // This sketch displays information every time a new sentence is correctly encoded.

  ReadGPS();

  DUMP(gps.location.lat());
  DUMP(gps.location.lng());
  DUMP(gps.location.isValid());

  DUMP(gps.date.isValid());
  DUMP(gps.date.value());
  DUMP(gps.date.day());
  DUMP(gps.date.month());
  DUMP(gps.date.year());

  DUMP(gps.time.isValid());
  DUMP(gps.time.hour());
  DUMP(gps.time.minute());
  DUMP(gps.time.second());
  DUMP(gps.time.centisecond());

  DUMP(gps.satellites.value());
  DUMP(gps.satellites.isValid());
  DUMP(gps.hdop.value());
  DUMP(gps.hdop.isValid());
  DUMP(gps.altitude.meters());
  DUMP(gps.altitude.isValid());
  DUMP(gps.course.deg());
  DUMP(gps.course.isValid());
  DUMP(gps.speed.kmph());
  DUMP(gps.speed.isValid());

  //***************** From Functions

  DUMP(GPS_Get_Lat());
  DUMP(GPS_Get_Lng());
  DUMP(GPS_Get_Alt());

  DUMP(GPS_Get_date_day());
  DUMP(GPS_Get_date_month());
  DUMP(GPS_Get_date_year());

  DUMP(GPS_Get_time_hour());
  DUMP(GPS_Get_time_minute());
  DUMP(GPS_Get_time_second());
  DUMP(GPS_Get_time_centisecond());
}

// Get the latitude
double GPS_Get_Lat()
{
  ReadTillValidGPS();
  if (gps.location.isValid())
  {
    return (gps.location.lat());
  }
  return (kGPSBadData);
}

// Get the longitude
double GPS_Get_Lng()
{
  ReadTillValidGPS();
  if (gps.location.isValid())
  {
    return (gps.location.lng());
  }
  return (kGPSBadData);
}
// Get the altitude
double GPS_Get_Alt()
{
  ReadTillValidGPS();
  if (gps.altitude.isValid())
  {
    return (gps.altitude.meters());
  }
  return (kGPSBadData);
}

// Get the date_day
int GPS_Get_date_day()
{
  ReadTillValidGPS();
  if (gps.date.isValid())
  {
    return (gps.date.day());
  }
  return (kGPSBadData);
}

// Get the date_month
int GPS_Get_date_month()
{
  ReadTillValidGPS();
  if (gps.date.isValid())
  {
    return (gps.date.month());
  }
  return (kGPSBadData);
}

// Get the date_year
int GPS_Get_date_year()
{
  ReadTillValidGPS();
  if (gps.date.isValid())
  {
    return (gps.date.year());
  }
  return (kGPSBadData);
}

// Get the time_hour
int GPS_Get_time_hour()
{
  ReadTillValidGPS();
  if (gps.time.isValid())
  {
    return (gps.time.hour());
  }
  return (kGPSBadData);
}
// Get the time_minute
int GPS_Get_time_minute()
{
  ReadTillValidGPS();
  if (gps.time.isValid())
  {
    return (gps.time.minute());
  }
  return (kGPSBadData);
}
// Get the time_second
int GPS_Get_time_second()
{
  ReadTillValidGPS();
  if (gps.time.isValid())
  {
    return (gps.time.second());
  }
  return (kGPSBadData);
}

// Get the time_centisecond
int GPS_Get_time_centisecond()
{
  ReadTillValidGPS();
  if (gps.time.isValid())
  {
    return (gps.time.centisecond());
  }
  return (kGPSBadData);
}
