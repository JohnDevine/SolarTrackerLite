#define ARDUINOTRACE_ENABLE false // Enable ArduinoTrace (false = disable, true = enable)
// #define DOUBLERESETDETECTOR_DEBUG  false // Enable DoubleResetDetector debug output (false = disable, true = enable)
#include "jd_global.h"

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#include "jd_IDLib.h"
#include "jd_LEDLib.h"
#include "jd_timing.h" // Provides interruptable delays

//********************  Date and time functions *********************
#include <jd_timeFunctions.hpp>

Timezone myTZ;
Timezone GMTTZ;

//********************  End of Date and time functions *********************

// Calculations for sun posn based on time, lat, long
#include "jd_Sun.h"

//************************  ServoEasing setup ************************************
// Must specify this BEFORE the include of "ServoEasing.hpp"
//#define USE_PCA9685_SERVO_EXPANDER    // Activating this enables the use of the PCA9685 I2C expander chip/board.
//#define USE_SOFT_I2C_MASTER           // Saves 1756 bytes program memory and 218 bytes RAM compared with Arduino Wire
//#define USE_SERVO_LIB                 // If USE_PCA9685_SERVO_EXPANDER is defined, Activating this enables force additional using of regular servo library.
//#define USE_LEIGHTWEIGHT_SERVO_LIB    // Makes the servo pulse generating immune to other libraries blocking interrupts for a longer time like SoftwareSerial, Adafruit_NeoPixel and DmxSimple.
//#define PROVIDE_ONLY_LINEAR_MOVEMENT  // Activating this disables all but LINEAR movement. Saves up to 1540 bytes program memory.
#define DISABLE_COMPLEX_FUNCTIONS // Activating this disables the SINE, CIRCULAR, BACK, ELASTIC, BOUNCE and PRECISION easings. Saves up to 1850 bytes program memory.
#define MAX_EASING_SERVOS 2
//#define DISABLE_MICROS_AS_DEGREE_PARAMETER // Activating this disables microsecond values as (target angle) parameter. Saves 128 bytes program memory.
//#define DISABLE_MIN_AND_MAX_CONSTRAINTS    // Activating this disables constraints. Saves 4 bytes RAM per servo but strangely enough no program memory.
//#define DISABLE_PAUSE_RESUME               // Activating this disables pause and resume functions. Saves 5 bytes RAM per servo.
//#define DEBUG                              // Activating this enables generate lots of lovely debug output for this library.

//#define PRINT_FOR_SERIAL_PLOTTER           // Activating this enables generate the Arduino plotter output from ServoEasing.hpp.
/*
 * Specify which easings types should be available.
 * If no easing is defined, all easings are active.
 * This must be done before the #include "ServoEasing.hpp"
 */
//#define ENABLE_EASE_QUADRATIC
#define ENABLE_EASE_CUBIC
//#define ENABLE_EASE_QUARTIC
//#define ENABLE_EASE_SINE
//#define ENABLE_EASE_CIRCULAR
//#define ENABLE_EASE_BACK
//#define ENABLE_EASE_ELASTIC
//#define ENABLE_EASE_BOUNCE
//#define ENABLE_EASE_PRECISION
//#define ENABLE_EASE_USER

#include "ServoEasing.hpp"

#define START_DEGREE_VALUE 180 // The degree value written to the servo at time of attach.
#define SERVO_SPEED 40       // Servo in degrees per second

ServoEasing AzmithTrackingServo;

//************************ End of Servo Easing setup ************************************

// General Paramaters
const int MAX_DEVICE_ID_LEN = 20;
char deviceID[MAX_DEVICE_ID_LEN];
const int UPDATE_FREQUENCY = 20000;  // Update frequency in milliseconds

// WiFi SSID & password are set on Setup
const int MAXSSIDLEN = 32;          // Note this is 31 + null terminator
char ssid[MAXSSIDLEN];              // Content can be changed
const int MAXPASSLEN = 64;          // Note this is 63 + null terminator
char esp_password[MAXPASSLEN];      // Content can be changed
const char *SSID_PREFIX = "ESZ_";   // This format pointer means data CANNOT be changed
const char *PASSWORD_PREFIX = "JD"; // This format pointer means data CANNOT be changed

