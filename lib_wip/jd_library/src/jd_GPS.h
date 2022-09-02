#ifndef jd_GPS
    #define jd_GPS

    #include "jd_global.h"
    #include "jd_timing.h"

    #include "HardwareSerial.h"
    #include "TinyGPS++.h"

    // Error codes
    #define kGoodGPS 1 // GPS function good
    #define kUARTFailed 10 // UART Failed to come up
    #define kGoodGPSRead 15 // GPS read is good and llocation is valid
    #define kReadFailed 20 // GPS read failed
    #define kGPSBadData 555 // Define bad data (HaHaHa)

    // Serial Ports
    #define UART0 0 // programming uart port on ESP32
    #define UART1 1 
    #define UART2 2

    TinyGPSPlus gps;
    HardwareSerial *SerialGPS;

    // const int GPSWait = 600000; // How long to wait for GPS to sync in milli seconds

    
    const int interval_between_checks = 1000; //How long between checks to see if UART up
    const int max_gps_wait = 5000; // Maximum to wait for UART to get data in ms (must be > interval_between_checks)

    // Initialise the UART and GPS
    int initGPS(int whichSerial, long baudRate, uint32_t config, int8_t rxPin, int8_t txPin);

    // Read the GPS
    int ReadGPS();

    // See if we have a GPS unit attached and running
    bool CheckForGPS(HardwareSerial SerialGPS);

    // Wait for a GPS lock
    void WaitForGPS(TinyGPSPlus gps, HardwareSerial SerialGPS);

    // This provides a test function for the GPS and displays quite a lot of the data available
    void testGPS();

    // Get the latitude
    double GPS_Get_Lat();

    // Get the longitude
    double GPS_Get_Lng();

// Get the altitude
    double GPS_Get_Alt();

#endif // jd_GPS.h