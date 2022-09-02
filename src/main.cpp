#define ARDUINOTRACE_ENABLE true // Enable ArduinoTrace (false = disable, true = enable)
// #define DOUBLERESETDETECTOR_DEBUG  false // Enable DoubleResetDetector debug output (false = disable, true = enable)
#include "jd_global.h"

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#include "jd_IDLib.h"
#include "jd_LEDLib.h"
#include "ServoEasing.hpp"
#include "jd_timing.h"

// General Paramaters
const int MAX_DEVICE_ID_LEN = 20;
char deviceID[MAX_DEVICE_ID_LEN];

// LED blink counts
#define kErrWiFiFailure 3
#define kErrWiFiGood 4

// WiFi SSID & password are set on Setup
const int MAXSSIDLEN = 32;          // Note this is 31 + null terminator
char ssid[MAXSSIDLEN];              // Content can be changed
const int MAXPASSLEN = 64;          // Note this is 63 + null terminator
char esp_password[MAXPASSLEN];      // Content can be changed
const char *SSID_PREFIX = "ESZ_";   // This format pointer means data CANNOT be changed
const char *PASSWORD_PREFIX = "JD"; // This format pointer means data CANNOT be changed

// Servo stuff
ServoEasing AzmithTrackingServo;

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
  wm.resetSettings();

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
  AzmithTrackingServo.attach(PIN_D5, 1); // Set pin to use and initial angle
  jd_delay(500);                         // Give servo time to get to initial posn

  // Test the servos
  AzmithTrackingServo.easeTo(179, 40); // Move to 179 degrees at 40 degrees per second, blocking call
  // jd_delay(10000);  // Give servo time to get to position .. not needed with blocking call above
}

void loop()
{
  // put your main code here, to run repeatedly:
  // TRACE();
}