// GPS stuff
#include "jd_GPS.h"
int gps_dev_status = 0;

void setup()
{
  // put your setup code here, to run once:

  // Serial.begin(115200);
  // while (!Serial) // wait for serial to come up
  // ;

  ARDUINOTRACE_INIT(115200);
  TRACE();
  // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // it is a good practice to make sure your code sets wifi mode how you want it.

  // Initialise the led pin as an output
  init_led(ESP32_LED_BUILTIN);

  // WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  //******************** Uncomment next line for For Debugging +++++++
  // wm.resetSettings();
  //********************  +++++++

  // Set the SSID. Password and deviceid up
  getUniqueID(ssid, MAXSSIDLEN - 1, SSID_PREFIX);
  getUniqueID(esp_password, MAXPASSLEN - 1, PASSWORD_PREFIX);
  getUniqueID(deviceID, MAX_DEVICE_ID_LEN - 1, "");

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  // res = wm.autoConnect("AutoConnectAP", "password"); // password protected ap
  res = wm.autoConnect(deviceID, esp_password); // password protected ap

  if (!res)
  {
    DUMP("Failed to connect");
    blinkLED(ESP32_LED_BUILTIN, kErrWiFiFailure, true); // system will sit here blinking the LED
    ESP.restart();
  }
  else
  {
    // if you get here you have connected to the WiFi
    blinkLED(ESP32_LED_BUILTIN, kErrWiFiGood, false);
    DUMP("connected...yeey :)");
  }

  // Setup the servos
  AzmithTrackingServo.setSpeed(SERVO_SPEED);
  AzmithTrackingServo.setReverseOperation(true);                               // Servo is upside down
  if (AzmithTrackingServo.attach(PIN_D5, START_DEGREE_VALUE) == INVALID_SERVO) // Set pin to use and initial angle
  {
    // Bad servo connection if here
    DUMP("Cannot connect to Azmith Servo");
    blinkLED(ESP32_LED_BUILTIN, kErrAzServoFailure, true); // system will sit here blinking the LED
  }

  jd_delay(500); // Give servo time to get to initial posn

  //******** Test servo
  AzmithTrackingServo.easeTo(0, SERVO_SPEED);
  DUMP("0 degrees");
  jd_delay(1000);
  AzmithTrackingServo.easeTo(45, SERVO_SPEED);
  DUMP("45 degrees");
  jd_delay(1000);
  AzmithTrackingServo.easeTo(90, SERVO_SPEED);
  DUMP("90 degrees");
  jd_delay(1000);
  AzmithTrackingServo.easeTo(135, SERVO_SPEED);
  DUMP("135 degrees");
  jd_delay(1000);
  AzmithTrackingServo.easeTo(180, SERVO_SPEED);
  DUMP("180 degrees");
  jd_delay(1000);
  //******** End of Test servo

  // AzmithTrackingServo.easeTo(0, SERVO_SPEED); // Move to 179 degrees at default degrees per second, blocking call

  // Initialise GPS and wait to get a valid location
  gps_dev_status = initGPS(UART1, 9600, SERIAL_8N1, PIN_D17, PIN_D16); // Pin 16 (yellow) to GPS rx pin, Pin 17 (green) to GPS tx pin

  if (gps_dev_status != kGoodGPS)
  {
    TRACE();
    DUMP("Bad GPS status");
    DUMP(gps_dev_status);
    blinkLED(ESP32_LED_BUILTIN, kErrGPSInitFailure, true); // system will sit here blinking the LED
  }

// Setup time stuff
#if (ARDUINOTRACE_ENABLE)
  initEZTime(DEBUG, (uint8_t)GPS_Get_time_hour(), (uint8_t)GPS_Get_time_minute(), (uint8_t)GPS_Get_time_second(), GPS_Get_date_day(), GPS_Get_date_month(), GPS_Get_date_year()); // Debug level can be set to NONE, ERROR, INFO, DEBUG
#else
  initEZTime(NONE, (uint8_t)GPS_Get_time_hour(), (uint8_t)GPS_Get_time_minute(), (uint8_t)GPS_Get_time_second(), GPS_Get_date_day(), GPS_Get_date_month(), GPS_Get_date_year()); // Debug level can be set to NONE, ERROR, INFO, DEBUG
#endif

  GMTTZ.setTime((uint8_t)GPS_Get_time_hour(), (uint8_t)GPS_Get_time_minute(), (uint8_t)GPS_Get_time_second(), GPS_Get_date_day(), GPS_Get_date_month(), GPS_Get_date_year());
  // myTZ.setLocation(F("Asia/Bangkok"));
  // myTZ.setPosix(F("ICT-7"));
  GMTTZ.setPosix(F("GMT+0")); // Time from GPS is GMT
  myTZ.setPosix(F("ICT-7"));  // Convert GMT provided by GPS to local
}

