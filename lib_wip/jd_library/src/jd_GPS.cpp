#include "jd_GPS.h"

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
  if (gps.location.isValid())
  {
    return kGoodGPSRead;
  }
  TRACE();
  DUMP(gps.charsProcessed());
  DUMP(gps.sentencesWithFix());
  DUMP(gps.failedChecksum());
  DUMP(gps.passedChecksum());

  return kReadFailed;
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
  DUMP(gps.date.value());
  DUMP(gps.time.value());
  DUMP(gps.time.isValid());
  DUMP(gps.date.isValid());
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
}

// Get the latitude
double GPS_Get_Lat()
{
  ReadGPS();
  if (gps.location.isValid())
  {
    return (gps.location.lat());
  }
  return (kGPSBadData);
}

// Get the longitude
double GPS_Get_Lng()
{
  ReadGPS();
  if (gps.location.isValid())
  {
    return (gps.location.lng());
  }
  return (kGPSBadData);
}
// Get the altitude
double GPS_Get_Alt()
{
  ReadGPS();
  if (gps.altitude.isValid())
  {
    return (gps.altitude.meters());
  }
  return (kGPSBadData);
}