void loop()
{
  // put your main code here, to run repeatedly:
  TRACE();
  events(); // Allows events set by ezTime to be executed.
  // This displays information every time a new sentence is correctly encoded.
  gps_dev_status = ReadGPS();
  if (gps_dev_status == kGoodGPSRead)
  {
    testGPS();
  }
  else if (gps_dev_status == kNoGPSFixYet)
  {
    DUMP("No GPS FIX Yet");
  }
  else
  {
    TRACE();
    DUMP("GPS read failed");
    DUMP(gps_dev_status);
    blinkLED(ESP32_LED_BUILTIN, kErrGPSReadFailure, true); // system will sit here blinking the LED
  }

  // Calculate the sun inclination and azmith
  // time_t utc = now();
  // double az, el;
  jd_azel jd_calc_azel;
  jd_DateTime jd_curr_date_time;

  jd_curr_date_time.yr = myTZ.year();
  jd_curr_date_time.mnth = myTZ.month();
  jd_curr_date_time.day = myTZ.day();
  jd_curr_date_time.hr = myTZ.hour();
  jd_curr_date_time.min = myTZ.minute();
  jd_curr_date_time.sec = myTZ.second();
  jd_curr_date_time.offset_gmt = myTZ.getOffset(); // Set to Thailand for now ... to sort how to get TZ later

  // calcHorizontalCoordinates(utc, GPS_Get_Lat(), GPS_Get_Lng(), az, el);

  jd_GetTrackerAzEl(&jd_calc_azel, jd_curr_date_time, GPS_Get_Lat(), GPS_Get_Lng());

  // Position unit to point at current position of the sun
  // AzmithTrackingServo.easeTo(0, SERVO_SPEED);

  DUMP("Setting to face sun");
  AzmithTrackingServo.easeTo((float)jd_calc_azel.az, SERVO_SPEED); // Move to point at the sun at default degrees per second, blocking call
  DUMP("Facing sun");
  DUMP(jd_calc_azel.az);
  DUMP(jd_calc_azel.el);
  DUMP(jd_curr_date_time.yr);
  DUMP(jd_curr_date_time.mnth);
  DUMP(jd_curr_date_time.day);
  DUMP(jd_curr_date_time.hr);
  DUMP(jd_curr_date_time.min);
  DUMP(jd_curr_date_time.sec);
  DUMP(jd_curr_date_time.offset_gmt);

  //******** Set up blinks to let user know working.
  //** 1 long 3 sec blink followed closely by azmith/20 blinks
  blinkLED(ESP32_LED_BUILTIN, 0, false);  // OFF
  blinkLED(ESP32_LED_BUILTIN, 1, false);  // ON
  jd_delay(3000);
  blinkLED(ESP32_LED_BUILTIN, 0, false);  // OFF
  jd_delay(1000);

  blinkLED(ESP32_LED_BUILTIN, jd_calc_azel.az / 20, false);  // Many Blinks
  
  //******** End of Set up blinks to let user know working.

  jd_delay(UPDATE_FREQUENCY);
}
